#ifndef COLOR_HPP
#define COLOR_HPP

#include "utils.hpp"

#define R 0
#define G 1
#define B 2

#define BLACK ((Pcolor) {0x00,0x00,0x00})
#define WHITE ((Pcolor) {0xFF,0xFF,0xFF})
#define RED   ((Pcolor) {0xFF,0x00,0x00})
#define GREEN ((Pcolor) {0x00,0xFF,0x00})
#define BLUE  ((Pcolor) {0x00,0x00,0xFF})

using Pcolor = std::array<short, 3>;
using Vmap = std::vector<std::vector<Pcolor>>;
using Cfunction = std::function<Pcolor(const size_t&, const complex&)>;

namespace ColorGen {
    using Vcolor = std::vector<Pcolor>;
    using Cpair = std::pair<Pcolor, size_t>;
    using VCpair = std::vector<Cpair>;

    Cfunction generateSmooth(const VCpair& colors_pair, const long double& p);
    Cfunction generateDefault(const size_t type, const size_t size);
    Cfunction generateRootsSimple(const Vcolor& color, const Vcomplex& roots);
};


#endif