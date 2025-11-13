#include "garage.h"
#include <stdexcept>
#include <iostream>

Garage::Garage() {
    initialize_cars();
}

void Garage::initialize_cars() {
    using namespace car_factory;

    cars.emplace(0, CarSlot{0,common_green_car(), false}); // ID 0
    cars.emplace(1, CarSlot{1,red_car(),false}); // ID 1
    cars.emplace(2, CarSlot{2,red_sport_car(),false}); // ID 2
    cars.emplace(3, CarSlot{3,special_car(),false}); // ID 3
    cars.emplace(4, CarSlot{4,four_by_four_convertible(),false}); // ID 4
    cars.emplace(5, CarSlot{5,pickup_truck(),false}); // ID 5
    cars.emplace(6, CarSlot{6,limousine(),false}); // ID 6
}

std::vector<CarSlot> Garage::get_available_cars() {
    std::lock_guard<std::mutex> lock(m);
    std::vector<CarSlot> result;
    result.reserve(cars.size());

    for (const auto& kv : cars) {
        const CarSlot& slot = kv.second;
        if (!slot.reservado) {
            result.push_back(slot);
        }
    }
    return result;
}

bool Garage::reserve_car(uint8_t car_id) {
    std::lock_guard<std::mutex> lock(m);

    auto it = cars.find(car_id);
    if (it == cars.end()) {
        return false; // car_id invalido, no existe por las dudas
    }

    CarSlot& slot = it->second;
    if (slot.reservado) {
        return false;
    }

    slot.reservado = true;
    return true;
}

void Garage::release_car(uint8_t car_id) {
    std::lock_guard<std::mutex> lock(m);
    auto it = cars.find(car_id);
    
    if (it == cars.end()) {
        return;
    }
    
    it->second.reservado = false;
}

CarModel Garage::get_car_model(uint8_t car_id) {
    std::lock_guard<std::mutex> lock(m);
    auto it = cars.find(car_id);
    
    if (it == cars.end()) {
        throw std::invalid_argument("Car ID " + std::to_string(car_id) + " not found.");
    }

    return it->second.model;
}

bool Garage::is_car_available(uint8_t car_id) {
    std::lock_guard<std::mutex> lock(m);
    auto it = cars.find(car_id);
    
    if (it == cars.end()) {
        return false;
    }

    return !it->second.reservado;
}
