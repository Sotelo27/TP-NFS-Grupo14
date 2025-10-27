#ifndef CLIENT_ACTION_H
#define CLIENT_ACTION_H

#include <cstddef>
#include <cstdint>

struct ClientAction {
    size_t id;
    Movement action;
};

#endif
