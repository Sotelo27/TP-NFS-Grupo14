#include "cheat_detector.h"

CheatDetector::CheatDetector(size_t max_size): MAX_SIZE(max_size) {}

void CheatDetector::add_key(const char* key_name) {
    input_buffer += key_name;

    if (input_buffer.size() > MAX_SIZE) {
        input_buffer.erase(0, 1);
    }
}

bool CheatDetector::check_cheat(const std::string& code) {
    return input_buffer.find(code) != std::string::npos;
}
