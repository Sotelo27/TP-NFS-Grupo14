#include "event_resolver.h"

#include "../constants.h"

#define SOUND_DAMAGED_FILE std::string(ASSETS_PATH) + "/audio/game/boom3.wav"
#define SOUND_CHECKPOINT_PASSED_FILE std::string(ASSETS_PATH) + "/audio/game/boom3.wav"

#define MAX_VOLUME 128
#define LIMIT_DISTANCE_SOUND 800

EventResolver::EventResolver(SdlAudioManager& audio_manager,
                             std::unordered_map<size_t, CarInfoGame>& info_players,
                             size_t client_id):
        client_id(client_id), info_players(info_players), audio_manager(audio_manager) {
    sounds_initialization();
    event_dispatch_init();
}

void EventResolver::event_dispatch_init() {
    event_dispatch[EventRaceType::DAMAGED] = [this](EventInfo& event) { event_damaged(event); };
    event_dispatch[EventRaceType::CHECKPOINT_PASSED] = [this](EventInfo& event) {
        event_checkpoint_passed(event);
    };
}

std::string EventResolver::to_string(EventRaceType e) {
    switch (e) {
        case EventRaceType::DAMAGED:
            return "DAMAGED";
        case EventRaceType::CHECKPOINT_PASSED:
            return "CHECKPOINT_PASSED";
        case EventRaceType::IMPROVEMENT_ACQUIRED:
            return "IMPROVEMENT_ACQUIRED";
    }
    return "Unknown";
}

void EventResolver::sounds_initialization() {
    audio_manager.loadSound(to_string(EventRaceType::DAMAGED), SOUND_DAMAGED_FILE);
    audio_manager.loadSound(to_string(EventRaceType::CHECKPOINT_PASSED),
                            SOUND_CHECKPOINT_PASSED_FILE);
}

void EventResolver::event_damaged(EventInfo& event) { 
    std::string id_sound = to_string(EventRaceType::DAMAGED);
    bool play_sound = true;
    if (event.player_id == client_id) {
        audio_manager.setSoundVolume(id_sound, MAX_VOLUME);
    } else {
        play_sound = setup_sound_volume_based_on_distance(id_sound, event.player_id);
    }

    if (play_sound) {
        audio_manager.playSound(id_sound); 
    }
}

bool EventResolver::setup_sound_volume_based_on_distance(const std::string& id_sound,
                                                         size_t event_player_id) {
    auto it_event_player = info_players.find(event_player_id);
    auto it_client_player = info_players.find(client_id);
    if (it_event_player == info_players.end() || it_client_player == info_players.end()) {
        return false;
    }

    CarInfoGame& event_player_info = it_event_player->second;
    CarInfoGame& client_player_info = it_client_player->second;

    int dist = distance(event_player_info.info_car.x,
                        event_player_info.info_car.y,
                        client_player_info.info_car.x,
                        client_player_info.info_car.y);

    if (dist >= LIMIT_DISTANCE_SOUND) {
        return false;
    } 

    int volume = MAX_VOLUME * (LIMIT_DISTANCE_SOUND - dist) / LIMIT_DISTANCE_SOUND;
    audio_manager.setSoundVolume(id_sound, volume);

    return true;
}

void EventResolver::event_checkpoint_passed(EventInfo& event) {
    if (event.player_id != client_id) return;

    audio_manager.playSound(to_string(EventRaceType::CHECKPOINT_PASSED));
}

void EventResolver::resolve_events(std::vector<EventInfo>& events) {
    for (auto& event: events) {
        auto it = event_dispatch.find(static_cast<EventRaceType>(event.event_type));
        if (it != event_dispatch.end()) {
            it->second(event);
        }
    }
}

int EventResolver::distance(int x1, int y1, int x2, int y2) {
    return std::sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));
}