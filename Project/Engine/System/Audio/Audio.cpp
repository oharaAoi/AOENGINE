#include "Audio.h"
#include <mfapi.h>
#include <mfobjects.h>
#include <mfreadwrite.h>
#include <mmreg.h>
#include <cstring>
#include <stdexcept>
#include <string>

using namespace AOENGINE;

float Audio::masterVolume_ = 0.5f;

Audio::~Audio() {
    Finalize();
    if (masterVoice_) { masterVoice_->DestroyVoice(); masterVoice_ = nullptr; }
    xAudio2_.Reset();
}

void Audio::Finalize() {
	for (PlayingSound& sourceVoice : playingSourceList_) {
		if (sourceVoice.pSourceVoice) {
			sourceVoice.pSourceVoice->DestroyVoice();  // ボイスを解放
		}
		if (!sourceVoice.buffer.empty()) {
			sourceVoice.buffer.clear();
		}
	}
	playingSourceList_.clear();
}

/// <summary>
/// 初期化
/// </summary>
void Audio::Init() {
	HRESULT result = S_FALSE;

	// XAudioエンジンのインスタンス
	result = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	if (FAILED(result)) { throw std::runtime_error("Cannot initialize XAudio2."); }
	// マスターボイスを生成
	result = xAudio2_->CreateMasteringVoice(&masterVoice_);
	if (FAILED(result)) { throw std::runtime_error("Cannot create audio output."); }
}

void Audio::Update() {
	for (auto it = playingSourceList_.begin(); it != playingSourceList_.end(); ) {
		XAUDIO2_VOICE_STATE state;
		(*it).pSourceVoice->GetState(&state);

		if (state.BuffersQueued == 0) {
			// ボイスが再生終了した場合
			(*it).pSourceVoice->DestroyVoice();  // リソースの解放
			(*it).buffer.clear();
			it = playingSourceList_.erase(it); // リストから削除
		} else {
			++it; // 次のボイスへ
		}
	}
}

/// <summary>
/// 音源のデータを読む
/// </summary>
/// <param name="filename"></param>
/// <returns></returns>
SoundData Audio::LoadWave(const char* filename) {
    return LoadMP3(ConvertToWideString(filename).c_str());
}

// Media Foundation decodes both WAV and MP3 to stereo 16-bit PCM.
SoundData Audio::LoadMP3(const wchar_t* filename) {
    auto check = [](HRESULT hr) {
        if (FAILED(hr)) { throw std::runtime_error("Audio decode failed (HRESULT " + std::to_string(static_cast<unsigned long>(hr)) + ")."); }
    };
    check(MFStartup(MF_VERSION));
    struct Shutdown { ~Shutdown() { MFShutdown(); } } shutdown;
    ComPtr<IMFSourceReader> reader;
    check(MFCreateSourceReaderFromURL(filename, nullptr, &reader));
    check(reader->SetStreamSelection(MF_SOURCE_READER_ALL_STREAMS, FALSE));
    check(reader->SetStreamSelection(MF_SOURCE_READER_FIRST_AUDIO_STREAM, TRUE));
    ComPtr<IMFMediaType> output;
    check(MFCreateMediaType(&output));
    check(output->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio));
    check(output->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM));
    check(output->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, 16));
    check(output->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, 2));
    check(reader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, output.Get()));
    output.Reset();
    check(reader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, &output));

    SoundData data{};
    UINT32 channels = 0, rate = 0, bits = 0;
    check(output->GetUINT32(MF_MT_AUDIO_NUM_CHANNELS, &channels));
    check(output->GetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, &rate));
    check(output->GetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, &bits));
    if (channels != 2 || bits != 16 || rate == 0) { throw std::runtime_error("Unsupported decoded PCM format."); }
    data.wfex.wFormatTag = WAVE_FORMAT_PCM;
    data.wfex.nChannels = static_cast<WORD>(channels);
    data.wfex.nSamplesPerSec = rate;
    data.wfex.wBitsPerSample = static_cast<WORD>(bits);
    data.wfex.nBlockAlign = static_cast<WORD>(channels * bits / 8);
    data.wfex.nAvgBytesPerSec = rate * data.wfex.nBlockAlign;
    for (;;) {
        ComPtr<IMFSample> sample;
        DWORD flags = 0;
        check(reader->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, nullptr, &flags, nullptr, &sample));
        if (flags & (MF_SOURCE_READERF_ERROR | MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED)) {
            throw std::runtime_error("Audio stream format changed or failed while decoding.");
        }
        if (sample) {
            ComPtr<IMFMediaBuffer> buffer;
            check(sample->ConvertToContiguousBuffer(&buffer));
            DWORD length = 0;
            check(buffer->GetCurrentLength(&length));
            if (length > XAUDIO2_MAX_BUFFER_BYTES - data.pBuffer.size()) {
                throw std::runtime_error("Audio file is too large for in-memory playback.");
            }
            const size_t offset = data.pBuffer.size();
            data.pBuffer.resize(offset + length);
            BYTE* source = nullptr;
            check(buffer->Lock(&source, nullptr, nullptr));
            if (length) { std::memcpy(data.pBuffer.data() + offset, source, length); }
            check(buffer->Unlock());
        }
        if (flags & MF_SOURCE_READERF_ENDOFSTREAM) { break; }
    }
    if (data.pBuffer.empty() || data.pBuffer.size() % data.wfex.nBlockAlign != 0) {
        throw std::runtime_error("Audio contains no complete PCM frames.");
    }
    data.bufferSize = static_cast<unsigned int>(data.pBuffer.size());
    return data;
}

IXAudio2SourceVoice* Audio::CreateSourceVoice(const SoundData& data) {
    IXAudio2SourceVoice* voice = nullptr;
    if (!xAudio2_ || data.pBuffer.empty() || FAILED(xAudio2_->CreateSourceVoice(&voice, &data.wfex))) { return nullptr; }
    return voice;
}

AudioData Audio::LoadAudio(const SoundData& loadAudioData) {
	HRESULT hr;
	AudioData result{};

	result.data.wfex = loadAudioData.wfex;
	result.data.pBuffer = loadAudioData.pBuffer; // コピー（もしくはムーブ）
	result.data.bufferSize = static_cast<uint32_t>(result.data.pBuffer.size());

	hr = xAudio2_->CreateSourceVoice(&result.pSourceVoice, &result.data.wfex);
	assert(SUCCEEDED(hr));

	return result;
}


/// <summary>
/// 音声データの読み込み
/// </summary>
/// <param name="filename"></param>
/// <returns></returns>
SoundData Audio::SoundLoad(const char* filename) {
	SoundData loadData{};
	if (std::strcmp(GetFileExtension(filename), "wav") == 0) {
		loadData = LoadWave(filename);
	} else {
		loadData = LoadMP3(ConvertToWideString(filename).c_str());
	}
	// 読み込んだ音声データをreturn
	SoundData soundData = {};
	soundData.wfex = loadData.wfex;
	soundData.pBuffer = std::move(loadData.pBuffer);
	soundData.bufferSize = loadData.bufferSize;
	
	return soundData;
}

/// <summary>
/// 音声データの解放
/// </summary>
/// <param name="soundData"></param>
void Audio::SoundUnload(SoundData* soundData) {
	soundData->pBuffer.clear();
	soundData->bufferSize = 0;
	soundData->wfex = {};
}

/// <summary>
/// サウンドの再生
/// </summary>
/// <param name="xAudio2"></param>
/// <param name="soundData"></param>
void Audio::SoundPlayWave(const SoundData& soundData) {
    SingleShotPlay(soundData, 1.0f, true);
}

void Audio::PlayAudio(const AudioData& audioData, bool isLoop, float volume, bool checkPlaying) {
	HRESULT result = S_FALSE;

	if (checkPlaying) {
		if (IsPlaying(audioData.pSourceVoice)) {
			return;
		}
	}

	// 再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = audioData.data.pBuffer.data();
	buf.AudioBytes = audioData.data.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	// ループするかしないか
	if (isLoop) {
		buf.LoopCount = XAUDIO2_LOOP_INFINITE; // 無限ループ
	} else {
		buf.LoopCount = 0; // ループしない
	}

	// 波形データの再生
	result = audioData.pSourceVoice->SubmitSourceBuffer(&buf);
	assert(SUCCEEDED(result));
	result = audioData.pSourceVoice->SetVolume(volume * masterVolume_);
	assert(SUCCEEDED(result));
	result = audioData.pSourceVoice->Start();
	assert(SUCCEEDED(result));
}

void Audio::SingleShotPlay(const SoundData& loadAudioData, float volume, bool loop) {
	PlayingSound playingSound{};

	// ★ 再生中保持するバッファにコピー
	playingSound.buffer = loadAudioData.pBuffer;

	HRESULT hr = xAudio2_->CreateSourceVoice(&playingSound.pSourceVoice, &loadAudioData.wfex);
	assert(SUCCEEDED(hr));

	XAUDIO2_BUFFER buf{};
	buf.pAudioData = playingSound.buffer.data();               // ★ ここが超重要
	buf.AudioBytes = (UINT32)playingSound.buffer.size();
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.LoopCount = loop ? XAUDIO2_LOOP_INFINITE : 0;

	hr = playingSound.pSourceVoice->SubmitSourceBuffer(&buf);
	assert(SUCCEEDED(hr));
	hr = playingSound.pSourceVoice->SetVolume(volume * masterVolume_);
	assert(SUCCEEDED(hr));
	hr = playingSound.pSourceVoice->Start();
	assert(SUCCEEDED(hr));

	playingSourceList_.push_back(std::move(playingSound));
}

/// <summary>
/// Stopの再生
/// </summary>
/// <param name="xAudio2"></param>
/// <param name="soundData"></param>
void Audio::StopAudio(IXAudio2SourceVoice* pSourceVoice) {
	pSourceVoice->Stop();
	pSourceVoice->FlushSourceBuffers();
}

/// <summary>
/// soundの一時停止
/// </summary>
/// <param name="xAudio2"></param>
/// <param name="soundData"></param>
void Audio::PauseAudio(IXAudio2SourceVoice* pSourceVoice) {
	HRESULT result;
	result = pSourceVoice->Stop(0);
	assert(SUCCEEDED(result));
}

/// <summary>
/// 一時停止の再開
/// </summary>
/// <param name="xAudio2"></param>
/// <param name="soundData"></param>
void Audio::ReStartAudio(IXAudio2SourceVoice* pSourceVoice) {
	HRESULT result;
	result = pSourceVoice->Start();
	assert(SUCCEEDED(result));
}

/// <summary>
/// 
/// </summary>
/// <param name="pSourceVoice"></param>
void Audio::SetVolume(IXAudio2SourceVoice* pSourceVoice, float volume) {
	HRESULT result = S_FALSE;
	result = pSourceVoice->SetVolume(volume);
	assert(SUCCEEDED(result));
}

/// <summary>
/// IsPlayingの再生
/// </summary>
/// <param name="xAudio2"></param>
/// <param name="soundData"></param>
bool Audio::IsPlaying(IXAudio2SourceVoice* pSourceVoice) {
	XAUDIO2_VOICE_STATE state;
	pSourceVoice->GetState(&state);
	if (state.BuffersQueued > 0) {
		return true;
	} else {
		return false;
	}
}

const char* Audio::GetFileExtension(const char* filename) {
	const char* ext = std::strrchr(filename, '.'); // 最後のピリオドを探す
	if (ext == nullptr) {
		return ""; // 拡張子がない場合は空文字を返す
	}
	return ext + 1; // ピリオドの次の文字から拡張子を返す
}
