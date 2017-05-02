// fbmop.hpp
// Copyright Laurence Emms 2017

#include "fbm.h"
#include <boost/random.hpp>
#include <omp.h>

namespace bluedot {
    template <typename Real, typename RNG>
    FBMOperator<Real, RNG>::FBMOperator(RNG& rng, size_t octaves, Real exponent, const std::vector<Real>& multiplier, Real scale, Real offset) :
        _rng(rng), _octaves(octaves), _exponent(exponent), _multiplier(multiplier), _scale(scale), _offset(offset)
    {
    }

    template <typename Real, typename RNG>
    auto FBMOperator<Real, RNG>::operator()(Layer<Real>& layer) -> bool
    {
        FBM<Real, RNG> fbm{_rng, layer.width(), layer.height(), _octaves, _exponent};
        int num_samples = static_cast<int>(layer.width() * layer.height());
#pragma omp parallel for schedule(guided)
        for (int s{0}; s < num_samples; ++s)
        {
            size_t x = static_cast<size_t>(s) % layer.width();
            size_t y = static_cast<size_t>(s) / layer.width();
            for (size_t c{0}; c < layer.channels(); ++c)
            {
                Real value{fbm(x, y) * _scale};
                if (c < _multiplier.size())
                {
                    value *= _multiplier[c];
                }
                value += _offset;
                layer(x, y, c) += value;
            }
        }

        return true;
    }

    template <typename Real, typename RNG>
    auto FBMOperator<Real, RNG>::operator()(Layer<Real>& layer, Layer<Real>& mask) -> bool
    {
        assert(mask.channels() > 0);

        FBM<Real, RNG> fbm{_rng, layer.width(), layer.height(), _octaves, _exponent};
        int num_samples = static_cast<int>(layer.width() * layer.height());
#pragma omp parallel for schedule(guided)
        for (int s{0}; s < num_samples; ++s)
        {
            size_t x = static_cast<size_t>(s) % layer.width();
            size_t y = static_cast<size_t>(s) / layer.width();
            for (size_t c{0}; c < layer.channels(); ++c)
            {
                Real value{fbm(x, y) * mask(x, y, 0) * _scale};
                if (c < _multiplier.size())
                {
                    value *= _multiplier[c];
                }
                value += _offset;
                layer(x, y, c) += value;
            }
        }

        return true;
    }
}