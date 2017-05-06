// normalizeop.hpp
// Copyright Laurence Emms 2017

namespace bluedot {
    template <typename Real>
    NormalizeOperator<Real>::NormalizeOperator()
    {
    }

    template <typename Real>
    auto NormalizeOperator<Real>::operator()(Layer<Real>& layer) -> bool
    {
        int num_samples = static_cast<int>(layer.width() * layer.height());
        Real min_value{std::numeric_limits<Real>::max()};
        Real max_value{-std::numeric_limits<Real>::max()};
        for (int s{0}; s < num_samples; ++s)
        {
            size_t x = static_cast<size_t>(s) % layer.width();
            size_t y = static_cast<size_t>(s) / layer.width();
            for (size_t c{1}; c < layer.channels(); ++c)
            {
                min_value = std::min(min_value, layer(x, y, c));
                max_value = std::max(max_value, layer(x, y, c));
            }
        }

        Real range{static_cast<Real>(1.0) / (max_value - min_value)};
#pragma omp parallel for schedule(guided)
        for (int s{0}; s < num_samples; ++s)
        {
            size_t x = static_cast<size_t>(s) % layer.width();
            size_t y = static_cast<size_t>(s) / layer.width();
            for (size_t c{1}; c < layer.channels(); ++c)
            {
                layer(x, y, c) = (layer(x, y, c) - min_value) * range;
            }
        }

        return true;
    }

    template <typename Real>
    auto NormalizeOperator<Real>::operator()(Layer<Real>& layer, Layer<Real>& mask) -> bool
    {
        assert(mask.channels() > 0);

        int num_samples = static_cast<int>(layer.width() * layer.height());
        Real min_value{std::numeric_limits<Real>::max()};
        Real max_value{-std::numeric_limits<Real>::max()};
        for (int s{0}; s < num_samples; ++s)
        {
            size_t x = static_cast<size_t>(s) % layer.width();
            size_t y = static_cast<size_t>(s) / layer.width();
            for (size_t c{1}; c < layer.channels(); ++c)
            {
                min_value = std::min(min_value, layer(x, y, c));
                max_value = std::max(max_value, layer(x, y, c));
            }
        }

        Real range{static_cast<Real>(1.0) / (max_value - min_value)};
#pragma omp parallel for schedule(guided)
        for (int s{0}; s < num_samples; ++s)
        {
            size_t x = static_cast<size_t>(s) % layer.width();
            size_t y = static_cast<size_t>(s) / layer.width();
            for (size_t c{1}; c < layer.channels(); ++c)
            {
                Real t{mask(x, y, 0)};
                layer(x, y, c) = (static_cast<Real>(1.0) - t) * layer(x, y, c) + t * (layer(x, y, c) - min_value) * range;
            }
        }

        return true;
    }
}