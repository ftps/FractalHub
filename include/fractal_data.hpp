#ifndef FRACTAL_DATA_HPP
#define FRACTAL_DATA_HPP

#include "burningship.hpp"
#include "multibrot.hpp"
#include "newton.hpp"
#include "buddha.hpp"

#include <fstream>

enum class FractalType {
    MandelCSpace,
    MandelZSpace,
    BurningCSpace,
    BurningZSpace,
    BuddhaCSpace,
    BuddhaZSpace,
    Newton,
    Unknown
};

std::shared_ptr<FractalThread> read_data(const std::string& filename);

#endif