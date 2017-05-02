// fbm.hpp
// Copyright Laurence Emms 2017

namespace bluedot {
    template <typename T, typename RNG>
    FBM<T, RNG>::FBM(RNG& rng, size_t width, size_t height, size_t octaves, T exponent, bool spherical) : _width(width), _height(height)
    {
        T weight = static_cast<T>(1.0) / static_cast<T>(octaves);
        _noise.resize(_width * _height, static_cast<T>(0.0));
        for (size_t y{0}; y < _height; ++y)
        {
            for (size_t x{0}; x < _width; ++x)
            {
                _noise[x + y * _width] = static_cast<T>(rng()) * weight;
            }
        }
        if (spherical)
        {
            for (size_t x{0}; x < _width; ++x)
            {
                _noise[x] = _noise[0];
            }
            for (size_t x{0}; x < _width; ++x)
            {
                _noise[x + (_height - 1) * _width] = _noise[(_height - 1) * _width];
            }
            for (size_t y{0}; y < _height; ++y)
            {
                _noise[_width - 1 + y * _width] = _noise[y * _width];
            }
        }
        for (size_t o{0}; o < octaves; ++o)
        {
            weight *= exponent;
            width /= 2;
            height /= 2;
            size_t w = width + 1;
            size_t h = height + 1;
            std::vector<T> layer(w * h, static_cast<T>(0.0));
            for (size_t y{0}; y < h; ++y)
            {
                for (size_t x{0}; x < w; ++x)
                {
                    layer[x + y * w] = static_cast<T>(rng());
                }
            }
            if (spherical)
            {
                for (size_t x{0}; x < w; ++x)
                {
                    layer[x] = layer[0];
                }
                for (size_t x{0}; x < w; ++x)
                {
                    layer[x + (h - 1) * w] = layer[(h - 1) * w];
                }
                for (size_t y{0}; y < h; ++y)
                {
                    layer[w - 1 + y * w] = layer[y * w];
                }
            }

            for (size_t y{0}; y < _height; ++y)
            {
                for (size_t x{0}; x < _width; ++x)
                {
                    T flx{static_cast<T>(x) / static_cast<T>(_width) * static_cast<T>(width)};
                    T fly{static_cast<T>(y) / static_cast<T>(_height) * static_cast<T>(height)};
                    size_t lx{static_cast<size_t>(std::floor(flx))};
                    size_t ly{static_cast<size_t>(std::floor(fly))};
                    T dlx{flx - static_cast<float>(lx)};
                    T dly{fly - static_cast<float>(ly)};
                    size_t nlx{(lx + 1 < w) ? lx + 1 : lx};
                    size_t nly{(ly + 1 < h) ? ly + 1 : ly};
                    T x00 = layer[lx + ly * w];
                    T x10 = layer[nlx + ly * w];
                    T x01 = layer[lx + nly * w];
                    T x11 = layer[nlx + nly * w];
                    T x0 = x00 + dlx * (x10 - x00);
                    T x1 = x01 + dlx * (x11 - x01);
                    _noise[x + y * _width] += (x0 + dly * (x1 - x0)) * weight;
                }
            }
        }
    }

    template <typename T, typename RNG>
    auto FBM<T, RNG>::operator()(size_t x, size_t y) const -> T
    {
        assert(x < _width);
        assert(y < _height);
        return _noise[x + y * _width];
    }
}