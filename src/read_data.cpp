#include "read_data.hpp"

FractalType read_type(std::ifstream& fp);
Cfunction read_color_function(std::ifstream& fp);
FThreadOpts read_main(std::ifstream& fp);
MandelOptions read_mandel_opts(std::ifstream& fp);
NewtonOptions read_newton_opts(std::ifstream& fp);

Pcolor read_color(std::string color);
u_short convert_hex(const char& c);

std::shared_ptr<FractalThread> read_data(const std::string& filename)
{
    std::ifstream fp(filename, std::ios::in);
    std::shared_ptr<FractalThread> fractal;
    FractalType type = read_type(fp);

    if (fp.bad()) {
        std::cout << "Error opening file, exiting . . ." << std::endl;
        std::exit(-2);
    }


    if (type == FractalType::MandelCSpace) {
        fractal = std::shared_ptr<FractalThread>(new MandelbrotCspace(read_mandel_opts(fp)));
    }
    else if (type == FractalType::MandelZSpace) {
        fractal = std::shared_ptr<FractalThread>(new MandelbrotZspace(read_mandel_opts(fp)));
    }
    else if (type == FractalType::BurningCSpace) {
        fractal = std::shared_ptr<FractalThread>(new BurningShipCspace(read_mandel_opts(fp)));
    }
    else if (type == FractalType::BurningZSpace) {
        fractal = std::shared_ptr<FractalThread>(new BurningShipZspace(read_mandel_opts(fp)));
    }
    else if (type == FractalType::Newton) {
        fractal = std::shared_ptr<FractalThread>(new NewtonFractal(read_newton_opts(fp)));
    }
    else {
        // throw error
    }

    fractal->setOpFile(filename);
    return fractal;
}




FractalType read_type(std::ifstream& fp)
{
    std::string type;

    fp >> type;

    if (type == "Mandel_CSpace") return FractalType::MandelCSpace;
    else if (type == "Mandel_ZSpace") return FractalType::MandelZSpace;
    else if (type == "Burning_CSpace") return FractalType::BurningCSpace;
    else if (type == "Burning_ZSpace") return FractalType::BurningZSpace;
    else if (type == "Newton_Fractal") return FractalType::Newton;

    return FractalType::Unknown;
}

FThreadOpts read_main(std::ifstream& fp)
{
    FThreadOpts fOpts;
    long double ld_aux_a, ld_aux_b, calc_aux;
    size_t st_aux_a, st_aux_b;
    std::string aux_c;

    fp >> ld_aux_a >> ld_aux_b;
    fp >> fOpts.x_size;
    fp >> st_aux_a >> st_aux_b;
    fp >> fOpts.max_iterations;
    fp >> fOpts.name;
    fp >> aux_c;

    // read color
    fOpts.base_color = read_color(aux_c);
    fOpts.color = read_color_function(fp);

    // compute size
    fOpts.size = {st_aux_a, st_aux_b};
    calc_aux = (fOpts.x_size*st_aux_b)/st_aux_a;
    fOpts.tl_corner = {ld_aux_a - fOpts.x_size/2.0, ld_aux_b + calc_aux/2.0};

    return fOpts;
}

MandelOptions read_mandel_opts(std::ifstream& fp)
{
    long double aux_a, aux_b;
    MandelOptions fOpts(read_main(fp));

    fp >> aux_a >> aux_b;
    fOpts.n = {aux_a, aux_b};
    fp >> aux_a >> aux_b;
    fOpts.c = {aux_a, aux_b};

    return fOpts;
}

NewtonOptions read_newton_opts(std::ifstream& fp)
{
    size_t n;
    long double ld_aux_a, ld_aux_b;
    std::string c_aux;
    NewtonOptions fOpts(read_main(fp));

    fOpts.roots.clear();
    fOpts.colors.clear();

    fp >> n;
    for (size_t i = 0; i < n; ++i) {
        fp >> ld_aux_a >> ld_aux_b >> c_aux;
        fOpts.roots.emplace_back(ld_aux_a, ld_aux_b);
        fOpts.colors.push_back(read_color(c_aux));
    }
    fp >> fOpts.rad_2 >> fOpts.a;

    fOpts.color = ColorGen::generateRootsSimple(fOpts.colors, fOpts.roots);

    return fOpts;
}








Cfunction read_color_function(std::ifstream& fp)
{
    std::string aux_type;
    Cfunction res = [](const size_t& size, const complex& z) { return WHITE; };

    fp >> aux_type;
    
    if (aux_type == "Default") {
        size_t number, size;
        fp >> number >> size;
        res = ColorGen::generateDefault(number, size);
    }
    else if (aux_type == "Smooth") {
        size_t c_number, c_num;
        std::string c_aux;
        ColorGen::VCpair palette;
        long double p;

        fp >> c_number >> p;
        for (size_t i = 0; i < c_number; ++i) {
            fp >> c_aux >> c_num;
            palette.emplace_back(read_color(c_aux), c_num);
        }

        res = ColorGen::generateSmooth(palette, p);
    }
    else if (aux_type == "Roots") {
        // do nothing
    }
    else {
        // throw error
    }

    return res;
}

Pcolor read_color(std::string color)
{
    if (color.size() != 7 && color[0] != '#') {
        // throw error
    }

    Pcolor color_v;

    std::transform(color.begin(), color.end(), color.begin(),
        [](unsigned char c){ return std::tolower(c); });
    for (size_t i = 0; i < 3; ++i) {
        color_v[i] = (convert_hex(color[2*i + 1]) << 4) + convert_hex(color[2*i + 2]);
    }

    return color_v;
}

u_short convert_hex(const char& c)
{
    if (c >= 'a' && c <= 'f') {
        return (c - 'a') + 0xa;
    }
    else if (c >= '0' && c <= '9') {
        return (c - '0');
    }

    //throw error
    return 0;
}