#ifndef NEWTON_HPP
#define NEWTON_HPP

#include "fractal.hpp"

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