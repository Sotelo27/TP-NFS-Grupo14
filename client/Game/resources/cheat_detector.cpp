#include "cheat_detector.h"

CheatDetector::CheatDetector(size_t max_size): MAX_SIZE(max_size) {}

void CheatDetector::add_key(const char* key_name) {
    input_buffer += key_name;

    if (input_buffer.size() > MAX_SIZE) {
        input_buffer.erase(0, 1);
    }
}

bool CheatDetector::check_cheat(const std::string& code) {
    if (input_buffer.find(code) == std::string::npos) {
        return false;
    }

    input_buffer.clear();
    return true;
}

bool CheatDetector::check_cheat_key(const char* key_name) {
    // Cheat de vida infinita: tecla '7'
    if (std::string(key_name) == "7") {
        return true;
    }
    // Cheat de teletransporte: tecla '8'
    if (std::string(key_name) == "8") {
        return true;
    }
    // Cheat de ganar carrera automáticamente: tecla '9'
    if (std::string(key_name) == "9") {
        return true;
    }
    return false;
}
