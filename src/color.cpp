#include "color.hpp"

namespace ColorGen {
    Cfunction generateSmooth(const VCpair& colors_pair, const long double& p)
    {
        Vcolor colors;
        long double lp = std::log(p);

        for (size_t i = 0; i < colors_pair.size()-1; ++i) {
            auto [c1, k1] = colors_pair[i];
            auto [c2, k2] = colors_pair[i+1];
            for (size_t k = 0; k < k1; ++k) {
                colors.push_back(c1 + (c2 - c1)*(((double)k)/((double)k1)));
            }
        }

        auto [c1, k1] = colors_pair.back();
        auto [c2, k2] = colors_pair.front();
        for (size_t k = 0; k < k1; ++k) {
            colors.push_back(c1 + (c2 - c1)*(((double)k)/((double)k1)));
        }

        Cfunction res = [colors, lp](const size_t& n, const complex& z) {
            long double nu = std::log(std::log(sqrMod(z))/(2*lp))/lp;
            size_t it = ((size_t)(n + 1 - nu))%colors.size();

            return colors[it];
        };

        return res;
    }

    Cfunction generateRootsSimple(const Vcolor& color, const Vcomplex& roots)
    {
        if (color.size() != roots.size()) {
            // throw error
        }

        Cfunction res = [color, roots](const size_t& n, const complex& z) {
            long double r = std::numeric_limits<long double>::infinity();
            size_t k = 0;

            for (size_t i = 0; i < roots.size(); ++i) {
                long double r_n = sqrMod(z - roots[i]);
                if (r_n < r) {
                    r = r_n;
                    k = i;
                }
            }

            return color[k];
        };

        return res;
    }






    Cfunction generateDefault(const size_t type, const size_t size)
    {
        Cfunction res;

        switch (type)
        {
        case 0:
            res = [](const size_t& n, const complex& z) { return WHITE; };
            break;
        
        case 1:
            res = generateSmooth({{WHITE, size}, {BLACK, size}}, 2);
            break;

        case 2:
            res = generateSmooth({{RED, size}, {WHITE, size}}, 2);
            break;

        case 3:
            res = generateSmooth({{GREEN, size}, {WHITE, size}}, 2);
            break;

        case 4:
            res = generateSmooth({{BLUE, size}, {WHITE, size}}, 2);
            break;


        default:
            // throw error
            break;
        }

        return res;
    }
};