#include "AudioPlayer.h"
#include "Engine/Engine.h"
#include "Engine/System/Audio/SoundDatabase.h"

using namespace AOENGINE;

AudioPlayer::AudioPlayer() {
}

AudioPlayer::~AudioPlayer() {
	Finalize();
}

void AudioPlayer::Finalize() { 
	if (audioData_.pSourceVoice != nullptr) {
		Stop();
		audioData_.pSourceVoice->DestroyVoice();
		audioData_.pSourceVoice = nullptr;
	}
}

void AudioPlayer::Init(const std::string& filePath) {
	pAudio_ = Engine::GetAudio();
	audioData_ = pAudio_->LoadAudio(SoundDatabase::GetInstance()->GetAudioData(filePath));
}

void AudioPlayer::Play(bool isLoop, float volume, bool checkPlaying) {
	pAudio_->PlayAudio(audioData_, isLoop, volume, checkPlaying);
}

void AudioPlayer::Pause() {
	pAudio_->PauseAudio(audioData_.pSourceVoice);
}

void AudioPlayer::ReStart() {
	pAudio_->ReStartAudio(audioData_.pSourceVoice);
}

void AudioPlayer::Stop() {
	pAudio_->StopAudio(audioData_.pSourceVoice);
}

void AudioPlayer::SetVolume(float volume) {
	pAudio_->SetVolume(audioData_.pSourceVoice, volume);
}

bool AudioPlayer::GetIsPlaying() {
	return pAudio_->IsPlaying(audioData_.pSourceVoice);
}

void AudioPlayer::SinglShotPlay(const std::string& filePath, float volume) {
	Engine::GetAudio()->SinglShotPlay(SoundDatabase::GetInstance()->GetAudioData(filePath), volume);
}
