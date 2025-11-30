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

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}