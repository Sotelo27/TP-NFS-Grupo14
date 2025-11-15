#include "garage.h"
#include <stdexcept>
#include <iostream>

Garage::Garage() {
    initialize_cars();
}

void Garage::initialize_cars() {
    using namespace car_factory;

    cars.emplace(0, CarSlot{0,common_green_car()}); // ID 0
    cars.emplace(1, CarSlot{1,red_car()}); // ID 1
    cars.emplace(2, CarSlot{2,red_sport_car()}); // ID 2
    cars.emplace(3, CarSlot{3,special_car()}); // ID 3
    cars.emplace(4, CarSlot{4,four_by_four_convertible()}); // ID 4
    cars.emplace(5, CarSlot{5,pickup_truck()}); // ID 5
    cars.emplace(6, CarSlot{6,limousine()}); // ID 6
}

std::vector<CarSlot> Garage::get_available_cars() {
    std::lock_guard<std::mutex> lock(m);
    std::vector<CarSlot> result;
    result.reserve(cars.size());

    for (const auto& kv : cars) {
        const CarSlot& slot = kv.second;
        result.push_back(slot);
    }
    return result;
}

bool Garage::reserve_car(uint8_t car_id) {
    std::lock_guard<std::mutex> lock(m);

    auto it = cars.find(car_id);
    if (it == cars.end()) {
        std::cout << "[Garage] User tried to select an invalid car (ID: " << (int)car_id << ") in the garage." << std::endl;
        return false;
    }

    std::cout << "[Garage] User selected the car (ID: " << (int)car_id << ") in the garage." << std::endl;
    return true;
}

CarModel Garage::get_car_model(uint8_t car_id) {
    std::lock_guard<std::mutex> lock(m);
    auto it = cars.find(car_id);
    if (it == cars.end()) {
        throw std::invalid_argument("Car ID " + std::to_string(car_id) + " not found.");
    }
    return it->second.model;
}
