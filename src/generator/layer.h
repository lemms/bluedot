// layer.h
// Layers of the map
// Copyright Laurence Emms 2017

#pragma once
#include <vector>

namespace bluedot {
    template <typename Real>
    class Layer {
    public:
        Layer(size_t width, size_t height, size_t channels);
        inline auto operator()(size_t x, size_t y, size_t channel) -> Real&;
        inline auto operator()(size_t x, size_t y, size_t channel) const -> const Real&;
        inline auto width() const -> size_t;
        inline auto height() const -> size_t;
        inline auto channels() const -> size_t;
    private:
        size_t _width;
        size_t _height;
        size_t _channels;
        std::vector<Real> _layer;
    };
}

#include "layer.hpp"