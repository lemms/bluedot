// layer.hpp
// Copyright Laurence Emms 2017

namespace bluedot {
    template <typename Real>
    Layer<Real>::Layer(size_t width, size_t height, size_t channels) : _width(width), _height(height), _channels(channels), _layer(width * height * channels)
    {
    }

    template <typename Real>
    auto Layer<Real>::operator()(size_t x, size_t y, size_t channel) -> Real&
    {
        return _layer[(x + y * _width) * _channels + channel];
    }

    template <typename Real>
    auto Layer<Real>::operator()(size_t x, size_t y, size_t channel) const -> const Real&
    {
        return _layer[(x + y * _width) * _channels + channel];
    }
    
    template <typename Real>
    auto Layer<Real>::width() const -> size_t
    {
        return _width;
    }

    template <typename Real>
    auto Layer<Real>::height() const -> size_t
    {
        return _height;
    }

    template <typename Real>
    auto Layer<Real>::channels() const -> size_t
    {
        return _channels;
    }
}