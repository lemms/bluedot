// fbmop.h
// FBM operation
// Copyright Laurence Emms 2017

#pragma once
#include "op.h"
#include "layer.h"

namespace bluedot {
    template <typename Real, typename RNG>
    class FBMOperator : public UnaryOperator<Real> {
    public:
        FBMOperator(RNG& rng,
                    size_t octaves,
                    Real exponent,
                    const std::vector<Real>& multiplier = {static_cast<Real>(0.0), static_cast<Real>(0.0), static_cast<Real>(0.0), static_cast<Real>(0.0)},
                    Real scale = static_cast<Real>(1.0),
                    Real offset = static_cast<Real>(0.0),
                    bool spherical = true);
        virtual auto operator()(Layer<Real>& layer) -> bool;
        virtual auto operator()(Layer<Real>& layer, Layer<Real>& mask) -> bool;
    private:
        RNG& _rng;
        size_t _octaves;
        Real _exponent;
        std::vector<Real> _multiplier;
        Real _scale;
        Real _offset;
        bool _spherical;
    };
}

#include "fbmop.hpp"