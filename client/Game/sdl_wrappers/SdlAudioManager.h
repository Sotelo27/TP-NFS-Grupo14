#ifndef __SDL_AUDIO_MANAGER_H__
#define __SDL_AUDIO_MANAGER_H__

#include <iostream>
#include <string>
#include <unordered_map>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

class AudioManager {
private:
    std::unordered_map<std::string, Mix_Music*> music;
    std::unordered_map<std::string, Mix_Chunk*> sounds;
    std::string current_music;

public:
    AudioManager();

    void loadMusic(const std::string& id, const std::string& file);
    void loadSound(const std::string& id, const std::string& file);

    void playMusic(const std::string& id, int loops = -1, int fade_in_ms = 0);

    void pauseMusic();
    void resumeMusic();

    void setMusicVolume(int volume);

    void playSound(const std::string& id, int loops = 0, int channel = -1);
    void setSoundVolume(const std::string& id, int volume);

    ~AudioManager();
};

#endif
