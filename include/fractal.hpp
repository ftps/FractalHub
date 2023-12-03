#ifndef FRACTAL_HPP
#define FRACTAL_HPP

#include <thread>
#include <atomic>
#include <random>
#include <Magick++.h>

#include "utils.hpp"
#include "color.hpp"

#define Nthreads std::thread::hardware_concurrency()

struct FThreadOpts {
    complex tl_corner;
    long double x_size;
    Vpoint size;
    size_t max_iterations;
    std::string name;
    std::string op_file;
    int ssaa;
};

class FractalThread : protected FThreadOpts {
    public:
        void setOpFile(const std::string& op_file);
        void setDimensions(complex tl_corner, long double x_size, Vpoint size);
        virtual void run();
        void printMap();
        void drawImage();

    protected:
        FractalThread(const FThreadOpts& fOpts) : FThreadOpts(fOpts)
            { setDimensions(tl_corner, x_size, size); }
        virtual void thread(Cmap& map, const Vpoint& ends) = 0;
        void init();
        complex index2point(const Vpoint& loc) const;
        bool point2index(const complex& z, Vpoint& loc) const;
        
        Cmap map;
        complex br_corner;
        complex c_vector;
        Vpoint size1;
        Pcolor base_color = BLACK;
        bool has_run = false;
        std::vector<complex> ssaa_dz;
};

#endif