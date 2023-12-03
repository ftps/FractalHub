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

complex FractalThread::index2point(const Vpoint& loc) const
{
    return tl_corner + complex((std::real(c_vector)*loc[X])/size1[X], (std::imag(c_vector)*loc[Y])/size1[Y]);
}

bool FractalThread::point2index(const complex& z, Vpoint& loc) const
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































