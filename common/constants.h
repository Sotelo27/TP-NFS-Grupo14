#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cstdint>
#include <string_view>

constexpr int EXITO = 0;
constexpr int ERROR = 1;

constexpr uint8_t CODE_CLIENT_NITRO = 0x04;

constexpr uint8_t CODE_SERVER_MSG = 0x10;
constexpr uint8_t CODE_NITRO_ACTIVATED = 0x07;
constexpr uint8_t CODE_NITRO_EXPIRED = 0x08;

// Base protocol: Client -> Server
constexpr uint8_t CODE_C2S_NAME = 0x01;
constexpr uint8_t CODE_C2S_MOVE = 0x02;
constexpr uint8_t CODE_C2S_ROOM = 0x03;  // crear/unirse a sala (payload con subcódigo)

// Base protocol: Server -> Client
constexpr uint8_t CODE_S2C_OK  = 0x30;
constexpr uint8_t CODE_S2C_POS = 0x31;
constexpr uint8_t CODE_S2C_YOUR_ID = 0x32;
constexpr uint8_t CODE_S2C_ROOMS = 0x21; // listado de salas
constexpr uint8_t CODE_S2C_PLAYER_NAME = 0x33; // (id, length, username)

// Subcódigos para CODE_C2S_ROOM
constexpr uint8_t ROOM_CREATE = 0x02;
constexpr uint8_t ROOM_JOIN   = 0x03;

constexpr std::string_view HIT_NITRO = "A car hit the nitro!";
constexpr std::string_view OUT_NITRO = "A car is out of juice.";

constexpr std::string_view CLIENT_ACTION_READ = "read";
constexpr std::string_view CLIENT_ACTION_EXIT = "exit";
constexpr std::string_view CLIENT_ACTION_NITRO = "nitro";

constexpr std::string_view SERVER_MSG = "msg";
constexpr std::string_view SERVER_NITRO_ACTIVATED = "nitro activated";
constexpr std::string_view SERVER_NITRO_EXPIRED = "nitro expired";

constexpr uint8_t ERROR_MESSAGE = 0x00;

constexpr int SHUT_BOTH_CLOSED = 2;

#endif  // CONSTANTS_H
