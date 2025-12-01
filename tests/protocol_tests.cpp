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

void test_send_exit() {
    Socket skt("3024");
    std::thread server_thread([&]() {
        Socket connection = skt.accept();
        ServerProtocol protocol(std::move(connection));
        ClientMessage msg = protocol.receive();
        ASSERT_EQ(msg.type, ClientMessage::Type::Exit);
    });

    ClientProtocol protocol(Socket("localhost", "3024"));
    protocol.send_exit();

    server_thread.join();
}

void test_send_and_receive_map_info() {
    Socket skt("3025");
    std::vector<PlayerTickInfo> players_tick = {
        {
            "Alice",      // username
            1,            // car_id
            10,           // player_id
            100,          // x
            200,          // y
            0.0f,         // angle (float)
            90,           // health (uint8_t)
            100,          // max_health (uint8_t)
            10,           // speed_mps (uint16_t o float según tu struct, usa 10 si es uint16_t)
            50,           // x_checkpoint (uint16_t)
            60,           // y_checkpoint (uint16_t)
            0,            // hint_angle_deg (CAMBIADO de 0.0f a 0, si es uint16_t)
            1,            // position_in_race (uint16_t)
            5,            // distance_to_checkpoint (CAMBIADO de 5.0f a 5, si es uint16_t)
            2,            // checkpoints_remaining (uint16_t)
            {}            // improvements
        }
    };
    std::vector<NpcTickInfo> npcs_tick = {
        {1, 300, 400}
    };
    std::vector<EventInfo> events_tick = {
        {1, 10}
    };
    TimeTickInfo race_time{123};

    std::thread server_thread([&]() {
        Socket connection = skt.accept();
        ServerProtocol protocol(std::move(connection));
        protocol.send_map_info(players_tick, npcs_tick, events_tick, race_time);
    });

    ClientProtocol protocol(Socket("localhost", "3025"));
    ServerMessage msg = protocol.receive();
    ASSERT_EQ(msg.type, ServerMessage::Type::MapInfo);
    ASSERT_EQ(msg.players_tick.size(), 1);
    ASSERT_EQ(msg.players_tick[0].username, "Alice");
    ASSERT_EQ(msg.npcs_tick.size(), 1);
    ASSERT_EQ(msg.npcs_tick[0].npc_id, 1);
    ASSERT_EQ(msg.events_tick.size(), 1);
    ASSERT_EQ(msg.events_tick[0].player_id, 10u);
    ASSERT_EQ(msg.race_time.seconds, 123);

    server_thread.join();
}

void test_send_and_receive_game_over() {
    Socket skt("3026");
    std::thread server_thread([&]() {
        Socket connection = skt.accept();
        ServerProtocol protocol(std::move(connection));
        protocol.send_ok();
        protocol.send_game_over(); // Usa el método del protocolo
    });

    ClientProtocol protocol(Socket("localhost", "3026"));
    protocol.receive(); 
    ServerMessage msg = protocol.receive();
    ASSERT_EQ(msg.type, ServerMessage::Type::GameOver);

    server_thread.join();
}

void test_send_and_receive_player_name() {
    Socket skt("3027");
    std::string username = "Bob";
    uint32_t pid = 77;

    std::thread server_thread([&]() {
        Socket connection = skt.accept();
        ServerProtocol protocol(std::move(connection));
        ServerOutMsg msg;
        msg.type = ServerOutType::PlayerName;
        msg.id = pid;
        msg.username = username;
        protocol.send_player_name(msg);
    });

    ClientProtocol protocol(Socket("localhost", "3027"));
    ServerMessage msg = protocol.receive();
    ASSERT_EQ(msg.type, ServerMessage::Type::PlayerName);
    ASSERT_EQ(msg.id, pid);
    ASSERT_EQ(msg.username, username);

    server_thread.join();
}

void test_send_and_receive_market_time() {
    Socket skt("3028");
    TimeTickInfo tti{456};

    std::thread server_thread([&]() {
        Socket connection = skt.accept();
        ServerProtocol protocol(std::move(connection));
        protocol.send_market_time(tti);
    });

    ClientProtocol protocol(Socket("localhost", "3028"));
    ServerMessage msg = protocol.receive();
    ASSERT_EQ(msg.type, ServerMessage::Type::MarketTime);
    ASSERT_EQ(msg.race_time.seconds, 456);

    server_thread.join();
}

void test_send_and_receive_result_race_current() {
    Socket skt("3029");
    std::vector<PlayerResultCurrent> results = {
        {10, "Alice", 12, 120, 1},
        {11, "Bob", 13, 130, 2}
    };

    std::thread server_thread([&]() {
        Socket connection = skt.accept();
        ServerProtocol protocol(std::move(connection));
        ServerOutMsg msg;
        msg.type = ServerOutType::Results;
        msg.results_current = results;
        protocol.send_result_race_current(results);
    });

    ClientProtocol protocol(Socket("localhost", "3029"));
    ServerMessage msg = protocol.receive();
    ASSERT_EQ(msg.type, ServerMessage::Type::Results);
    ASSERT_EQ(msg.results_current.size(), 2);
    ASSERT_EQ(msg.results_current[0].player_id, 10);
    ASSERT_EQ(msg.results_current[1].username, "Bob");

    server_thread.join();
}

void test_concurrent_clients_send_name() {
    const int N = 8;
    std::vector<std::thread> server_threads;
    std::vector<std::thread> client_threads;
    std::vector<std::string> names;
    for (int i = 0; i < N; ++i) names.push_back("User" + std::to_string(i));

    Socket skt("3050");
    std::atomic<int> ok_count{0};
    std::mutex name_mutex;
    std::vector<std::string> received_names;

    // Server threads: cada uno espera un nombre y lo valida 
    for (int i = 0; i < N; ++i) {
        server_threads.emplace_back([&]() {
            Socket connection = skt.accept();
            ServerProtocol protocol(std::move(connection));
            ClientMessage msg = protocol.receive();
            if (msg.type == ClientMessage::Type::Name) {
                std::lock_guard<std::mutex> lk(name_mutex);
                received_names.push_back(msg.username);
            }
        });
    }

    // Client threads: cada uno envía su nombre
    for (int i = 0; i < N; ++i) {
        client_threads.emplace_back([&, i]() {
            ClientProtocol protocol(Socket("localhost", "3050"));
            ClientMessage msg;
            msg.type = ClientMessage::Type::Name;
            msg.username = names[i];
            protocol.send_name(msg);
        });
    }

    for (auto& t : client_threads) t.join();
    for (auto& t : server_threads) t.join();

    // Verifica que todos los nombres enviados fueron recibidos 
    std::sort(names.begin(), names.end());
    std::sort(received_names.begin(), received_names.end());
    ASSERT_EQ(received_names.size(), names.size());
    ASSERT_EQ(received_names, names);
}

void test_client_disconnect_and_reconnect() {
    Socket skt("3051");
    std::atomic<bool> received_first{false};
    std::atomic<bool> received_second{false};

    // Server thread: espera dos conexiones, cada una debe mandar un nombre
    std::thread server_thread([&]() {
        for (int i = 0; i < 2; ++i) {
            Socket connection = skt.accept();
            ServerProtocol protocol(std::move(connection));
            ClientMessage msg = protocol.receive();
            if (i == 0 && msg.type == ClientMessage::Type::Name && msg.username == "reconnect_test") {
                received_first = true;
            }
            if (i == 1 && msg.type == ClientMessage::Type::Name && msg.username == "reconnect_test") {
                received_second = true;
            }
        }
    });

    // Primer cliente: conecta, manda nombre, cierra
    {
        ClientProtocol protocol(Socket("localhost", "3051"));
        ClientMessage msg;
        msg.type = ClientMessage::Type::Name;
        msg.username = "reconnect_test";
        protocol.send_name(msg);
        // Destructor cierra el socket
    }
    // Espera un poco para simular desconexión
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Segundo cliente: reconecta, manda nombre de nuevo
    {
        ClientProtocol protocol(Socket("localhost", "3051"));
        ClientMessage msg;
        msg.type = ClientMessage::Type::Name;
        msg.username = "reconnect_test";
        protocol.send_name(msg);
    }

    server_thread.join();
    ASSERT_TRUE(received_first);
    ASSERT_TRUE(received_second);
}
