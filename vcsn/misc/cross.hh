#ifndef VCSN_MISC_CROSS_HH
# define VCSN_MISC_CROSS_HH

# include <type_traits>

# include <vcsn/misc/raise.hh> // pass
# include <vcsn/misc/tuple.hh>

namespace vcsn
{

  template <typename... Sequences>
  struct cross_sequences
  {
    /// Type of the tuple of all the maps.
    using sequences_t = std::tuple<Sequences...>;

    /// Type of index sequences.
    template <std::size_t... I>
    using seq = vcsn::detail::index_sequence<I...>;

    static constexpr size_t size = sizeof...(Sequences);

    /// Index sequence for our maps.
    using indices_t = vcsn::detail::make_index_sequence<sizeof...(Sequences)>;

    cross_sequences(const sequences_t& sequences)
      : sequences_(sequences)
    {}

    cross_sequences(Sequences... sequences)
      : sequences_(sequences...)
    {}

    /// Composite iterator.
    struct iterator
    {
      using iterators_t
        = std::tuple<typename std::remove_reference<Sequences>::type::iterator...>;
      using value_type
        = std::tuple<typename std::remove_reference<Sequences>::type::value_type...>;

      iterator(typename std::remove_reference<Sequences>::type::iterator... is,
               typename std::remove_reference<Sequences>::type::iterator... ends)
        : is_{is...}
        , begins_{is...}
        , ends_{ends...}
      {}

      /// The current position.
      iterators_t is_;
      /// The begins.
      iterators_t begins_;
      /// The ends.
      iterators_t ends_;

      /// Advance to next position.
      iterator& operator++()
      {
        if (next_() == -1)
          done_();
        return *this;
      }

      bool operator!=(const iterator& that) const
      {
        return not_equal_(that, indices_t{});
      }

      value_type operator*()
      {
        return dereference_(indices_t{});
      }

    private:
      /// We have reached the end, move all the cursors to this end.
      void done_()
      {
        is_ = ends_;
      }

      /// Move to the next position.  Return the index of the lastest
      /// iterator that could move, -1 if we reached the end.
      int next_()
      {
        auto res = next_(indices_t{});
        // Reset all the iterators that are before the first one that could
        // advance.
        if (res != -1)
          reset_up_to_(res);
        return res;
      }

      template <std::size_t... I>
      int next_(seq<I...>)
      {
        int res = -1;
        using swallow = int[];
        (void) swallow
          {
            (res == -1
             && std::get<size-1-I>(is_) != std::get<size-1-I>(ends_)
             && std::next(std::get<size-1-I>(is_)) != std::get<size-1-I>(ends_))
              ? ++std::get<size-1-I>(is_), res = size-1-I
              : 0
            ...
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
        using swallow = int[];
        (void) swallow
          {
            (n < I
             && ((std::get<I>(is_) = std::get<I>(begins_)), true))...
          };
      }

      template <std::size_t... I>
      bool not_equal_(const iterator& that, seq<I...>) const
      {
        for (auto n: {(std::get<I>(is_) != std::get<I>(that.is_))...})
          if (n)
            return true;
        return false;
      }

      /// Tuple of values.
      template <std::size_t... I>
      value_type dereference_(seq<I...>) const
      {
        return value_type{(*std::get<I>(is_))...};
      }
    };

    iterator begin() const
    {
      auto res = begin_(indices_t{});
      return res;
    }

    iterator end() const
    {
      return end_(indices_t{});
    }

  private:
    template <std::size_t... I>
    iterator begin_(seq<I...>) const
    {
      return iterator(std::get<I>(sequences_).begin()...,
                      std::get<I>(sequences_).end()...);
    }

    template <std::size_t... I>
    iterator end_(seq<I...>) const
    {
      return iterator(std::get<I>(sequences_).end()...,
                      std::get<I>(sequences_).end()...);
    }

    sequences_t sequences_;
  };

  template <typename... Sequences>
  cross_sequences<Sequences...>
  cross(Sequences&&... seqs)
  {
    return {std::forward<Sequences>(seqs)...};
  }

  template <typename... Sequences>
  cross_sequences<Sequences...>
  cross_tuple(const std::tuple<Sequences...>& seqs)
  {
    return {seqs};
  }
}

#endif // !VCSN_MISC_CROSS_HH
