#include "newton.hpp"

inline complex NewtonFractal::rhapson(const complex& z)
{
    complex res = {0.0, 0.0};

    for (const complex& c : roots) {
        res += c_a/(z - c);
    }

    return z - c_one/res;
}

inline bool NewtonFractal::checkRoot(const complex& z)
{
    for (const complex& r : roots) {
        if (sqrMod(z - r) < rad_2) return true;
    }

    return false;
}

void NewtonFractal::thread(Cmap& map, const Vpoint& ends)
{
    for (size_t i = ends[X]; i < ends[Y]; ++i) {
        for (size_t j = 0; j < size[X]; ++j) {
            complex z = index2point({j,i});
            for (size_t k = 0; k < max_iterations; ++k) {
                z = rhapson(z);
                if (checkRoot(z)) {
                    map[i][j] = color(k, z);
                    break;
                }
            }
        }
    }
}