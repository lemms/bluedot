// generator.hpp
// Copyright Laurence Emms 2017

#include <tuple>

namespace bluedot {
    template <typename Real>
    auto Generator<Real>::create_layer(const std::string& name, size_t width, size_t height, size_t channels) -> void
    {
        _layers.insert(std::make_pair(name, Layer<Real>(width, height, channels)));
    }

    template <typename Real>
    auto Generator<Real>::apply_unary_operator(const std::string& layer, UnaryOperator<Real>& op, const std::string& mask) -> bool
    {
        auto l = _layers.find(layer);
        if (l == _layers.end())
            return false;
        if (mask == "")
        {
            return op(l->second);
        }
        else
        {
            auto m = _layers.find(mask);
            if (m == _layers.end())
                return false;
            return op(l->second, m->second);
        }
        return false;
    }

    template <typename Real>
    auto Generator<Real>::apply_binary_operator(const std::string& layer0, const std::string& layer1, BinaryOperator<Real>& op, const std::string& mask) -> bool
    {
        auto l0 = _layers.find(layer0);
        if (l0 == _layers.end())
            return false;
        auto l1 = _layers.find(layer1);
        if (l1 == _layers.end())
            return false;
        if (mask == "")
        {
            return op(l0->second, l1->second);
        }
        else
        {
            auto m = _layers.find(mask);
            if (m == _layers.end())
                return false;
            return op(l0->second, l1->second, m->second);
        }
        return false;
    }

    template <typename Real>
    auto Generator<Real>::operator()(const std::string& layer, size_t x, size_t y, size_t channel) -> Real
    {
        auto l = _layers.find(layer);
        if (l == _layers.end())
            return static_cast<Real>(0.0);
        return l->second(x, y, channel);
    }

    template <typename Real>
    auto Generator<Real>::operator()(const std::string& layer, size_t x, size_t y, size_t channel) const -> Real
    {
        auto l = _layers.find(layer);
        if (l == _layers.end())
            return static_cast<Real>(0.0);
        return l->second(x, y, channel);
    }
}