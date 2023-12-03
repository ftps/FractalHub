#ifndef MULTIBROT_HPP
#define MULTIBROT_HPP

#include "fractal.hpp"

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

#endif