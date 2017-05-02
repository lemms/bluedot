// lessthanop.h
// Less than pass through operator
// Keep samples less than some level, otherwise set them to 0
// Copyright Laurence Emms 2017

#pragma once
#include "op.h"
#include "layer.h"

namespace bluedot {
    template <typename Real>
    class LessThanOperator : public UnaryOperator<Real> {
    public:
        LessThanOperator(const std::vector<Real>& level = {static_cast<Real>(0.0), static_cast<Real>(0.0), static_cast<Real>(0.0), static_cast<Real>(0.0)}, bool clamp = false);
        virtual auto operator()(Layer<Real>& layer) -> bool;
        virtual auto operator()(Layer<Real>& layer, Layer<Real>& mask) -> bool;
    private:
        std::vector<Real> _level;
        bool _clamp;
    };
}

#include "lessthanop.hpp"
