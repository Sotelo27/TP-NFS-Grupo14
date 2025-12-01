#include "event_resolver.h"

#define SOUND_DAMAGED_FILE std::string(ASSETS_PATH) + "/audio/game/boom3.wav"
#define SOUND_CHECKPOINT_PASSED_FILE std::string(ASSETS_PATH) + "/audio/game/boom3.wav"

EventResolver::EventResolver(SdlAudioManager& audio_manager,
                             std::unordered_map<size_t, CarInfoGame>& info_players,
                             size_t client_id):
        client_id(client_id), info_players(info_players), audio_manager(audio_manager) {
    sounds_initialization();
    event_dispatch_init();
}

void EventResolver::event_dispatch_init() {
    event_dispatch[EventRaceType::DAMAGED] = [this]() { event_damaged(); };
    event_dispatch[EventRaceType::CHECKPOINT_PASSED] = [this]() { event_checkpoint_passed(); };
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

void EventResolver::event_damaged() { audio_manager.playSound(to_string(EventRaceType::DAMAGED)); }

void EventResolver::event_checkpoint_passed() {
    audio_manager.playSound(to_string(EventRaceType::CHECKPOINT_PASSED));
}

void EventResolver::resolve_events(std::vector<EventInfo>& events) {
    for (const auto& event: events) {
        auto it = event_dispatch.find(static_cast<EventRaceType>(event.event_type));
        if (it != event_dispatch.end()) {
            it->second();
        }
    }
}
