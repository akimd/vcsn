#ifndef VCSN_MISC_ZIP_MAPS_HH
# define VCSN_MISC_ZIP_MAPS_HH

# include <cassert>
# include <type_traits>

# include <vcsn/misc/raise.hh> // pass
# include <vcsn/misc/tuple.hh>

namespace vcsn
{

  struct as_tuple {};
  struct as_pair {};

  template <typename Dereference = as_tuple, typename... Maps>
  struct zipped_maps
  {
    /// Type of the tuple of all the maps.
    using maps_t = std::tuple<Maps...>;

    /// Type of index sequences.
    template <std::size_t... I>
    using seq = vcsn::detail::index_sequence<I...>;

    /// Index sequence for our maps.
    using indices_t = vcsn::detail::make_index_sequence<sizeof...(Maps)>;

    zipped_maps(Maps... maps)
      : maps_(maps...)
    {}

    zipped_maps(const maps_t& maps)
      : maps_(maps)
    {}

    /// Composite iterator.
    struct iterator
    {
      using iterators_t
        = std::tuple<typename std::remove_reference<Maps>::type::iterator...>;
      using values_t
        = std::tuple<typename std::remove_reference<Maps>::type::value_type...>;
      using ranges_t
        = std::tuple<std::pair<typename std::remove_reference<Maps>::type::iterator,
                               typename std::remove_reference<Maps>::type::iterator>...>;
      /// Common key type.
      using key_t
        = typename std::remove_const<typename std::tuple_element<0, values_t>::type::first_type>::type;
      /// Tuple of mapped types.
      using mapped_t
        = std::tuple<typename std::remove_reference<Maps>::type::mapped_type...>;

      iterator(zipped_maps& zip,
               typename std::remove_reference<Maps>::type::iterator... is,
               typename std::remove_reference<Maps>::type::iterator... ends)
        : zip_(zip)
        , is_(is...)
        , ends_(ends...)
        , is_done_(false)
      {
        align_();
      }

      /// The maps etc.
      zipped_maps& zip_;

      /// The current position.
      iterators_t is_;
      /// The genuine ends.
      iterators_t ends_;
      /// Whether we reached the end.
      bool is_done_;

      /// Advance to next position.
      iterator& operator++()
      {
        if (next_() == -1)
          {
            step_();
            align_();
          }
        return *this;
      }

      bool operator!=(const iterator& that) const
      {
        return not_equal_(that, indices_t{});
      }

      std::pair<key_t, mapped_t> dereference_(as_pair)
      {
        return dereference_as_pair();
      }

      values_t dereference_(as_tuple)
      {
        return dereference_as_tuple();
      }

      auto operator*()
        // GCC wants "this->", clang does not need it.
        -> decltype(this->dereference_(Dereference()))
      {
        return dereference_(Dereference());
      }

      /// Return as <<k1, v1>, <k1, v2>, ...>.
      values_t dereference_as_tuple()
      {
        return dereference_(indices_t{});
      }

      /// Return as <k1, <v1, v2...>>.
      std::pair<key_t, mapped_t> dereference_as_pair()
      {
        return {dereference_first_(), dereference_second_(indices_t{})};
      }

    private:
      /// We have reached the end, move all the cursors to this end.
      void done_()
      {
        done_(indices_t{});
      }

      template<std::size_t... I>
      void done_(seq<I...>)
      {
        using swallow = bool[];
        (void) swallow
          {
            (std::get<I>(is_) = std::get<I>(ends_),
             is_done_ = true)...
          };
      }

      /// Move to the next position in the current range.  Return true
      /// iff there is such a position.
      int next_()
      {
        return -1;
      }

      /// Move beginning of ranges to their end, and align.
      void reset_up_to_(int)
      {
      }

      /// Move beginning of ranges to their end, and align.
      void step_()
      {
        ++std::get<0>(is_);
      }

      template <std::size_t... I>
      void step_(seq<I...>)
      {
      }

      /// Set up the next ranges: the next common key, and the
      /// associated ranges.  Does not advance, just aligns all ranges.
      void align_()
      {
        key_t k = std::get<0>(is_)->first;
        while (!is_done_)
          {
            auto k2 = align_(k, indices_t{});
            if (k == k2)
              // We have found a common key for all maps.
              break;
            else
              k = k2;
          }
      }

      /// Try to align all the ranges to support key k.  Return the
      /// smallest candidate key.
      template <std::size_t... I>
      key_t align_(key_t k, seq<I...>)
      {
        using swallow = int[];
        (void) swallow{ ((is_done_ || (k = align_<I>(k))), 0)... };
        return k;
      }

      /// Given the current candidate key, try to find the proper range
      /// for I.  Return the first supported key <= k.
      template<std::size_t I>
      key_t align_(key_t k)
      {
        assert(!is_done_);
        auto& first = std::get<I>(is_);

        // Look for the beginning of the range for key k.
        // first = second;
        while (first != std::get<I>(ends_) && first->first < k)
          ++first;
        if (first == std::get<I>(ends_))
          // Nothing left.
          done_();
        else
          // Found something, return its key.
          k = first->first;
        return k;
      }

      template <std::size_t... I>
      bool not_equal_(const iterator& that, seq<I...>) const
      {
        for (auto n: {(std::get<I>(is_) != std::get<I>(that.is_))...})
          if (n)
            return true;
        return false;
      }

      /// The common key.
      key_t dereference_first_() const
      {
        return std::get<0>(is_)->first;
      }

      /// The associated tuple of values.
      template <std::size_t... I>
      mapped_t dereference_second_(seq<I...>) const
      {
        // clang 3.4 on top of libstdc++ wants this ctor to be
        // explicitly called.
        return mapped_t{(std::get<I>(is_)->second)...};
      }

      /// Tuple of pairs.
      template <std::size_t... I>
      values_t dereference_(seq<I...>) const
      {
        // clang 3.4 on top of libstdc++ wants this ctor to be
        // explicitly called:

        // vcsn/misc/zip-maps.hh:275:16: error: chosen constructor is
        //                     explicit in copy-initialization
        //  return {(*std::get<I>(is_))...};
        //         ^~~~~~~~~~~~~~~~~~~~~~~~
        return values_t{(*std::get<I>(is_))...};
      }
    };

    iterator begin()
    {
      auto res = begin_(indices_t{});
      return res;
    }

    iterator end()
    {
      return end_(indices_t{});
    }

  private:
    template <std::size_t... I>
    iterator begin_(seq<I...>)
    {
      return iterator(*this,
                      std::get<I>(maps_).begin()...,
                      std::get<I>(maps_).end()...);
    }

    template <std::size_t... I>
    iterator end_(seq<I...>)
    {
      return iterator(*this,
                      std::get<I>(maps_).end()...,
                      std::get<I>(maps_).end()...);
    }

    maps_t maps_;
  };

  template <typename Dereference = as_pair, typename... Maps>
  zipped_maps<Dereference, Maps...>
  zip_maps(Maps&&... maps)
  {
    return {std::forward<Maps>(maps)...};
  }

  template <typename Dereference = as_pair, typename... Maps>
  zipped_maps<Dereference, Maps...>
  zip_map_tuple(const std::tuple<Maps...>& maps)
  {
    return {maps};
  }
}

#endif // !VCSN_MISC_ZIP_MAPS_HH
