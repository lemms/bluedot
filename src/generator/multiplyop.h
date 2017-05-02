// multiplyop.h
// Multiply operation
// Copyright Laurence Emms 2017

#pragma once
#include "op.h"
#include "layer.h"

namespace bluedot {
    template <typename Real>
    class MultiplyOperator : public BinaryOperator<Real> {
    public:
        MultiplyOperator(const std::vector<Real>& multiplier = {static_cast<Real>(0.0), static_cast<Real>(0.0), static_cast<Real>(0.0), static_cast<Real>(0.0)},
                         Real scale = static_cast<Real>(1.0),
                         Real offset = static_cast<Real>(0.0));
        virtual auto operator()(Layer<Real>& layer0, Layer<Real>& layer1) -> bool;
        virtual auto operator()(Layer<Real>& layer0, Layer<Real>& layer1, Layer<Real>& mask) -> bool;
    private:
        std::vector<Real> _multiplier;
        Real _scale;
        Real _offset;
    };
}

#include "multiplyop.hpp"
