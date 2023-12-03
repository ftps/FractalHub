#ifndef BUDDHA_HPP
#define BUDDHA_HPP

#include "multibrot.hpp"

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

#endif