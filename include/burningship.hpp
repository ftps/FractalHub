#ifndef BURNINGSHIP_HPP
#define BURNINGSHIP_HPP

#include "multibrot.hpp"

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

#endif