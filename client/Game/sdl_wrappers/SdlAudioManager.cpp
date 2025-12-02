#include "SdlAudioManager.h"

#include <algorithm>

#define LIMITED_CHANNELS 16
#define MAX_VOLUME 128

SdlAudioManager::SdlAudioManager() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        throw std::runtime_error("Error initializing SDL: " + std::string(SDL_GetError()));
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        throw std::runtime_error("Error initializing SDL_mixer: " + std::string(Mix_GetError()));
    }

    Mix_AllocateChannels(LIMITED_CHANNELS);
}

SdlAudioManager::~SdlAudioManager() {
    for (auto& par: music) {
        Mix_FreeMusic(par.second);
    }
    music.clear();

    for (auto& par: sounds) {
        Mix_FreeChunk(par.second);
    }
    sounds.clear();

    Mix_CloseAudio();

    SDL_Quit();
}

void SdlAudioManager::loadMusic(const std::string& id, const std::string& file) {
    if (music.find(id) != music.end()) {
        throw std::runtime_error("Music '" + id + "' is already loaded");
    }

    Mix_Music* musica = Mix_LoadMUS(file.c_str());
    if (!musica) {
        throw std::runtime_error("Error loading music '" + file +
                                 "': " + std::string(Mix_GetError()));
    }

    music[id] = musica;
}

void SdlAudioManager::loadSound(const std::string& id, const std::string& file) {
    if (sounds.find(id) != sounds.end()) {
        throw std::runtime_error("Sound '" + id + "' is already loaded");
    }

    Mix_Chunk* sound = Mix_LoadWAV(file.c_str());
    if (!sound) {
        throw std::runtime_error("Error loading sound '" + file +
                                 "': " + std::string(Mix_GetError()));
    }

    sounds[id] = sound;
}

void SdlAudioManager::playMusic(const std::string& id, int loops, int fade_in_ms) {
    auto it = music.find(id);
    if (it == music.end()) {
        throw std::runtime_error("Music '" + id + "' not found");
    }

    if (fade_in_ms > 0) {
        Mix_FadeInMusic(it->second, loops, fade_in_ms);
    } else {
        Mix_PlayMusic(it->second, loops);
    }

    current_music = id;
}

void SdlAudioManager::pauseMusic() { Mix_PauseMusic(); }

void SdlAudioManager::resumeMusic() { Mix_ResumeMusic(); }

void SdlAudioManager::setMusicVolume(int volume) {
    volume = std::max(0, std::min(volume, MAX_VOLUME));

    Mix_VolumeMusic(volume);
}

void SdlAudioManager::playSound(const std::string& id, int loops, int channel) {
    auto it = sounds.find(id);
    if (it == sounds.end()) {
        throw std::runtime_error("Sound '" + id + "' not found");
    }

    int used_channel = Mix_PlayChannel(channel, it->second, loops);
    if (used_channel == -1) {
        throw std::runtime_error("Error playing sound: " + std::string(Mix_GetError()));
    }
}

void SdlAudioManager::setSoundVolume(const std::string& id, int volume) {
    auto it = sounds.find(id);
    if (it == sounds.end()) {
        throw std::runtime_error("Sound '" + id + "' not found");
    }

    volume = std::max(0, std::min(volume, MAX_VOLUME));

    Mix_VolumeChunk(it->second, volume);
}
