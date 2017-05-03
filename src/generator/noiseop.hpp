// noiseop.hpp
// Copyright Laurence Emms 2017

#include <boost/random.hpp>
#include <omp.h>

namespace bluedot {
    template <typename Real, typename RNG>
    NoiseOperator<Real, RNG>::NoiseOperator(RNG& rng, const std::vector<Real>& multiplier, Real scale, Real offset) :
        _rng(rng), _multiplier(multiplier), _scale(scale), _offset(offset)
    {
    }

    template <typename Real, typename RNG>
    auto NoiseOperator<Real, RNG>::operator()(Layer<Real>& layer) -> bool
    {
        int num_samples = static_cast<int>(layer.width() * layer.height());
        for (int s{0}; s < num_samples; ++s)
        {
            size_t x = static_cast<size_t>(s) % layer.width();
            size_t y = static_cast<size_t>(s) / layer.width();
            for (size_t c{0}; c < layer.channels(); ++c)
            {
                Real value{static_cast<Real>(_rng()) * _scale};
                if (c < _multiplier.size())
                {
                    value *= _multiplier[c];
                }
                value += _offset;
                layer(x, y, c) = value;
            }
        }

        return true;
    }

    template <typename Real, typename RNG>
    auto NoiseOperator<Real, RNG>::operator()(Layer<Real>& layer, Layer<Real>& mask) -> bool
    {
        assert(mask.channels() > 0);

        int num_samples = static_cast<int>(layer.width() * layer.height());
        for (int s{0}; s < num_samples; ++s)
        {
            size_t x = static_cast<size_t>(s) % layer.width();
            size_t y = static_cast<size_t>(s) / layer.width();
            for (size_t c{0}; c < layer.channels(); ++c)
            {
                Real value{static_cast<Real>(_rng()) * _scale};
                if (c < _multiplier.size())
                {
                    value *= _multiplier[c];
                }
                value += _offset;
                Real t{mask(x, y, 0)};
                layer(x, y, c) = (static_cast<Real>(1.0) - t) * layer(x, y, c) + t * value;
            }
        }

        return true;
    }
}
