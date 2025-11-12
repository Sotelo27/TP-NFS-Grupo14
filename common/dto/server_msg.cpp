#include "server_msg.h"

ServerMessage::ServerMessage() 
    : type(Type::Unknown), id(0), x(0), y(0), angle(0.0f), room_id(0) {}

ServerOutMsg::ServerOutMsg() 
    : type(ServerOutType::Ok), id(0), x(0), y(0), angle(0.f), your_id(0), room_id(0) {}
