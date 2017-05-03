// maddop.h
// Multiply and add operator
// Copyright Laurence Emms 2017

#pragma once
#include "op.h"
#include "layer.h"

namespace bluedot {
    template <typename Real>
    class MADDOperator : public UnaryOperator<Real> {
    public:
        MADDOperator(const std::vector<Real>& multiplier = {static_cast<Real>(0.0), static_cast<Real>(0.0), static_cast<Real>(0.0), static_cast<Real>(0.0)},
                     Real scale = static_cast<Real>(1.0),
                     Real offset = static_cast<Real>(0.0));
        virtual auto operator()(Layer<Real>& layer) -> bool;
        virtual auto operator()(Layer<Real>& layer, Layer<Real>& mask) -> bool;
    private:
        std::vector<Real> _multiplier;
        Real _scale;
        Real _offset;
    };
}

#include "maddop.hpp"
