#include "multibrot.hpp"

void MandelbrotCspace::thread(Cmap& map, const Vpoint& ends)
{
    ColorGen::Vcolor color_v;

    for (size_t i = ends[X]; i < ends[Y]; ++i) {
        for (size_t j = 0; j < size[X]; ++j) {
            complex c0_c = index2point({j,i});
            color_v.clear();
            for (const complex& dz : ssaa_dz) {
                complex c0 = c0_c + dz;
                complex z = z_seed;
                bool non_escape = true;
                for (size_t k = 0; k < max_iterations; ++k) {
                    z = std::pow(z, n) + c0;
                    if (sqrMod(z) > 4) {
                        non_escape = false;
                        color_v.push_back(color(k, z));
                        break;
                    }
                }
                if (non_escape) {
                    color_v.push_back(base_color);
                }
            }
            map[i][j] = ColorGen::averageColor(color_v);
        }
    }
}






void MandelbrotZspace::thread(Cmap& map, const Vpoint& ends)
{
    ColorGen::Vcolor color_v;

    for (size_t i = ends[X]; i < ends[Y]; ++i) {
        for (size_t j = 0; j < size[X]; ++j) {
            complex z_c = index2point({j,i});
            color_v.clear();
            for (const complex& dz : ssaa_dz) {
                complex z = z_c + dz;
                bool non_escape = true;
                for (size_t k = 0; k < max_iterations; ++k) {
                    z = std::pow(z, n) + c;
                    if (sqrMod(z) > 4) {
                        non_escape = false;
                        color_v.push_back(color(k, z));
                        break;
                    }
                }
                if (non_escape) {
                    color_v.push_back(base_color);
                }
            }
            map[i][j] = ColorGen::averageColor(color_v);
        }
    }
}