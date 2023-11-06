#include "fractal.hpp"

//#undef Nthreads
//#define Nthreads 1

void FractalThread::setOpFile(const std::string& op_file)
{
    this->op_file = op_file;
}

void FractalThread::setDimensions(complex tl_corner, long double x_size, Vpoint size)
{
    this->tl_corner = tl_corner;
    this->size = size;

    size1 = size - Vpoint{1,1};
    c_vector = {x_size, -x_size*size[Y]/size[X]};
    br_corner = tl_corner + c_vector;

    ssaa_dz.clear();
    if (ssaa == 0) {
        ssaa_dz.push_back({0,0});
    }
    else {
        long double dx = std::abs(std::real(c_vector))/size1[X];
        long double dy = std::abs(std::imag(c_vector))/size1[Y];

        int nx = (ssaa + 1)/2 + 1;
        int ny = ssaa/2 + 1;

        long double dxn = dx/(nx + 1);
        long double dyn = dy/(ny + 1);

        for (int i = 1; i <= nx; ++i) {
            for (int j = 1; j <= ny; ++j) {
                ssaa_dz.push_back({dxn*j - dx/2, dyn*i - dy/2});
            }
        }
    }
}

inline complex FractalThread::index2point(const Vpoint& loc) const
{
    return tl_corner + complex((std::real(c_vector)*loc[X])/size1[X], (std::imag(c_vector)*loc[Y])/size1[Y]);
}

inline bool FractalThread::point2index(const complex& z, Vpoint& loc) const
{
    if (z.real() < tl_corner.real() || z.imag() > tl_corner.imag()) {
        return false;
    }
    else if (z.real() > br_corner.real() || z.imag() < br_corner.imag()) {
        return false;
    }

    complex aux = z - tl_corner;
    loc = {static_cast<size_t>(size1[Y]*(aux.imag()/c_vector.imag())), static_cast<size_t>(size1[X]*(std::real(aux)/std::real(c_vector)))};

    return true;
}

void FractalThread::init()
{
    map = Cmap(size[Y], std::vector<Pcolor>(size[X], base_color));
}

void FractalThread::run()
{
    std::vector<std::thread> t_vector;
    
    if (has_run) return;

    init();

    for (size_t i = 0; i < Nthreads; ++i) {
        Vpoint ends = {i*size[Y]/Nthreads, (i+1)*size[Y]/Nthreads};
        t_vector.push_back(std::thread([&, ends]{ this->thread(this->map, ends); }));
    }

    for (std::thread& t : t_vector) {
        t.join();
    }

    has_run = true;
}

void FractalThread::printMap()
{
    if (!has_run) return;

    for (size_t i = 0; i < size[Y]; ++i) {
        for (size_t j = 0; j < size[X]; ++j) {
            if (map[i][j] == BLACK) putchar('*');
            else putchar(' ');
        }
        std::cout << "|\n";
    }
}

void FractalThread::drawImage()
{
    fs::path p{ name + ".png" };

    if (fs::exists(p)) {
        size_t i = 0;

        do {
            p = fs::path{name + "_" + std::to_string(++i) + ".png"};
        } while(fs::exists(p));

        name = name + "_" + std::to_string(i);
    }

    unsigned char* pix = new unsigned char[size[X]*size[Y]*3];
    for (size_t i = 0; i < size[Y]; ++i) {
        for(size_t j = 0; j < size[X]; ++j) {
            pix[3*(size[X]*i + j)] = map[i][j][R];
            pix[3*(size[X]*i + j)+1] = map[i][j][G];
            pix[3*(size[X]*i + j)+2] = map[i][j][B];
        }
    }

    Magick::Image image;
    image.read(size[X], size[Y], "RGB", Magick::CharPixel, pix);
    image.write(name + ".png");

    if (fs::exists(fs::path{name + "_op.dat"})) {
        fs::remove(fs::path{name + "_op.dat"});
    }

    fs::copy_file(op_file, name + "_op.dat");

    delete pix;
}






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






void BurningShipCspace::thread(Cmap& map, const Vpoint& ends)
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
                    z = std::pow(complex(std::abs(z.real()), std::abs(z.imag())), n) + c0;
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
            map[size1[Y] - i][j] = ColorGen::averageColor(color_v);   
        }
    }
}






void BurningShipZspace::thread(Cmap& map, const Vpoint& ends)
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
                    z = std::pow(complex(std::abs(z.real()), std::abs(z.imag())), n) + c;
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
            map[size1[Y] - i][j] = ColorGen::averageColor(color_v);
        }
    }
}





void BuddhabrotBase::run()
{
    std::vector<std::thread> t_vector;
    
    if (has_run) return;

    init();

    v_map.clear();
    v_map.reserve(Nthreads);
    total_hits = 0;
    for (size_t i = 0; i < Nthreads; ++i) {
        v_map.push_back(Cmap(size[Y], std::vector<Pcolor>(size[X], BLACK)));
        t_vector.push_back(std::thread([&]{ this->thread(this->v_map.back(), {0,0}); }));
    }

    for (std::thread& t : t_vector) {
        t.join();
    }


    for (size_t i = 0; i <= size1[Y]; ++i) {
        for (size_t j = 0; j <= size1[X]; ++j) {
            for (const Cmap& m : v_map) {
                map[i][j] += m[i][j];
            }
        }
    }
    
    color(map);

    has_run = true;
}

inline void BuddhabrotBase::addToMap(Cmap& map, const std::vector<complex>& orbit, size_t it)
{
    Vpoint loc;
    size_t ch = (it < iter_channel[0]) ? order_channel[0] : ((it < iter_channel[1]) ? order_channel[1] : order_channel[2]);
    int c = 0;

    for (const complex& z : orbit) {
        if (point2index(z, loc)) {
            ++map[loc[X]][loc[Y]][ch];
            ++c;
        }
    }

    total_hits.fetch_add(c, std::memory_order_relaxed);
}

void BuddhabrotCspace::thread(Cmap& map, const Vpoint& ends)
{
    std::random_device rd1, rd2;
    std::mt19937 e1(rd1()), e2(rd2());
    std::uniform_real_distribution<> r_dist(0.0, 4.0), t_dist(0, 2*M_PI);
    std::vector<complex> orbit;
    size_t iter = 0;

    orbit.reserve(iter_channel[2]);
    while (true) {
        ++iter;
        complex z = z_seed;
        complex c;

        double r = std::sqrt(r_dist(e1));
        double t = t_dist(e2);
        c = complex(r*std::cos(t), r*std::sin(t));

        orbit.clear();
        for (size_t i = 0; i < iter_channel[2]; ++i) {
            z = std::pow(z, n) + c;
            orbit.push_back(z);
            if (sqrMod(z) >  4) {
                addToMap(map, orbit, i);
                break;
            }
        }

        if (!(iter % 100000)) {
            printf("Total hits: %d, render hits: %d\n", total_hits.load(), render_hits);
        }

        if (total_hits >= render_hits) {
            break;
        }
    };
}

void BuddhabrotZspace::thread(Cmap& map, const Vpoint& ends)
{
    std::random_device rd1, rd2;
    std::mt19937 e1(rd1()), e2(rd2());
    std::uniform_real_distribution<> r_dist(0.0, 4.0), t_dist(0, 2*M_PI);
    std::vector<complex> orbit;
    size_t iter = 0;

    orbit.reserve(iter_channel[2]);
    while (true) {
        ++iter;
        complex z;
        complex c = z_seed;

        double r = std::sqrt(r_dist(e1));
        double t = t_dist(e2);
        z = complex(r*std::cos(t), r*std::sin(t));

        orbit.clear();
        for (size_t i = 0; i < iter_channel[2]; ++i) {
            z = std::pow(z, n) + c;
            orbit.push_back(z);
            if (sqrMod(z) >  4) {
                addToMap(map, orbit, i);
                break;
            }
        }

        if (!(iter % 100000)) {
            printf("Total hits: %d, render hits: %d\n", total_hits.load(), render_hits);
        }

        if (total_hits >= render_hits) {
            break;
        }
    };
}











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