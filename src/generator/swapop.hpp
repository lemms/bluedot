// swapop.hpp
// Copyright Laurence Emms 2017

namespace bluedot {
    template <typename Real>
    auto SwapOperator<Real>::operator()(Layer<Real>& layer0, Layer<Real>& layer1) -> bool
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
            for (size_t c{0}; c < layer0.channels(); ++c)
            {
                Real value{layer0(x, y, c)};
                layer0(x, y, c) = layer1(x, y, c);
                layer1(x, y, c) = value;
            }
        }

        return true;
    }

    template <typename Real>
    auto SwapOperator<Real>::operator()(Layer<Real>& layer0, Layer<Real>& layer1, Layer<Real>& mask) -> bool
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
            for (size_t c{0}; c < layer0.channels(); ++c)
            {
                if (mask(x, y, 0) <= static_cast<Real>(0.0))
                    continue;
                Real value{layer0(x, y, c)};
                layer0(x, y, c) = layer1(x, y, c);
                layer1(x, y, c) = value;
            }
        }

        return true;
    }
}
