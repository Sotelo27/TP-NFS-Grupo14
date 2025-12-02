#ifndef EVENT_RESOLVER_H
#define EVENT_RESOLVER_H

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include "../../../common/enum/event.h"
#include "../info_game.h"
#include "../sdl_wrappers/SdlAudioManager.h"

class EventResolver {
private:
    size_t client_id;
    std::unordered_map<size_t, CarInfoGame>& info_players;
    SdlAudioManager& audio_manager;
    std::unordered_map<EventRaceType, std::function<void(EventInfo&)>> event_dispatch;

    void sounds_initialization();
    void event_dispatch_init();

    std::string to_string(EventRaceType e);

    void event_damaged(EventInfo& event);
    bool setup_sound_volume_based_on_distance(const std::string& id_sound, size_t event_player_id);
    void event_checkpoint_passed(EventInfo& event);

    int distance(int x1, int y1, int x2, int y2);
public:
    explicit EventResolver(SdlAudioManager& audio_manager,
                           std::unordered_map<size_t, CarInfoGame>& info_players, size_t client_id);

    void resolve_events(std::vector<EventInfo>& events);
};

#endif  // MAPS_TEXTURES_H
