#ifndef FRACTAL_HPP
#define FRACTAL_HPP

#include <thread>
#include <Magick++.h>

#include "utils.hpp"
#include "color.hpp"

#define Nthreads std::thread::hardware_concurrency()

enum class FractalType {
    MandelCSpace,
    MandelZSpace,
    BurningCSpace,
    BurningZSpace,
    Newton,
    Unknown
};

struct FThreadOpts {
    complex tl_corner;
    long double x_size;
    Vpoint size;
    size_t max_iterations;
    std::string name;
    Pcolor base_color = BLACK;
    Cfunction color;
    std::string op_file;
};

class FractalThread : protected FThreadOpts {
    public:
        void setOpFile(const std::string& op_file);
        void setColorFunction(Cfunction color);
        void setDimensions(complex tl_corner, long double x_size, Vpoint size);
        void run();
        void printMap();
        void drawImage();

    protected:
        FractalThread(const FThreadOpts& fOpts) : FThreadOpts(fOpts),
            map(fOpts.size[Y], std::vector<Pcolor>(fOpts.size[X], fOpts.base_color))
            { setDimensions(tl_corner, x_size, size); }
        virtual void thread(Vmap& map, const Vpoint& ends) = 0;
        complex index2point(const Vpoint& loc);
        
        Vmap map;
        complex br_corner;
        complex c_vector;
        Vpoint size1;
        bool has_run = false;
};




struct MandelOptions : public FThreadOpts {
    complex n = 2;
    complex c = {0,0};
};

class MandelbrotCspace : public FractalThread {
    public:
        MandelbrotCspace(const MandelOptions& fOpts)
            : FractalThread(fOpts), n(fOpts.n), z_seed(fOpts.c) { }
    private:
        void thread(Vmap& map, const Vpoint& ends);

        const complex n;
        const complex z_seed;
};

class MandelbrotZspace : public FractalThread {
    public:
        MandelbrotZspace(const MandelOptions& fOpts)
            : FractalThread(fOpts), n(fOpts.n), c(fOpts.c) { }
    private:
        void thread(Vmap& map, const Vpoint& ends);

        const complex n;
        const complex c;
};

class BurningShipCspace : public FractalThread {
    public:
        BurningShipCspace(const MandelOptions& fOpts) 
            : FractalThread(fOpts), n(fOpts.n), z_seed(fOpts.c) { }

    private:
        void thread(Vmap& map, const Vpoint& ends);

        const complex n;
        const complex z_seed;
};

class BurningShipZspace : public FractalThread {
    public:
        BurningShipZspace(const MandelOptions& fOpts) 
            : FractalThread(fOpts), n(fOpts.n), c(fOpts.c) { }

    private:
        void thread(Vmap& map, const Vpoint& ends);

        const complex n;
        const complex c;
};



struct NewtonOptions : public FThreadOpts{
    Vcomplex roots;
    long double rad_2;
    long double a;
    ColorGen::Vcolor colors;
};

class NewtonFractal : public FractalThread {
    public:
        NewtonFractal(const NewtonOptions& fOpts)
            : FractalThread(fOpts), roots(fOpts.roots), rad_2(fOpts.rad_2),
            c_a(fOpts.a, 0) { }
        void testNewton();
    private:
        void thread(Vmap& map, const Vpoint& ends);
        inline complex rhapson(const complex& z);
        inline bool checkRoot(const complex& z);

        const Vcomplex roots;
        const long double rad_2;
        const complex c_a;
};




#endif