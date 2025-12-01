#ifndef CLIENT_ACTION_H
#define CLIENT_ACTION_H

#include <string>
#include <vector>
#include <utility>
#include <cstdint>
#include "../common/dto/movement.h" 

struct ClientAction {
    enum class Type {
        Move,
        Name,
        Room,
        StartGame,
        ChooseCar,
        Improvement,
        Cheat
    };
    Type type;
    size_t id;
    Movement movement;
    std::string username;
    uint8_t room_cmd;
    uint8_t room_id;
    std::vector<std::pair<std::string, uint8_t>> races;
    uint8_t car_id;
    uint8_t improvement_id;
    uint8_t cheat;
    bool infinite_life; // NUEVO

    ClientAction()
        : type(Type::Move), id(0), movement(), username(), room_cmd(0), room_id(0),
          races(), car_id(0), improvement_id(0), cheat(0), infinite_life(false) {}

    ClientAction(size_t id_, Movement mov)
        : type(Type::Move), id(id_), movement(mov), username(), room_cmd(0), room_id(0),
          races(), car_id(0), improvement_id(0), cheat(0), infinite_life(false) {}

    ClientAction(size_t id_, std::string&& name)
        : type(Type::Name), id(id_), movement(), username(std::move(name)), room_cmd(0), room_id(0),
          races(), car_id(0), improvement_id(0), cheat(0), infinite_life(false) {}

    //Acá sigue llevando el nombre a cargar en el loader map configure
    ClientAction(size_t id_, std::vector<std::pair<std::string, uint8_t>> races_)
        : type(Type::StartGame), id(id_), movement(), username(), room_cmd(0), room_id(0),
          races(std::move(races_)), car_id(0), improvement_id(0), cheat(0), infinite_life(false) {}

    ClientAction(size_t id_, uint8_t cheat_code)
        : type(Type::Cheat), id(id_), movement(), username(), room_cmd(0), room_id(0),
          races(), car_id(0), improvement_id(0), cheat(cheat_code), infinite_life(false) {}
};

#endif
