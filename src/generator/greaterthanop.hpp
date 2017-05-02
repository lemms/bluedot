// greaterthanop.hpp
// Copyright Laurence Emms 2017

namespace bluedot {
    template <typename Real>
    GreaterThanOperator<Real>::GreaterThanOperator(const std::vector<Real>& level, bool clamp) : _level(level), _clamp(clamp)
    {
    }

    template <typename Real>
    auto GreaterThanOperator<Real>::operator()(Layer<Real>& layer) -> bool
    {
        int num_samples = static_cast<int>(layer.width() * layer.height());
#pragma omp parallel for schedule(guided)
        for (int s{0}; s < num_samples; ++s)
        {
            size_t x = static_cast<size_t>(s) % layer.width();
            size_t y = static_cast<size_t>(s) / layer.width();
            for (size_t c{0}; c < layer.channels(); ++c)
            {
                if (c < _level.size() && layer(x, y, c) <= _level[c])
                    if (_clamp)
                    {
                        layer(x, y, c) = _level[c];
                    }
                    else
                    {
                        layer(x, y, c) = static_cast<Real>(0.0);
                    }
            }
        }

        return true;
    }

    template <typename Real>
    auto GreaterThanOperator<Real>::operator()(Layer<Real>& layer, Layer<Real>& mask) -> bool
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
                if (mask(x, y, 0) > static_cast<Real>(0.0))
                    if (c < _level.size() && layer(x, y, c) <= _level[c])
                        if (_clamp)
                        {
                            layer(x, y, c) = _level[c];
                        }
                        else
                        {
                            layer(x, y, c) = static_cast<Real>(0.0);
                        }
            }
        }

        return true;
    }
}