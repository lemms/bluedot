// fillop.hpp
// Copyright Laurence Emms 2017

namespace bluedot {
    template <typename Real>
    FillOperator<Real>::FillOperator(const std::vector<Real>& multiplier, Real scale, Real offset) : _multiplier(multiplier), _scale(scale), _offset(offset)
    {
    }

    template <typename Real>
    auto FillOperator<Real>::operator()(Layer<Real>& layer) -> bool
    {
        int num_samples = static_cast<int>(layer.width() * layer.height());
#pragma omp parallel for schedule(guided)
        for (int s{0}; s < num_samples; ++s)
        {
            size_t x = static_cast<size_t>(s) % layer.width();
            size_t y = static_cast<size_t>(s) / layer.width();
            for (size_t c{0}; c < layer.channels(); ++c)
            {
                Real value{_scale};
                if (c < _multiplier.size())
                {
                    value *= _multiplier[c];
                }
                value += _offset;
                layer(x, y, c) += value;
            }
        }

        return true;
    }

    template <typename Real>
    auto FillOperator<Real>::operator()(Layer<Real>& layer, Layer<Real>& mask) -> bool
    {
        assert(mask.channels() > 0);

        int num_samples = static_cast<int>(layer.width() * layer.height());
#pragma omp parallel for schedule(guided)
        for (int s{0}; s < num_samples; ++s)
        {
            size_t x = static_cast<size_t>(s) % layer.width();
            size_t y = static_cast<size_t>(s) / layer.width();
            for (size_t c{0}; c < layer.channels(); ++c)
            {
                Real value{mask(x, y, 0) * _scale};
                if (c < _multiplier.size())
                {
                    value *= _multiplier[c];
                }
                value += _offset;
                layer(x, y, c) += value;
            }
        }

        return true;
    }
}