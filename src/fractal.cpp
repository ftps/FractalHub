#include "fractal.hpp"

//#undef Nthreads
//#define Nthreads 1

void FractalThread::setOpFile(const std::string& op_file)
{
    this->op_file = op_file;
}

void FractalThread::setColorFunction(Cfunction color)
{
    this->color = color;
}

void FractalThread::setDimensions(complex tl_corner, long double x_size, Vpoint size)
{
    this->tl_corner = tl_corner;
    this->size = size;

    size1 = size - Vpoint{1,1};
    c_vector = {x_size, -x_size*size[Y]/size[X]};
    br_corner = tl_corner + c_vector;
}

complex FractalThread::index2point(const Vpoint& loc)
{
    //return tl_corner + ((c_vector*loc)/size1);
    return tl_corner + complex((std::real(c_vector)*loc[X])/size1[X], (std::imag(c_vector)*loc[Y])/size1[Y]);
}

void FractalThread::run()
{
    std::vector<std::thread> t_vector;
    
    if (has_run) return;

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






void MandelbrotCspace::thread(Vmap& map, const Vpoint& ends)
{
    for (size_t i = ends[X]; i < ends[Y]; ++i) {
        for (size_t j = 0; j < size[X]; ++j) {
            complex c0 = index2point({j,i});
            complex z = z_seed;
            for (size_t k = 0; k < max_iterations; ++k) {
                z = std::pow(z, n) + c0;
                if (sqrMod(z) > 4) {
                    map[i][j] = color(k, z);
                    break;
                }
            }
        }
    }
}






void MandelbrotZspace::thread(Vmap& map, const Vpoint& ends)
{
    for (size_t i = ends[X]; i < ends[Y]; ++i) {
        for (size_t j = 0; j < size[X]; ++j) {
            complex z = index2point({j,i});
            for (size_t k = 0; k < max_iterations; ++k) {
                z = std::pow(z, n) + c;
                if (sqrMod(z) > 4) {
                    map[i][j] = color(k, z);
                    break;
                }
            }
        }
    }
}






void BurningShipCspace::thread(Vmap& map, const Vpoint& ends)
{
    for (size_t i = ends[X]; i < ends[Y]; ++i) {
        for (size_t j = 0; j < size[X]; ++j) {
            complex c0 = index2point({j,i});
            complex z = z_seed;
            for (size_t k = 0; k < max_iterations; ++k) {
                z = std::pow(complex(std::abs(z.real()), std::abs(z.imag())), n) + c0;
                if (sqrMod(z) > 4) {
                    map[i][j] = color(k, z);
                    break;
                }
            }
        }
    }
}






void BurningShipZspace::thread(Vmap& map, const Vpoint& ends)
{
    for (size_t i = ends[X]; i < ends[Y]; ++i) {
        for (size_t j = 0; j < size[X]; ++j) {
            complex z = index2point({j,i});
            for (size_t k = 0; k < max_iterations; ++k) {
                z = std::pow(complex(std::abs(z.real()), std::abs(z.imag())), n) + c;
                if (sqrMod(z) > 4) {
                    map[i][j] = color(k, z);
                    break;
                }
            }
        }
    }
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

void NewtonFractal::thread(Vmap& map, const Vpoint& ends)
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

void NewtonFractal::testNewton()
{
    for (complex r : roots) {
        std::cout << "For root r = " << r << std::endl;
        r += complex{0.2, 0.0};
        std::cout << "Starting at z = " << r << std::endl;
        for (int i = 0; i < 5; ++i) {
            r = rhapson(r);
        }
        std::cout << "Ended at z = " << r << std::endl << std::endl << std::endl;
        
    }
}
