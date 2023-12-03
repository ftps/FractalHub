#include "buddha.hpp"

void BuddhabrotBase::run()
{
    std::vector<std::thread> t_vector;
    
    if (has_run) return;

    init();

    v_map.clear();
    v_map.reserve(Nthreads);
    total_hits = 0;
    for (size_t i = 0; i < Nthreads; ++i) {
        v_map.push_back(Cmap(size[Y], std::vector<Pcolor>(size[X], BLACK)));
        t_vector.push_back(std::thread([&]{ this->thread(this->v_map.back(), {0,0}); }));
    }

    for (std::thread& t : t_vector) {
        t.join();
    }


    for (size_t i = 0; i <= size1[Y]; ++i) {
        for (size_t j = 0; j <= size1[X]; ++j) {
            for (const Cmap& m : v_map) {
                map[i][j] += m[i][j];
            }
        }
    }
    
    color(map);

    has_run = true;
}

inline void BuddhabrotBase::addToMap(Cmap& map, const std::vector<complex>& orbit, size_t it)
{
    Vpoint loc;
    size_t ch = (it < iter_channel[0]) ? order_channel[0] : ((it < iter_channel[1]) ? order_channel[1] : order_channel[2]);
    int c = 0;

    for (const complex& z : orbit) {
        if (point2index(z, loc)) {
            ++map[loc[X]][loc[Y]][ch];
            ++c;
        }
    }

    total_hits.fetch_add(c, std::memory_order_relaxed);
}

void BuddhabrotCspace::thread(Cmap& map, const Vpoint& ends)
{
    std::random_device rd1, rd2;
    std::mt19937 e1(rd1()), e2(rd2());
    std::uniform_real_distribution<> r_dist(0.0, 4.0), t_dist(0, 2*M_PI);
    std::vector<complex> orbit;
    size_t iter = 0;

    orbit.reserve(iter_channel[2]);
    while (true) {
        ++iter;
        complex z = z_seed;
        complex c;

        double r = std::sqrt(r_dist(e1));
        double t = t_dist(e2);
        c = complex(r*std::cos(t), r*std::sin(t));

        orbit.clear();
        for (size_t i = 0; i < iter_channel[2]; ++i) {
            z = std::pow(z, n) + c;
            orbit.push_back(z);
            if (sqrMod(z) >  4) {
                addToMap(map, orbit, i);
                break;
            }
        }

        if (!(iter % 100000)) {
            printf("Total hits: %d, render hits: %d\n", total_hits.load(), render_hits);
        }

        if (total_hits >= render_hits) {
            break;
        }
    };
}

void BuddhabrotZspace::thread(Cmap& map, const Vpoint& ends)
{
    std::random_device rd1, rd2;
    std::mt19937 e1(rd1()), e2(rd2());
    std::uniform_real_distribution<> r_dist(0.0, 4.0), t_dist(0, 2*M_PI);
    std::vector<complex> orbit;
    size_t iter = 0;

    orbit.reserve(iter_channel[2]);
    while (true) {
        ++iter;
        complex z;
        complex c = z_seed;

        double r = std::sqrt(r_dist(e1));
        double t = t_dist(e2);
        z = complex(r*std::cos(t), r*std::sin(t));

        orbit.clear();
        for (size_t i = 0; i < iter_channel[2]; ++i) {
            z = std::pow(z, n) + c;
            orbit.push_back(z);
            if (sqrMod(z) >  4) {
                addToMap(map, orbit, i);
                break;
            }
        }

        if (!(iter % 100000)) {
            printf("Total hits: %d, render hits: %d\n", total_hits.load(), render_hits);
        }

        if (total_hits >= render_hits) {
            break;
        }
    };
}