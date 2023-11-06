#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <array>
#include <functional>
#include <ostream>
#include <string>
#include <memory>
#include <algorithm>
#include <iostream>
#include <filesystem>
#include <limits>
#include <stdexcept>

#define _USE_MATH_DEFINES
#include <cmath>
#include <complex>

#define LOG {std::cout << "IN LINE " << __LINE__ << " OF FILE " << __FILE__ << std::endl;}

#define X 0
#define Y 1

namespace fs = std::filesystem;

using complex = std::complex<long double>;
using Vpoint = std::array<size_t, 2>;
using ComplexF = std::function<complex(const complex&)>;
using Vcomplex = std::vector<complex>;
using Imap = std::vector<std::vector<size_t>>;

constexpr complex c_one = {1.0, 0.0};
constexpr complex c_card = {0.25, 0.0};

inline long double sqrMod(const complex& val)
{
    return std::real(val)*std::real(val) + std::imag(val)*std::imag(val);
}

inline void sortChannel(std::array<size_t, 3>& iter_channel, std::array<size_t, 3>& order_channel)
{
    size_t aux;

    if (iter_channel[0] > iter_channel[1]) {
        aux = iter_channel[0];
        iter_channel[0] = iter_channel[1];
        iter_channel[1] = aux;

        order_channel[0] = 1;
        order_channel[1] = 0;
    }

    if (iter_channel[1] > iter_channel[2]) {
        aux = iter_channel[1];
        iter_channel[1] = iter_channel[2];
        iter_channel[2] = aux;

        order_channel[2] = order_channel[1];
        order_channel[1] = 2;

        if (iter_channel[0] > iter_channel[1]) {
            aux = iter_channel[0];
            iter_channel[0] = iter_channel[1];
            iter_channel[1] = aux;

            aux = order_channel[0];
            order_channel[0] = order_channel[1];
            order_channel[1] = aux;
        }
    }
}

inline bool circleCardiod(const complex& val)
{
    if (sqrMod(val + c_card) <= 0.25) return true; // check if inside a large portion of the cardioid
    else if (sqrMod(val + c_one) <= 0.0625) return true; // check if inside double fixed point
    
    // implement cardioid check

    return false;
}

template <typename T, size_t N>
std::ostream& operator<<(std::ostream& os, const std::array<T,N>& val)
{
    os << "{";
    for (const T& v : val) {
        os << " " << v;
    }
    os << " }";

    return os;
}

template <typename T, size_t N>
inline std::array<T, N> operator+(const std::array<T, N> l, const std::array<T, N> r)
{
    std::array<T, N> res;

    for (size_t i = 0; i < N; ++i) {
        res[i] = l[i] + r[i];
    }

    return res;
}

template <typename T, size_t N>
inline std::array<T, N> operator-(const std::array<T, N> l, const std::array<T, N> r)
{
    std::array<T, N> res;

    for (size_t i = 0; i < N; ++i) {
        res[i] = l[i] - r[i];
    }

    return res;
}

template <typename T, size_t N>
inline std::array<T, N>& operator+=(std::array<T, N>& l, const std::array<T, N> r)
{
    for (size_t i = 0; i < N; ++i) {
        l[i] += r[i];
    }

    return l;
}

template <typename T, size_t N>
inline std::array<T, N>& operator-=(std::array<T, N>& l, const std::array<T, N> r)
{
    for (size_t i = 0; i < N; ++i) {
        l[i] -= r[i];
    }

    return l;
}

template <typename T, typename U, size_t N>
inline std::array<decltype(std::declval<T>()/std::declval<U>()), N> operator/(const std::array<T, N>& l, const std::array<U, N>& r)
{
    std::array<decltype(std::declval<T>()/std::declval<U>()), N> res;

    for (size_t i = 0; i < N; ++i) {
        res[i] = l[i]/r[i];
    }

    return res;
}

template <typename T, typename U, size_t N>
inline std::array<decltype(std::declval<T>()*std::declval<U>()), N> operator*(const std::array<T, N>& l, const std::array<U, N>& r)
{
    std::array<decltype(std::declval<T>()*std::declval<T>()), N> res;

    for (size_t i = 0; i < N; ++i) {
        res[i] = l[i]*r[i];
    }

    return res;
}

template <typename T, size_t N>
inline std::array<T, N> operator/(const std::array<T, N>& l, const T& r)
{
    std::array<T, N> res;

    for (size_t i = 0; i < N; ++i) {
        res[i] = l[i]/r;
    }

    return res;
}

template <typename T, size_t N>
inline std::array<T, N> operator*(const std::array<T, N>& l, const double& r)
{
    std::array<T, N> res;

    for (size_t i = 0; i < N; ++i) {
        res[i] = l[i]*r;
    }

    return res;
}

#endif