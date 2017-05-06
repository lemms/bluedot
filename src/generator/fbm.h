// fbm.h
// Implementation of FBM
// c.f. Fractal Brownian Motion by Patricio Gonzalez Vivo & Jen Lowe https://thebookofshaders.com/13/
// Copyright Laurence Emms 2017

#pragma once
#include <vector>

namespace bluedot
{
    template <typename T, typename RNG>
    class FBM {
    public:
        FBM(RNG& rng, size_t width, size_t height, size_t octaves, T exponent = 2.0, bool spherical = true);
        auto operator()(size_t x, size_t y) const -> T;
    private:
        size_t _width;
        size_t _height;
        std::vector<T> _noise;
    };
}

#include "fbm.hpp"