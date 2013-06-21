#ifndef VCSN_MISC_MILITARY_ORDER_HH
# define VCSN_MISC_MILITARY_ORDER_HH

namespace vcsn
{
  /// Military strict order predicate.
  /// This predicate applies to any type which provides a length method.
  /// If two elements have the same length they are compared with operator<,
  /// otherwise, the shorter is the smaller.
  ///
  /// Known algorithms using this:
  /// enumerate return a list sorted w.r.t military order.
  template <typename T>
  struct MilitaryOrder
  {
    bool operator()(const T& x, const T& y) const
    {
      if (x.length() == y.length())
        return x < y;
      else
        return x.length() < y.length();
    }
  };
}

#endif // ! VCSN_MISC_MILITARY_ORDER_HH
