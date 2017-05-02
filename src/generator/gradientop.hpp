// gradientop.hpp
// Copyright Laurence Emms 2017

namespace bluedot {
    template <typename Real>
    GradientOperator<Real>::GradientOperator(const std::vector<Real>& multiplier, Real scale, Real offset, bool spherical = true) : _multiplier(multiplier), _scale(scale), _offset(offset), _spherical(spherical)
    {
    }

    template <typename Real>
    auto GradientOperator<Real>::operator()(Layer<Real>& layer) -> bool
    {
        if (layer.channels() < 3)
        {
            return false;
        }
        int num_samples = static_cast<int>(layer.width() * layer.height());
#pragma omp parallel for schedule(guided)
        for (int s{0}; s < num_samples; ++s)
        {
            size_t x{static_cast<size_t>(s) % layer.width()};
            size_t y{static_cast<size_t>(s) / layer.width()};

            size_t up{std::min(layer.height(), y + 1)};
            size_t down{static_cast<size_t>(std::max(0, static_cast<int>(y) - 1))};
            size_t right{0};
            size_t left{0};
            if (_spherical)
            {
                right = (x + 1 >= layer.width()) ? x + 1 - layer.width() : x + 1;
                left = (static_cast<int>(x) - 1 < 0) ? x + layer.width() - 1 : x - 1;
            }
            else
            {
                right = std::min(layer.width(), x + 1);
                left = static_cast<size_t>(std::max(0, static_cast<int>(x) - 1));
            }

            Real right_alpha{layer(right, y, 0)};
            Real left_alpha{layer(left, y, 0)};
            Real up_alpha{layer(x, up, 0)};
            Real down_alpha{layer(x, down, 0)};
            Real x_gradient{(right_alpha - left_alpha) * 0.5f * _scale};
            Real y_gradient{(up_alpha - down_alpha) * 0.5f * _scale};
            if (_multiplier.size() > 1)
            {
                x_gradient *= _multiplier[1];
            }
            if (_multiplier.size() > 2)
            {
                y_gradient *= _multiplier[2];
            }
            x_gradient += _offset;
            y_gradient += _offset;
            layer(x, y, 1) = x_gradient;
            layer(x, y, 2) = y_gradient;
        }

        return true;
    }

    template <typename Real>
    auto GradientOperator<Real>::operator()(Layer<Real>& layer, Layer<Real>& mask) -> bool
    {
        assert(mask.channels() > 0);
        if (layer.channels() < 3)
        {
            return false;
        }

        int num_samples = static_cast<int>(layer.width() * layer.height());
#pragma omp parallel for schedule(guided)
        for (int s{0}; s < num_samples; ++s)
        {
            size_t x{static_cast<size_t>(s) % layer.width()};
            size_t y{static_cast<size_t>(s) / layer.width()};
            if (mask(x, y, 0) <= static_cast<Real>(0.0))
                continue;

            size_t up{std::min(layer.height(), y + 1)};
            size_t down{static_cast<size_t>(std::max(0, static_cast<int>(y) - 1))};
            size_t right{0};
            size_t left{0};
            if (_spherical)
            {
                right = (x + 1 >= layer.width()) ? x + 1 - layer.width() : x + 1;
                left = (static_cast<int>(x) - 1 < 0) ? x + layer.width() - 1 : x - 1;
            }
            else
            {
                right = std::min(layer.width(), x + 1);
                left = static_cast<size_t>(std::max(0, static_cast<int>(x) - 1));
            }

            Real right_alpha{layer(right, y, 0)};
            Real left_alpha{layer(left, y, 0)};
            Real up_alpha{layer(x, up, 0)};
            Real down_alpha{layer(x, down, 0)};
            Real x_gradient{(right_alpha - left_alpha) * 0.5f * _scale};
            Real y_gradient{(up_alpha - down_alpha) * 0.5f * _scale};
            if (_multiplier.size() > 1)
            {
                x_gradient *= _multiplier[1];
            }
            if (_multiplier.size() > 2)
            {
                y_gradient *= _multiplier[2];
            }
            x_gradient += _offset;
            y_gradient += _offset;
            layer(x, y, 1) = x_gradient;
            layer(x, y, 2) = y_gradient;
        }

        return true;
    }
}
