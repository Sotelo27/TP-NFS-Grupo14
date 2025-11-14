#ifndef CAR_H
#define CAR_H

#include "../physics/Entidad.h"
#include "../../common/car_model.h"

class Car : public Entidad {
private:
    CarModel spec_;
    float vida_{100.f};

    /*
     * Empuja al auto aplicando una fuerza en su centro
     */
    void apply_force_center(float throttle) noexcept;

    /*
     * Hace girar al auto aplicando un torque
     */
    void apply_steer(float steer) noexcept;

    void apply_lateral_grip() noexcept;
    
    b2Vec2 lateral_velocity() const noexcept;

    /*
     * Pone un tope a la velocidad lineal (m/s).
     */
    void cap_speed(float vmax_mps) noexcept;

public:

    Car(size_t id, const CarModel& spec, b2Body* body);

    /*
     * Devuelve el modelo (especificaciones) del auto
     */
    const CarModel& get_spec() const noexcept;

    /*
     * Devuelve la vida actual del auto
     */
    float get_vida() const noexcept;

    /*
     * Establece la vida actual del auto
     */
    void set_vida(float v) noexcept;

    /*
     * Devuelve la velocidad del auto en metros por segundo
     */
    float speed_mps() const noexcept;

    /*
     * Aplica el movimiento del jugador: acelera, gira segun CarModel
     * y limita la velocidad maxima cuando corresponde
     */
    void apply_input(float throttle, float steer) noexcept;
    
    Type type() const override;

    /*
     * Manejara la colision por entidad
     * -> Si es con otro auto, se podra descontar vida a ambos
     * -> Si es con un borde, no se decuenta la vida
     * -> Si es con un edificio, se podra descontar vida al auto
    */
    void onCollision(Entidad* other) override;
};

#endif
