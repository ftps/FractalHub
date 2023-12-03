#include "color.hpp"

namespace ColorGen {

    Pcolor averageColor(const Vcolor& color)
    {
        Pcolor res = BLACK;

        for (const Pcolor c : color) {
            res += c;
        }

        res = res/static_cast<long>(color.size());

        return res;
    }





    Cfunction generateSmooth(const VCpair& colors_pair, const long double& p)
    {
        Vcolor colors;
        long double lp = std::log(p);

        for (size_t i = 0; i < colors_pair.size()-1; ++i) {
            const auto [c1, k1] = colors_pair[i];
            const auto [c2, k2] = colors_pair[i+1];
            for (size_t k = 0; k < k1; ++k) {
                colors.push_back(c1 + (c2 - c1)*((static_cast<double>(k))/(static_cast<double>(k1))));
            }
        }

        const auto [c1, k1] = colors_pair.back();
        const auto [c2, k2] = colors_pair.front();
        for (size_t k = 0; k < k1; ++k) {
            colors.push_back(c1 + (c2 - c1)*((static_cast<double>(k))/(static_cast<double>(k1))));
        }

        Cfunction res = [colors, lp](const size_t& n, const complex& z) {
            long double nu = std::log(std::log(sqrMod(z))/(2*lp))/lp;
            size_t it = (static_cast<size_t>(static_cast<long double>(n) + 1 - nu))%colors.size();

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






    Cfunction generateDefault(const size_t& type, const size_t& size)
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








    void threeChannel(Cmap& map)
    {
        Pcolor max_color = {0, 0, 0};
        for (const std::vector<Pcolor>& vc : map) {
            for (const Pcolor& c : vc) {
                for (size_t i = 0; i < 3; ++i) {
                    if (c[i] > max_color[i]) {
                        max_color[i] = c[i];
                    }
                }
            }
        }

        for (std::vector<Pcolor>& vc : map) {
            for (Pcolor& c : vc) {
                for (size_t i = 0; i < 3; ++i) {
                    c[i] = (255*c[i])/max_color[i];
                }
            }
        }
    }


    Cconverter generateSmooth(const VCpair& colors_pair)
    {
        Vcolor colors;

        for (size_t i = 0; i < colors_pair.size()-1; ++i) {
            const auto [c1, k1] = colors_pair[i];
            const auto [c2, k2] = colors_pair[i+1];
            for (size_t k = 0; k < k1; ++k) {
                colors.push_back(c1 + (c2 - c1)*((static_cast<double>(k))/(static_cast<double>(k1))));
            }
        }
        colors.push_back(colors_pair.back().first);

        Cconverter res = [colors](Cmap& map) {
            size_t len = colors.size()-1;
            long big = 0;

            for (const std::vector<Pcolor>& v : map) {
                for (const Pcolor& c : v) {
                    if (c[X] > big) {
                        big = c[X];
                    }
                }
            }

            for (std::vector<Pcolor>& v : map) {
                for (Pcolor& c : v) {
                    c = colors[(c[X]*len)/big];
                }
            }
        };

        return res;
    }



    Cconverter generateDefault(const size_t& type)
    {
        Cconverter res;

        switch (type)
        {        
        case 0:
            res = generateSmooth({{WHITE, 600}, {BLACK, 0}});
            break;

        case 1:
            res = generateSmooth({{BLACK, 600}, {WHITE, 0}});
            break;

        case 2:
            res = generateSmooth({{WHITE, 600}, {RED, 0}});
            break;

        case 3:
            res = generateSmooth({{WHITE, 600}, {GREEN, 0}});
            break;

        case 4:
            res = generateSmooth({{WHITE, 600}, {BLUE, 0}});
            break;


        default:
            throw std::invalid_argument("Value does not exist");
            break;
        }

        return res;
    }

    Cconverter generateThreeChannel(const double& threshold)
    {
        return [threshold](Cmap& map){
            Pcolor max_color = {0, 0, 0};
            for (const std::vector<Pcolor>& vc : map) {
                for (const Pcolor& c : vc) {
                    for (size_t i = 0; i < 3; ++i) {
                        if (c[i] > max_color[i]) {
                            max_color[i] = c[i];
                        }
                    }
                }
            }

            max_color = max_color*(1.0-threshold);

            for (std::vector<Pcolor>& vc : map) {
                for (Pcolor& c : vc) {
                    for (size_t i = 0; i < 3; ++i) {
                        c[i] = (255*c[i])/max_color[i];
                        if (c[i] > 255) c[i] = 255;
                    }
                }
            }
        };
    }
};