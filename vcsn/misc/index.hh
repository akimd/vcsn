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

      // Disallow index1_t i{index2_t{42}};
      template <typename T>
      index_t_impl(index_t_impl<T> t) = delete;

      bool operator==(index_t_impl t) const
      {
        return s == t.s;
      }

      // Disallow index1_t{42} ==  index2_t{42};
      template <typename T>
      bool operator==(index_t_impl<T> t) const = delete;

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
