#ifndef CHEAT_DETECTOR_H
#define CHEAT_DETECTOR_H

#include <string>

class CheatDetector {
private:
    std::string input_buffer;
    const size_t MAX_SIZE;

public:
    explicit CheatDetector(size_t max_size);

    void add_key(const char* key_name);

    bool check_cheat(const std::string& code);

    // Nuevo: para detectar cheats de teclas individuales
    bool check_cheat_key(const char* key_name);
};

#endif
