// gradientop.h
// Compute the gradient of the alpha channel
// Stores the gradient in the R and G channels
// Copyright Laurence Emms

#pragma once
#include "op.h"
#include "layer.h"

namespace bluedot {
    template <typename Real>
    class GradientOperator : public UnaryOperator<Real> {
    public:
        GradientOperator(const std::vector<Real>& multiplier = {static_cast<Real>(0.0), static_cast<Real>(0.0), static_cast<Real>(0.0), static_cast<Real>(0.0)},
                         Real scale = static_cast<Real>(1.0),
                         Real offset = static_cast<Real>(0.0),
                         bool spherical = true);
        virtual auto operator()(Layer<Real>& layer) -> bool;
        virtual auto operator()(Layer<Real>& layer, Layer<Real>& mask) -> bool;
    private:
        std::vector<Real> _multiplier;
        Real _scale;
        Real _offset;
        bool _spherical;
    };
}

#include "gradientop.hpp"