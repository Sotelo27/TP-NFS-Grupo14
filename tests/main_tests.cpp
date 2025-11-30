#include "gtest/gtest.h"
// #include "gmock/gmock.h"

#include "protocol_tests.h"

TEST(ProtocolTests, EnviarNuevoNombre) { test_send_name(); }
TEST(ProtocolTests, RecibirMiID) { test_recv_my_id(); }
TEST(ProtocolTests, CrearUnaSala) { test_create_1_room(); }
TEST(ProtocolTests, SendAndReceiveRooms) { test_send_and_receive_rooms(); }
TEST(ProtocolTests, SendAndReceiveCarsList) { test_send_and_receive_cars_list(); }
TEST(ProtocolTests, SendAndReceiveRaceStart) { test_send_and_receive_race_start(); }
TEST(ProtocolTests, SendAndReceiveResults) { test_send_and_receive_results(); }
TEST(ProtocolTests, SendAndReceiveImprovementOk) { test_send_and_receive_improvement_ok(); }
TEST(ProtocolTests, SendAndReceivePlayersList) { test_send_and_receive_players_list(); }
TEST(ProtocolTests, SendMove) { test_send_move(); }
TEST(ProtocolTests, SendChooseCar) { test_send_choose_car(); }
TEST(ProtocolTests, SendImprovement) { test_send_improvement(); }
TEST(ProtocolTests, SendCheat) { test_send_cheat(); }
TEST(ProtocolTests, SendExit) { test_send_exit(); }
TEST(ProtocolTests, SendAndReceiveMapInfo) { test_send_and_receive_map_info(); }
TEST(ProtocolTests, SendAndReceiveGameOver) { test_send_and_receive_game_over(); }
TEST(ProtocolTests, SendAndReceivePlayerName) { test_send_and_receive_player_name(); }
TEST(ProtocolTests, SendAndReceiveMarketTime) { test_send_and_receive_market_time(); }
TEST(ProtocolTests, SendAndReceiveResultRaceCurrent) { test_send_and_receive_result_race_current(); }
TEST(ProtocolTests, ConcurrentClientsSendName) { test_concurrent_clients_send_name(); }
TEST(ProtocolTests, ClientDisconnectAndReconnect) { test_client_disconnect_and_reconnect(); }

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}