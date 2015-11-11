#pragma once

namespace vcsn
{
  namespace detail
  {
    /// Lightweight state/transition handle (or index).
    template <typename Tag>
    struct index_t_impl
    {
      index_t_impl(unsigned i)
        : s{i}
      {}
      /// Default ctor to please containers.
      index_t_impl() = default;
      operator unsigned() const { return s; }
      /// Be compliant with Boost integer ranges.
      index_t_impl& operator++() { ++s; return *this; }
      /// Be compliant with Boost integer ranges.
      index_t_impl& operator--() { --s; return *this; }

    private:
      unsigned s;
    };
  }
}

namespace std
{
  /// Hash indexes.
  template <typename Tag>
  struct hash<vcsn::detail::index_t_impl<Tag>>
  {
    using index_t = vcsn::detail::index_t_impl<Tag>;
    size_t operator()(index_t s) const noexcept
    {
      return static_cast<size_t>(static_cast<unsigned>(s));
    }
  };
}
