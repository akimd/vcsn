#ifndef VCSN_MISC_ZIP_MAPS_HH
# define VCSN_MISC_ZIP_MAPS_HH

# include <cassert>
# include <type_traits>

# include <vcsn/misc/raise.hh> // pass
# include <vcsn/misc/tuple.hh>

# if 0
#  include <vcsn/misc/echo.hh>
# else
#  undef SHOWH
#  define SHOWH(Args)
#  undef V
#  define V(Args)
# endif

namespace vcsn
{

  template <typename... Maps>
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
        , ranges_{{is, is}...}
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
      /// Current ranges.
      ranges_t ranges_;
      /// Whether we reached the end.
      bool is_done_;

      /// Advance to next position.
      iterator& operator++()
      {
        if (next_() == -1)
          {
            SHOWH("Step");
            step_();
            align_();
          }
        return *this;
      }

      bool operator!=(const iterator& that) const
      {
        return not_equal_(that, indices_t{});
      }

      std::pair<key_t, mapped_t> operator*()
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
        detail::pass
          {
            (std::get<I>(ranges_).first
             = std::get<I>(ranges_).second
             = std::get<I>(is_)
             = std::get<I>(ends_),
             is_done_ = true)...
          };
      }

      /// Move to the next position in the current range.  Return true
      /// iff there is such a position.
      int next_()
      {
        auto res = next_(indices_t{});
        // Reset all the iterators that are before the first one that could
        // advance.
        if (res != -1)
          reset_up_to_(res);
        SHOWH(V(res));
        return res;
      }

      template <std::size_t... I>
      int next_(seq<I...>)
      {
        int res = -1;
        detail::pass
          {
            (res != -1
             || (std::get<I>(is_) != std::get<I>(ranges_).second
                 && (std::next(std::get<I>(is_)) != std::get<I>(ranges_).second
                     ? ++std::get<I>(is_), res = I
                     : I)))...
          };
        return res;
      }

      /// Move beginning of ranges to their end, and align.
      void reset_up_to_(int n)
      {
        reset_up_to_(n, indices_t{});
      }

      template <std::size_t... I>
      void reset_up_to_(size_t n, seq<I...>)
      {
        detail::pass
          {
            (I < n
             && ((std::get<I>(is_) = std::get<I>(ranges_).first), true))...
          };
      }

      /// Move beginning of ranges to their end, and align.
      void step_()
      {
        step_(indices_t{});
      }

      template <std::size_t... I>
      void step_(seq<I...>)
      {
        detail::pass{(std::get<I>(ranges_).first = std::get<I>(ranges_).second)...};
      }

      /// Set up the next ranges: the next common key, and the
      /// associated ranges.  Does not advance, just aligns all ranges.
      void align_()
      {
        key_t k = std::get<0>(is_)->first;
        while (!is_done_)
          {
            auto k2 = align_(k, indices_t{});
            SHOWH(V(k) << V(k2));
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
        detail::pass{ (is_done_ || (k = align_<I>(k)))... };
        return k;
      }

      /// Given the current candidate key, try to find the proper range
      /// for I.  Return the first supported key <= k.
      template<std::size_t I>
      key_t align_(key_t k)
      {
        assert(!is_done_);
        auto& first = std::get<I>(ranges_).first;
        auto& second = std::get<I>(ranges_).second;

        // Look for the beginning of the range for key k.
        // first = second;
        while (first != std::get<I>(ends_) && first->first < k)
          ++first;
        if (first == std::get<I>(ends_))
          {
            // Nothing left.
            SHOWH("Done: " << V(I) << V(k));
            done_();
            return k;
          }
        else
          {
            // We have a range, find its end.
            k = first->first;
            SHOWH("Found: " << V(I) << V(k));
            second = first;
            while (second != std::get<I>(ends_) && second->first == k)
              ++second;
            std::get<I>(is_) = first;
            return k;
          }
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
        return mapped_t{(std::get<I>(is_)->second)...};
      }

      /// Tuple of pairs.
      template <std::size_t... I>
      values_t dereference_(seq<I...>) const
      {
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

  template <typename... Maps>
  zipped_maps<Maps...> zip_maps(Maps&&... maps)
  {
    return {std::forward<Maps>(maps)...};
  }
}

#endif // !VCSN_MISC_ZIP_MAPS_HH
