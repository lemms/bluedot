// noisemop.h
// Noise operation
// Copyright Laurence Emms 2017

#pragma once
#include "op.h"
#include "layer.h"

namespace bluedot {
    template <typename Real, typename RNG>
    class NoiseOperator : public UnaryOperator<Real> {
    public:
        NoiseOperator(RNG& rng,
                      const std::vector<Real>& multiplier = {static_cast<Real>(0.0), static_cast<Real>(0.0), static_cast<Real>(0.0), static_cast<Real>(0.0)},
                      Real scale = static_cast<Real>(1.0),
                      Real offset = static_cast<Real>(0.0));
        virtual auto operator()(Layer<Real>& layer) -> bool;
        virtual auto operator()(Layer<Real>& layer, Layer<Real>& mask) -> bool;
    private:
        RNG& _rng;
        std::vector<Real> _multiplier;
        Real _scale;
        Real _offset;
    };
}

#include "noiseop.hpp"