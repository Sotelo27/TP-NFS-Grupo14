#ifndef INPUT_STATE_H
#define INPUT_STATE_H

// Estado de entrada del jugador para un tick.
// Todas las entradas son digitales (teclas del teclado).
struct InputState {
    bool up{false};       // acelerar
    bool down{false};     // retroceder
    bool left{false};     // girar a la izquierda
    bool right{false};    // girar a la derecha
};

#endif
