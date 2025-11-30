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
        response.your_id = 42; 
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
    ASSERT_EQ(server_msg.id, 25); 

    server_thread.join();
}

void test_send_and_receive_rooms() {
    Socket skt("3010");
    std::vector<RoomInfo> rooms = {
        {1, 2, 4},
        {2, 1, 4}
    };

    std::thread server_thread([&]() {
        Socket connection = skt.accept();
        ServerProtocol protocol(std::move(connection));
        protocol.send_rooms(rooms);
    });

    ClientProtocol protocol(Socket("localhost", "3010"));
    ServerMessage msg = protocol.receive();
    ASSERT_EQ(msg.type, ServerMessage::Type::Rooms);
    ASSERT_EQ(msg.rooms.size(), 2);
    ASSERT_EQ(msg.rooms[0].id, 1);
    ASSERT_EQ(msg.rooms[1].id, 2);

    server_thread.join();
}

void test_send_and_receive_cars_list() {
    Socket skt("3011");
    std::vector<CarInfo> cars = {
        {1, 120, 10, 80, 50, 90},
        {2, 100, 12, 90, 55, 85}
    };

    std::thread server_thread([&]() {
        Socket connection = skt.accept();
        ServerProtocol protocol(std::move(connection));
        protocol.send_cars_list(cars);
    });

    ClientProtocol protocol(Socket("localhost", "3011"));
    ServerMessage msg = protocol.receive();
    // No parseo completo en el cliente, solo verifica que no crashea y recibe el código.
    ASSERT_EQ(msg.type, ServerMessage::Type::Unknown);

    server_thread.join();
}

void test_send_and_receive_race_start() {
    Socket skt("3012");
    uint8_t map_id = 1;
    std::vector<std::pair<int32_t, int32_t>> checkpoints = {
        {100, 200}, {300, 400}
    };

    std::thread server_thread([&]() {
        Socket connection = skt.accept();
        ServerProtocol protocol(std::move(connection));
        protocol.send_race_start(map_id, (uint8_t)checkpoints.size(), checkpoints);
    });

    ClientProtocol protocol(Socket("localhost", "3012"));
    ServerMessage msg = protocol.receive();
    ASSERT_EQ(msg.type, ServerMessage::Type::RaceStart);
    ASSERT_EQ(msg.map_id, map_id);

    server_thread.join();
}

void test_send_and_receive_results() {
    Socket skt("3013");
    std::vector<PlayerResultTotal> results = {
        {"Alice", 120, 1},
        {"Bob", 130, 2}
    };

    std::thread server_thread([&]() {
        Socket connection = skt.accept();
        ServerProtocol protocol(std::move(connection));
        protocol.send_results(results);
    });

    ClientProtocol protocol(Socket("localhost", "3013"));
    ServerMessage msg = protocol.receive();
    ASSERT_EQ(msg.type, ServerMessage::Type::ResultsFinal);
    ASSERT_EQ(msg.results_total.size(), 2);
    ASSERT_EQ(msg.results_total[0].username, "Alice");
    ASSERT_EQ(msg.results_total[1].username, "Bob");

    server_thread.join();
}

void test_send_and_receive_improvement_ok() {
    Socket skt("3014");
    ImprovementResult result{};
    result.player_id = 7;
    result.improvement_id = 2;
    result.ok = true;
    result.total_penalty_seconds = 5;
    result.current_balance = 1000;

    std::thread server_thread([&]() {
        Socket connection = skt.accept();
        ServerProtocol protocol(std::move(connection));
        protocol.send_improvement_ok(result);
    });

    ClientProtocol protocol(Socket("localhost", "3014"));
    ServerMessage msg = protocol.receive();
    ASSERT_EQ(msg.type, ServerMessage::Type::ImprovementOK);
    ASSERT_EQ(msg.id, 7);
    ASSERT_EQ(msg.result_market_player.improvement_id, 2);
    ASSERT_TRUE(msg.result_market_player.ok);
    ASSERT_EQ(msg.result_market_player.total_penalty_seconds, 5);
    ASSERT_EQ(msg.result_market_player.current_balance, 1000);

    server_thread.join();
}

void test_send_and_receive_players_list() {
    Socket skt("3015");
    std::vector<PlayerInfo> players = {
        {10, "Alice", true, true, 90, 12345},
        {11, "Bob", false, false, 80, 23456}
    };

    std::thread server_thread([&]() {
        Socket connection = skt.accept();
        ServerProtocol protocol(std::move(connection));
        protocol.send_players_list(players);
    });

    ClientProtocol protocol(Socket("localhost", "3015"));
    ServerMessage msg = protocol.receive();
    ASSERT_EQ(msg.type, ServerMessage::Type::PlayersList);
    ASSERT_EQ(msg.players.size(), 2);
    ASSERT_EQ(msg.players[0].player_id, 10);
    ASSERT_EQ(msg.players[0].username, "Alice");
    ASSERT_TRUE(msg.players[0].is_ready);
    ASSERT_TRUE(msg.players[0].is_admin);
    ASSERT_EQ(msg.players[1].player_id, 11);
    ASSERT_EQ(msg.players[1].username, "Bob");
    ASSERT_FALSE(msg.players[1].is_ready);
    ASSERT_FALSE(msg.players[1].is_admin);

    server_thread.join();
}

void test_send_move() {
    Socket skt("3020");
    std::thread server_thread([&]() {
        Socket connection = skt.accept();
        ServerProtocol protocol(std::move(connection));
        ClientMessage msg = protocol.receive();
        ASSERT_EQ(msg.type, ClientMessage::Type::Move);
        ASSERT_EQ(msg.movement, Movement::Up);
    });

    ClientProtocol protocol(Socket("localhost", "3020"));
    protocol.send_move(Movement::Up);

    server_thread.join();
}

void test_send_choose_car() {
    Socket skt("3021");
    std::thread server_thread([&]() {
        Socket connection = skt.accept();
        ServerProtocol protocol(std::move(connection));
        ClientMessage msg = protocol.receive();
        ASSERT_EQ(msg.type, ClientMessage::Type::ChooseCar);
        ASSERT_EQ(msg.car_id, 3);
    });

    ClientProtocol protocol(Socket("localhost", "3021"));
    ClientMessage msg;
    msg.type = ClientMessage::Type::ChooseCar;
    msg.car_id = 3;
    protocol.send_choose_car(msg);

    server_thread.join();
}

void test_send_improvement() {
    Socket skt("3022");
    std::thread server_thread([&]() {
        Socket connection = skt.accept();
        ServerProtocol protocol(std::move(connection));
        ClientMessage msg = protocol.receive();
        ASSERT_EQ(msg.type, ClientMessage::Type::Improvement);
        ASSERT_EQ(msg.improvement, 2);
    });

    ClientProtocol protocol(Socket("localhost", "3022"));
    ClientMessage msg;
    msg.type = ClientMessage::Type::Improvement;
    msg.improvement = 2;
    protocol.send_improvement(msg);

    server_thread.join();
}

void test_send_cheat() {
    Socket skt("3023");
    std::thread server_thread([&]() {
        Socket connection = skt.accept();
        ServerProtocol protocol(std::move(connection));
        ClientMessage msg = protocol.receive();
        ASSERT_EQ(msg.type, ClientMessage::Type::Cheat);
        ASSERT_EQ(msg.cheat, 1);
    });

    ClientProtocol protocol(Socket("localhost", "3023"));
    ClientMessage msg;
    msg.type = ClientMessage::Type::Cheat;
    msg.cheat = 1;
    protocol.send_cheat(msg);

    server_thread.join();
}


