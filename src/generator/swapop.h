// swapop.h
// Layer swap operation
// Copyright Laurence Emms 2017

#pragma once
#include "op.h"
#include "layer.h"

namespace bluedot {
    template <typename Real>
    class SwapOperator : public BinaryOperator<Real> {
    public:
        virtual auto operator()(Layer<Real>& layer0, Layer<Real>& layer1) -> bool;
        virtual auto operator()(Layer<Real>& layer0, Layer<Real>& layer1, Layer<Real>& mask) -> bool;
    };
}

#include "swapop.hpp"
