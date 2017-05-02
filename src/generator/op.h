// op.h
// Operations on layers
// Copyright Laurence Emms 2017

#pragma once
#include "layer.h"

namespace bluedot {
    template <typename Real>
    class UnaryOperator {
    public:
        virtual auto operator()(Layer<Real>& layer) -> bool = 0;
        virtual auto operator()(Layer<Real>& layer, Layer<Real>& mask) -> bool = 0;
    };

    template <typename Real>
    class BinaryOperator {
    public:
        virtual auto operator()(Layer<Real>& layer0, Layer<Real>& layer1) -> bool = 0;
        virtual auto operator()(Layer<Real>& layer0, Layer<Real>& layer1, Layer<Real>& mask) -> bool = 0;
    };
}