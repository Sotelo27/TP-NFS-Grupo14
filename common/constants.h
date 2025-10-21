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
