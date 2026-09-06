#include "Engine/Core/Engine.h"
#include "Engine/System/Audio/SoundDatabase.h"
#include "Engine/Utilities/Logger.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <limits>
#include <thread>

// Only the legacy compatibility adapter needs this engine accessor.
namespace { AOENGINE::Audio* testAudio = nullptr; }
AOENGINE::Audio* Engine::GetAudio() { return testAudio; }
void AOENGINE::Logger::Log(const std::string& message) { std::cout << message; }

namespace {
void Check(bool condition, const char* message) {
    if (!condition) { throw std::runtime_error(message); }
}
void WriteWave(const std::filesystem::path& path) {
    std::filesystem::create_directories(path.parent_path());
    std::ofstream file(path, std::ios::binary);
    auto integer = [&](uint32_t value, int bytes) {
        for (int i = 0; i < bytes; ++i) { file.put(static_cast<char>((value >> (8 * i)) & 255)); }
    };
    // 100 ms of mono silence, decoded by MF to the engine's stereo PCM.
    constexpr uint32_t bytes = 8820;
    file.write("RIFF", 4); integer(36 + bytes, 4); file.write("WAVEfmt ", 8);
    integer(16, 4); integer(1, 2); integer(1, 2); integer(44100, 4);
    integer(88200, 4); integer(2, 2); integer(16, 2);
    file.write("data", 4); integer(bytes, 4);
    for (uint32_t i = 0; i < bytes; ++i) { file.put(0); }
}
void WriteSilentMp3(const std::filesystem::path& path) {
    // MPEG-1 Layer III, 128 kbps, 44.1 kHz stereo; zero-length spectral data.
    std::ofstream file(path, std::ios::binary);
    for (int frame = 0; frame < 10; ++frame) {
        const unsigned char header[] = {0xff, 0xfb, 0x90, 0x00};
        file.write(reinterpret_cast<const char*>(header), 4);
        for (int i = 4; i < 417; ++i) { file.put(0); }
    }
}
}

int main() {
    using namespace AOENGINE;
    namespace fs = std::filesystem;
    try {
        Check(SUCCEEDED(CoInitializeEx(nullptr, COINIT_MULTITHREADED)), "COM initialization failed");
        struct ComShutdown { ~ComShutdown() { CoUninitialize(); } } comShutdown;
        const auto root = fs::absolute("Tests/Audio/Generated/fixtures");
        const auto wave = root / "Assets/tone.wav";
        WriteWave(wave);
        SoundCatalog catalog(root);
        SoundDefinition tone{"Tone", "Assets/tone.wav", 0.5f, true, SoundCategory::SE, 1};
        std::string error;
        Check(catalog.Save({tone}, error), "Valid table did not save");
        Check(catalog.Load(error) && catalog.Find("Tone"), "Round trip failed");
        Check(!catalog.Save({tone, tone}, error), "Duplicate names accepted");
        Check(catalog.Find("Tone") != nullptr, "Failed save replaced active table");
        auto invalid = tone; invalid.file = "../outside.wav";
        Check(!catalog.Validate(invalid).empty(), "Path traversal accepted");
        invalid = tone; invalid.volume = std::numeric_limits<float>::quiet_NaN();
        Check(!catalog.Validate(invalid).empty(), "NaN accepted");
        invalid = tone; invalid.maxInstances = 0;
        Check(!catalog.Validate(invalid).empty(), "Zero instance limit accepted");
        { std::ofstream file(catalog.GetTablePath()); file << "{"; }
        Check(!catalog.Load(error) && catalog.Find("Tone"), "Malformed load replaced active table");
        Check(catalog.Save({tone}, error), "Table restore failed");
        Audio audio;
        audio.Init();
        testAudio = &audio;
        Audio::SetMasterVolume(0); // Tests never play audible output.
        auto* database = SoundDatabase::GetInstance();
        auto first = database->GetSharedAudioData(wave, audio);
        Check(first->bufferSize == first->pBuffer.size() && first->bufferSize > 0, "PCM size invalid");
        Check(first->wfex.wFormatTag == WAVE_FORMAT_PCM && first->wfex.nChannels == 2, "PCM format invalid");
        WriteSilentMp3(root / "Assets/silence.mp3");
        const auto mp3 = audio.LoadMP3((root / "Assets/silence.mp3").c_str());
        Check(mp3.bufferSize > 0 && mp3.bufferSize == mp3.pBuffer.size(), "MP3 decode failed");
        auto shared = database->GetSharedAudioData(wave, audio);
        Check(first == shared, "Decoded data was not cached");
        auto refreshed = database->GetSharedAudioData(wave, audio, true);
        Check(first != refreshed && !first->pBuffer.empty(), "Reload invalidated old PCM");
        WriteWave(root / "Assets/other/tone.wav");
        Check(database->GetSharedAudioData(root / "Assets/other/tone.wav", audio) != refreshed, "Same basename collided");
        { std::ofstream bad(root / "Assets/bad.mp3"); bad << "invalid mp3"; }
        bool rejected = false;
        try { audio.LoadMP3((root / "Assets/bad.mp3").c_str()); } catch (...) { rejected = true; }
        Check(rejected, "Malformed audio accepted");

        SoundManager sounds(audio, root);
        sounds.Init();
        auto a = sounds.Play("Tone");
        Check(static_cast<bool>(a) && sounds.IsPlaying(a), "Named playback failed");
        Check(!sounds.Play("Tone"), "Instance limit ignored");
        sounds.Pause(a);
        Check(!sounds.IsPlaying(a) && sounds.IsAlive(a), "Pause lost playback");
        sounds.Resume(a);
        sounds.SetGamePaused(true);
        Check(!sounds.IsPlaying(a), "Game pause ignored");
        auto preview = sounds.Preview(tone);
        Check(sounds.IsPlaying(preview), "Preview paused with game");
        sounds.SetGamePaused(false);
        Check(sounds.IsPlaying(a), "Game resume failed");
        sounds.Stop(a);
        auto b = sounds.Play("Tone");
        sounds.Stop(a);
        Check(sounds.IsAlive(b) && b.generation != a.generation, "Stale handle stopped a new voice");
        sounds.SetGameEnabled(false);
        Check(!sounds.IsAlive(b) && sounds.IsAlive(preview) && !sounds.Play("Tone"), "Scope stop failed");
        sounds.StopScope(SoundScope::Preview);
        Check(!sounds.IsAlive(preview), "Preview stop failed");
        sounds.SetGameEnabled(true);
        Check(!sounds.Play("Missing"), "Unknown name accepted");
        tone.loop = false;
        Check(sounds.GetCatalog().Save({tone}, error), "Cannot save one-shot definition");
        auto shot = sounds.Play("Tone");
        const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(3);
        while (sounds.IsAlive(shot) && std::chrono::steady_clock::now() < deadline) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); sounds.Update();
        }
        Check(!sounds.IsAlive(shot), "One-shot did not finish");
        sounds.Finalize(); sounds.Finalize();
        database->Init(); database->Init();
        audio.Finalize(); audio.Finalize();
        std::cout << "PASS: catalog, PCM decoding/cache, ownership, handles, limits, pause, preview, one-shot cleanup\n";
        return 0;
    } catch (const std::exception& e) { std::cerr << "FAIL: " << e.what() << '\n'; return 1; }
}
