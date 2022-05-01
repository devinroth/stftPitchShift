#pragma once

#include <algorithm>
#include <cmath>
#include <complex>
#include <vector>

namespace stftpitchshift
{
  template<class T>
  class Resampler
  {

  public:

    Resampler(const double factor = 1.0) :
      factor(factor)
    {
    }

    Resampler(const Resampler& other) :
      factor(other.factor)
    {
    }

    Resampler& operator=(const Resampler& other)
    {
      if(this != &other)
      {
        this->factor = other.factor;
      }

      return *this;
    }

    void cosine(const std::vector<std::complex<T>>& x,
                std::vector<std::complex<T>>& y) const
    {
      if (factor == 1)
      {
        y = x;
        return;
      }

      const double PI = std::acos(-1.0);

      const size_t N = x.size();
      const size_t M = static_cast<size_t>(std::round(N * factor));

      for (size_t m = 0; m < std::min(M, N); ++m)
      {
        const double n = m / factor;

        const ptrdiff_t n_ = static_cast<ptrdiff_t>(n);
        const ptrdiff_t n0 = n_ - (n < n_); // std::floor(n)
        const ptrdiff_t n1 = n_ + (n > n_); // std::ceil(n)

        if (n0 < 0 || ptrdiff_t(N) <= n0)
        {
          continue;
        }

        if (n1 < 0 || ptrdiff_t(N) <= n1)
        {
          continue;
        }

        if (n0 == n1)
        {
          y[m] = x[n0];
          continue;
        }

        const std::complex<T> y0 = x[n0];
        const std::complex<T> y1 = x[n1];

        const T i = static_cast<T>((n - n0) / (n1 - n0));
        const T j = static_cast<T>(0.5 - 0.5 * std::cos(i * PI));

        y[m] = y0 * (1 - j) + y1 * j;
      }
    }

    void linear(const std::vector<std::complex<T>>& x,
                std::vector<std::complex<T>>& y) const
    {
      if (factor == 1)
      {
        y = x;
        return;
      }

      const size_t N = x.size();
      const size_t M = static_cast<size_t>(std::round(N * factor));

      for (size_t m = 0; m < std::min(M, N); ++m)
      {
        const double n = m / factor;

        const ptrdiff_t n_ = static_cast<ptrdiff_t>(n);
        const ptrdiff_t n0 = n_ - (n < n_); // std::floor(n)
        const ptrdiff_t n1 = n_ + (n > n_); // std::ceil(n)

        if (n0 < 0 || ptrdiff_t(N) <= n0)
        {
          continue;
        }

        if (n1 < 0 || ptrdiff_t(N) <= n1)
        {
          continue;
        }

        if (n0 == n1)
        {
          y[m] = x[n0];
          continue;
        }

        const std::complex<T> y0 = x[n0];
        const std::complex<T> y1 = x[n1];

        const T i = static_cast<T>((n - n0) / (n1 - n0));

        y[m] = y0 * (1 - i) + y1 * i;
      }
    }

    void bilinear(const std::vector<std::complex<T>>& x0,
                  const std::vector<std::complex<T>>& x1,
                  std::vector<std::complex<T>>& y) const
    {
      std::vector<std::complex<T>> y0(x0.size());
      std::vector<std::complex<T>> y1(x1.size());

      linear(x0, y0);
      linear(x1, y1);

      for (size_t i = 0; i < y.size(); ++i)
      {
        y[i] = (y0[i] + y1[i]) / T(2);
      }
    }

  private:

    double factor;

  };
}
