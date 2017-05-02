// generator.h
// Copyright Laurence Emms 2017

#pragma once
#include <map>
#include "layer.h"
#include "op.h"

namespace bluedot
{
    template <typename Real>
    class Generator {
    public:
        auto create_layer(const std::string& name, size_t width, size_t height, size_t channels) -> void;
        auto apply_unary_operator(const std::string& layer, UnaryOperator<Real>& op, const std::string& mask = "") -> void;
        auto apply_binary_operator(const std::string& layer0, const std::string& layer1, BinaryOperator<Real>& op, const std::string& mask = "") -> void;
        auto operator()(const std::string& layer, size_t x, size_t y, size_t channel) -> Real;
        auto operator()(const std::string& layer, size_t x, size_t y, size_t channel) const -> Real;
    private:
        std::map<std::string, Layer<Real>> _layers;
    };
}

#include "generator.hpp"