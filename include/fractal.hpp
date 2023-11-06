#ifndef FRACTAL_HPP
#define FRACTAL_HPP

#include <thread>
#include <atomic>
#include <random>
#include <Magick++.h>

#include "utils.hpp"
#include "color.hpp"

#define Nthreads std::thread::hardware_concurrency()

enum class FractalType {
    MandelCSpace,
    MandelZSpace,
    BurningCSpace,
    BurningZSpace,
    BuddhaCSpace,
    BuddhaZSpace,
    Newton,
    Unknown
};

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
        inline complex index2point(const Vpoint& loc) const;
        inline bool point2index(const complex& z, Vpoint& loc) const;
        
        Cmap map;
        complex br_corner;
        complex c_vector;
        Vpoint size1;
        Pcolor base_color = BLACK;
        bool has_run = false;
        std::vector<complex> ssaa_dz;
};






/*
 *
 * Mandelbrot Fractal
 *
 */


struct MandelOptions : public FThreadOpts {
    complex n = 2;
    complex c = {0,0};
    Pcolor base_color = BLACK;
    Cfunction color;
};

class MandelbrotCspace : public FractalThread {
    public:
        MandelbrotCspace(const MandelOptions& fOpts)
            : FractalThread(fOpts), n(fOpts.n), z_seed(fOpts.c),
            color(fOpts.color) { base_color = fOpts.base_color; }
    private:
        void thread(Cmap& map, const Vpoint& ends);

        const complex n;
        const complex z_seed;
        const Cfunction color;
};

class MandelbrotZspace : public FractalThread {
    public:
        MandelbrotZspace(const MandelOptions& fOpts)
            : FractalThread(fOpts), n(fOpts.n), c(fOpts.c),
            color(fOpts.color) { base_color = fOpts.base_color; }
    private:
        void thread(Cmap& map, const Vpoint& ends);

        const complex n;
        const complex c;
        const Cfunction color;
};






/*
 *
 * Burning Ship Fractal
 *
 */

class BurningShipCspace : public FractalThread {
    public:
        BurningShipCspace(const MandelOptions& fOpts) 
            : FractalThread(fOpts), n(fOpts.n), z_seed(fOpts.c),
            color(fOpts.color) { base_color = fOpts.base_color; }

    private:
        void thread(Cmap& map, const Vpoint& ends);

        const complex n;
        const complex z_seed;
        const Cfunction color;
};

class BurningShipZspace : public FractalThread {
    public:
        BurningShipZspace(const MandelOptions& fOpts) 
            : FractalThread(fOpts), n(fOpts.n), c(fOpts.c),
            color(fOpts.color) { base_color = fOpts.base_color; }

    private:
        void thread(Cmap& map, const Vpoint& ends);

        const complex n;
        const complex c;
        const Cfunction color;
};






/*
 *
 * Buddhabrot Fractal
 *
 */

struct BuddhaOptions : public MandelOptions {
    bool three_channel;
    std::array<size_t, 3> iter_channel;
    int render_hits;
    Cconverter color;
};

class BuddhabrotBase : public FractalThread {
    public:
        BuddhabrotBase(const BuddhaOptions& fOpts) 
            : FractalThread(fOpts), n(fOpts.n), z_seed(fOpts.c),
            three_channel(fOpts.three_channel), render_hits(fOpts.render_hits),
            color(fOpts.color), iter_channel(fOpts.iter_channel)
            { sortChannel(iter_channel, order_channel); }
        void run();

    protected:
        inline void addToMap(Cmap& map, const std::vector<complex>& orbit, size_t it);

        const complex n;
        const complex z_seed;
        const bool three_channel;
        const int render_hits;
        Cconverter color;
        std::array<size_t, 3>  iter_channel;
        std::array<size_t, 3>  order_channel = {0,1,2};
        std::vector<Cmap> v_map;
        std::atomic_int total_hits;
};

class BuddhabrotCspace : public BuddhabrotBase {
    public:
        BuddhabrotCspace(const BuddhaOptions& fOpts) 
            : BuddhabrotBase(fOpts) { }

    private:
        void thread(Cmap& map, const Vpoint& ends);
};

class BuddhabrotZspace : public BuddhabrotBase {
    public:
        BuddhabrotZspace(const BuddhaOptions& fOpts) 
            : BuddhabrotBase(fOpts) { }

    private:
        void thread(Cmap& map, const Vpoint& ends);
};






/*
 *
 * Newton's Fractal
 *
 */

struct NewtonOptions : public FThreadOpts{
    Vcomplex roots;
    long double rad_2;
    long double a;
    ColorGen::Vcolor colors;
    Pcolor base_color = BLACK;
    Cfunction color;
};

class NewtonFractal : public FractalThread {
    public:
        NewtonFractal(const NewtonOptions& fOpts)
            : FractalThread(fOpts), roots(fOpts.roots), rad_2(fOpts.rad_2),
            c_a(fOpts.a, 0), color(fOpts.color) { base_color = fOpts.base_color; }
    private:
        void thread(Cmap& map, const Vpoint& ends);
        inline complex rhapson(const complex& z);
        inline bool checkRoot(const complex& z);

        const Vcomplex roots;
        const long double rad_2;
        const complex c_a;
        const Cfunction color;
};




#endif