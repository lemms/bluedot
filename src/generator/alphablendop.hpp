// alphablendop.hpp
// Copyright Laurence Emms 2017

namespace bluedot {
    template <typename Real>
    AlphaBlendOperator<Real>::AlphaBlendOperator(const std::vector<Real>& multiplier, Real scale, Real offset) : _multiplier(multiplier), _scale(scale), _offset(offset)
    {
    }

    template <typename Real>
    auto AlphaBlendOperator<Real>::operator()(Layer<Real>& layer0, Layer<Real>& layer1) -> bool
    {
        assert(layer0.width() == layer1.width());
        assert(layer0.height() == layer1.height());
        if (layer0.channels() != layer1.channels())
            return false;

        int num_samples = static_cast<int>(layer0.width() * layer0.height());
#pragma omp parallel for schedule(guided)
        for (int s{0}; s < num_samples; ++s)
        {
            size_t x = static_cast<size_t>(s) % layer0.width();
            size_t y = static_cast<size_t>(s) / layer0.width();
            for (size_t c{1}; c < layer0.channels(); ++c)
            {
                Real value{_scale * layer1(x, y, c)};
                if (c < _multiplier.size())
                {
                    value *= _multiplier[c];
                }
                value += _offset;
                Real u{std::max(static_cast<Real>(0.0), std::min(static_cast<Real>(1.0), layer1(x, y, 0)))};
                layer0(x, y, c) = (static_cast<Real>(1.0) - u) * layer0(x, y, c) + u * value;
            }
        }
        return true;
    }

    template <typename Real>
    auto AlphaBlendOperator<Real>::operator()(Layer<Real>& layer0, Layer<Real>& layer1, Layer<Real>& mask) -> bool
    {
        assert(mask.channels() > 0);
        assert(layer0.width() == layer1.width());
        assert(layer0.height() == layer1.height());
        if (layer0.channels() != layer1.channels())
            return false;

        int num_samples = static_cast<int>(layer0.width() * layer0.height());
#pragma omp parallel for schedule(guided)
        for (int s{0}; s < num_samples; ++s)
        {
            size_t x = static_cast<size_t>(s) % layer0.width();
            size_t y = static_cast<size_t>(s) / layer0.width();
            for (size_t c{1}; c < layer0.channels(); ++c)
            {
                Real value{_scale * layer1(x, y, c)};
                if (c < _multiplier.size())
                {
                    value *= _multiplier[c];
                }
                value += _offset;
                Real t{mask(x, y, 0)};
                Real u{std::max(static_cast<Real>(0.0), std::min(static_cast<Real>(1.0), layer1(x, y, 0)))};
                layer0(x, y, c) = (static_cast<Real>(1.0) - t) * layer0(x, y, c) + t * ((static_cast<Real>(1.0) - u) * layer0(x, y, c) + u * value);
            }
        }
        return true;
    }
}
