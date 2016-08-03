#pragma once

#include <vcsn/misc/raise.hh>

namespace vcsn
{
  /// An exponent, or range of exponents.
  ///
  /// The purpose of this class is to model exponents such as in
  /// `a{2}`, `a{2, 3}`, `a{-1}` etc.  To support them, we used to
  /// rely on overloads, e.g.,
  ///
  ///   auto multiply(weight, weight) -> weight;
  ///   auto multiply(weight, int min, int max = min) -> weight;
  ///
  /// unfortunately when on Z, both signatures collide, but not
  /// exactly, so the compiler chose the "best" one, which resulted in
  /// `3 * 3 = 27` in Z.  In turn, this results in `<3><3>a = <27>a`
  /// in Z.
  ///
  /// To avoid this collision, we decided that the second signature
  /// would be
  ///
  ///   auto multiply(weight, to{int min, int max = min}) -> weight;
  class to
  {
  public:
    /// Build a exponent range.
    ///
    /// \param min_  the minimum number.  If -1, denotes 0.
    /// \param max_  the maximum number.
    ///             If -1, denotes infinity, using star.
    ///             If -2, denotes the same value as min.
    to(int min_, int max_)
      : min{min_}
      , max{max_}
    {
      if (max == -2)
        max = min_;
      if (min == -1)
        min = 0;
      require(0 <= min
              && -1 <= max
              && (max == -1 || min <= max),
              "invalid exponents: ", min, ", ", max);
    }

    to(int min_)
      : to{min_, -2}
    {}

    to(unsigned min_)
      : to{int(min_)}
    {}

    /// Whether the max exponent is finte.
    bool finite() const
    {
      return max != -1;
    }

    /// Whether features a single exponent.
    bool single() const
    {
      return min == max;
    }

    int min;
    int max;
  };

  inline std::ostream& operator<<(std::ostream& o, const to& t)
  {
    return o << '{' << t.min << ", " << t.max << '}';
  }
}
