#include "fractal_data.hpp"

int main(int argc, char* argv[])
{
    Magick::InitializeMagick(*argv);

    if (argc != 2) {
        std::cout << "Error: run program as follows:\n\n\n";
        std::cout << "./madelbrot_exe path_to_op_file\n";
        std::exit(-1);
    }

    std::shared_ptr<FractalThread> f = read_data(std::string(argv[1]));
    f->run();
    f->drawImage();

    return 0;
}