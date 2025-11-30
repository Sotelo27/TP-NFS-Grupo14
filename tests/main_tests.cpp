#include "gtest/gtest.h"
// #include "gmock/gmock.h"

#include "protocol_tests.h"

TEST(ProtocolTests, EnviarNuevoNombre) { test_send_name(); }
TEST(ProtocolTests, RecibirMiID) { test_recv_my_id(); }
TEST(ProtocolTests, CrearUnaSala) { test_create_1_room(); }

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}