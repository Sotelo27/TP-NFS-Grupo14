#include "protocol_tests.h"
#include <gtest/gtest.h>

#include <map>
#include <memory>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "../client/client.h"
#include "../client/client_protocol.h"
#include "../common/dto/client_msg.h"

#include "../common/socket.h"
#include "../server/client_handler.h"
#include "../server/server_protocol.h"
#include "../client/client_protocol.h"

// template <typename T>
// bool receive_number(T& num, Socket& conexion) {
//     uint8_t size = sizeof(T);
//     if (conexion.recvall(&num, size) < size)
//         return false;
//     if constexpr (sizeof(T) == 2)
//         num = ntohs(num);
//     else if constexpr (sizeof(T) == 4)
//         num = ntohl(num);
//     return true;
// }

// void recv_new_game(Socket& skt) {
//     Socket conexion = skt.accept();
//     ServerProtocol protocol(std::move(conexion));
//
// }

void recv_new_name(Socket& skt, const std::string& nombre_esperado) {
    Socket connection = skt.accept();
    ServerProtocol protocol(std::move(connection));
    ClientMessage msg = protocol.receive();
    ASSERT_EQ(msg.type, ClientMessage::Type::Name);
    ASSERT_EQ(msg.username, nombre_esperado);
}

// void recv_new_room(Socket& skt, uint8_t room_id, const std::string& room_name) {
//     Socket connection = skt.accept();
//     ServerProtocol protocol(std::move(connection));
//     ClientMessage msg = protocol.receive();
//     ASSERT_EQ(msg.type, ClientMessage::Type::Room);
//
//     ServerMessage response;
//     response.type = ServerMessage::Type::RoomCreated;
//     response.room_id = room_id;
//
//     RoomInfo room_info;
//     room_info.id = room_id;
//     room_info.name = room_name;
//     response.rooms.push_back(room_info);
//
//     protocol.send(response);
//
// }

void test_send_name() {
    Socket skt("3000");
    std::string nombre_esperado = "Juan217";

    std::thread server_thread([&]() {
        recv_new_name(skt, nombre_esperado);
    });

    ClientProtocol protocol(Socket("localhost", "3000"));
    ClientMessage msg;
    msg.type = ClientMessage::Type::Name;
    msg.username = nombre_esperado;
    protocol.send_name(msg);

    server_thread.join();
}

void test_recv_my_id() {
    Socket skt("3000");

    std::thread server_thread([&]() {
        Socket connection = skt.accept();
        ServerProtocol protocol(std::move(connection));

        ServerOutMsg response;
        response.type = ServerOutType::YourId;
        response.id = 42;
        protocol.send_your_id(response);
    });


    ClientProtocol protocol(Socket("localhost", "3000"));

    ServerMessage server_msg = protocol.receive();
    ASSERT_EQ(server_msg.type, ServerMessage::Type::YourId);

    ASSERT_EQ(server_msg.id, 42);

    server_thread.join();
}


void test_create_1_room() {
    Socket skt("3000");

    std::thread server_thread([&]() {
        Socket connection = skt.accept();
        ServerProtocol protocol(std::move(connection));
        ClientMessage msg = protocol.receive();
        ASSERT_EQ(msg.type, ClientMessage::Type::Room);
        ASSERT_EQ(msg.room_cmd, ROOM_CREATE);

        ServerOutMsg response;
        response.type = ServerOutType::RoomCreated;
        response.room_id = 25;
        protocol.send_room_created(response);
    });

    ClientProtocol protocol(Socket("localhost", "3000"));
    ClientMessage msg;
    msg.type = ClientMessage::Type::Room;
    msg.room_cmd = ROOM_CREATE;
    protocol.send_create_room();

    ServerMessage server_msg = protocol.receive();
    ASSERT_EQ(server_msg.type, ServerMessage::Type::RoomCreated);
    ASSERT_EQ(server_msg.room_id, 25);

    server_thread.join();
}
