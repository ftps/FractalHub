#include "fractal_data.hpp"

FractalType read_type(std::ifstream& fp);

FThreadOpts read_main(std::ifstream& fp);
MandelOptions read_mandel_opts(std::ifstream& fp, const bool& rc = true);
BuddhaOptions read_buddha_opts(std::ifstream& fp);
NewtonOptions read_newton_opts(std::ifstream& fp);

Cfunction read_color_function(std::ifstream& fp);
Cconverter read_color_converter(std::ifstream& fp);

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
    else if (type == FractalType::BuddhaCSpace) {
        fractal = std::shared_ptr<FractalThread>(new BuddhabrotCspace(read_buddha_opts(fp)));
    }
    else if (type == FractalType::BuddhaZSpace) {
        fractal = std::shared_ptr<FractalThread>(new BuddhabrotZspace(read_buddha_opts(fp)));
    }
    else if (type == FractalType::Newton) {
        fractal = std::shared_ptr<FractalThread>(new NewtonFractal(read_newton_opts(fp)));
    }
    else {
        throw std::invalid_argument("Error reading type");
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
    else if (type == "Buddha_CSpace") return FractalType::BuddhaCSpace;
    else if (type == "Buddha_ZSpace") return FractalType::BuddhaZSpace;
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
    fp >> fOpts.ssaa;

    // compute size
    fOpts.size = {st_aux_a, st_aux_b};
    calc_aux = (fOpts.x_size*st_aux_b)/st_aux_a;
    fOpts.tl_corner = {ld_aux_a - fOpts.x_size/2.0, ld_aux_b + calc_aux/2.0};

    return fOpts;
}

MandelOptions read_mandel_opts(std::ifstream& fp, const bool& rc)
{
    long double aux_a, aux_b;
    std::string aux_s;
    MandelOptions fOpts(read_main(fp));

    fp >> aux_a >> aux_b;
    fOpts.n = {aux_a, aux_b};
    fp >> aux_a >> aux_b;
    fOpts.c = {aux_a, aux_b};

    if (rc) {
        fp >> aux_s;
        fOpts.base_color = read_color(aux_s);
        fOpts.color = read_color_function(fp);
    }
    

    return fOpts;
}

BuddhaOptions read_buddha_opts(std::ifstream& fp)
{
    BuddhaOptions fOpts(read_mandel_opts(fp, false));

    fp >> fOpts.three_channel;
    if (fOpts.three_channel) {
        fp >> fOpts.iter_channel[R] >> fOpts.iter_channel[G] >> fOpts.iter_channel[B];
    }
    else {
        fOpts.iter_channel[R] = fOpts.max_iterations;
        fOpts.iter_channel[G] = fOpts.iter_channel[R];
        fOpts.iter_channel[B] = fOpts.iter_channel[R];
    }
    
    fp >> fOpts.render_hits;
    fOpts.render_hits *= fOpts.size[X]*fOpts.size[Y];
    if (fOpts.three_channel) {
        double th;
        fp >> th;
        fOpts.color = ColorGen::generateThreeChannel(th);
    }
    else {
        fOpts.color = read_color_converter(fp);
    }

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
    fp >> c_aux;
    fOpts.base_color = read_color(c_aux);
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
    else {
        throw std::invalid_argument("Color not found");
    }

    return res;
}

Cconverter read_color_converter(std::ifstream& fp)
{
    std::string aux_type;
    Cconverter res;

    fp >> aux_type;

    if (aux_type == "Default") {
        size_t type;
        fp >> type;
        res = ColorGen::generateDefault(type);
    }
    else {
        throw std::invalid_argument("Converter not found");
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