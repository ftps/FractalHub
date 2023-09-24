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

constexpr complex c_one = {1.0, 0.0};

inline long double sqrMod(const complex& val)
{
    return std::real(val)*std::real(val) + std::imag(val)*std::imag(val);
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
inline std::array<decltype(std::declval<T>()*std::declval<U>()), N> operator/(const std::array<T, N>& l, const std::array<U, N>& r)
{
    std::array<decltype(std::declval<T>()*std::declval<U>()), N> res;

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
inline std::array<long double, N> operator/(const std::array<T, N>& l, const long double& r)
{
    std::array<long double, N> res;

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