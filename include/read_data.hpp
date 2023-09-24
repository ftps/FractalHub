#ifndef READ_DATA_HPP
#define READ_DATA_HPP

#include "fractal.hpp"

#include <fstream>

std::shared_ptr<FractalThread> read_data(const std::string& filename);

#endif