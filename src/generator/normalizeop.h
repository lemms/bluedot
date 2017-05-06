// normalizeop.h
// Normalize operator
// Normalizes the color channels into the range 0-1
// Copyright Laurence Emms 2017

#pragma once
#include "op.h"
#include "layer.h"

namespace bluedot {
    template <typename Real>
    class NormalizeOperator : public UnaryOperator<Real> {
    public:
        NormalizeOperator();
        virtual auto operator()(Layer<Real>& layer) -> bool;
        virtual auto operator()(Layer<Real>& layer, Layer<Real>& mask) -> bool;
    };
}

#include "normalizeop.hpp"