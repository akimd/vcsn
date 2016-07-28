#pragma once

#include <memory>

#include <boost/iterator/iterator_facade.hpp>

#include <vcsn/misc/tuple.hh>

namespace vcsn
{
  /// Provide a range that allows to iterate over the cross product of
  /// the provided ranges.
  ///
  /// E.g., {a,b,c} x {1,2} -> {(a,1), (a,2), (b,1), (b,2), (c,1), (c,2)}.
  template <typename... Sequences>
  struct cross_sequences
  {
    /// Type of the tuple of all the maps.
    using sequences_t = std::tuple<Sequences...>;

    /// Type of index sequences.
    template <std::size_t... I>
    using seq = vcsn::detail::index_sequence<I...>;

    /// Number of sequences.
    static constexpr size_t size = sizeof...(Sequences);

    /// Index sequence for our sequences.
    using indices_t = vcsn::detail::make_index_sequence<sizeof...(Sequences)>;

    /// The type of the underlying sequences, without reference.
    template <typename Seq>
    using seq_t = std::remove_reference_t<Seq>;

    /// The type of the members.
    using value_type
      = std::tuple<typename seq_t<Sequences>::value_type...>;

    cross_sequences(sequences_t sequences)
      : sequences_(std::move(sequences))
    {}

    cross_sequences(Sequences... sequences)
      : sequences_(sequences...)
    {}

    /// Tuple of const_iterators.
    using const_iterators_t
      = std::tuple<typename seq_t<Sequences>::const_iterator...>;

    /// Tuple of iterators.
    using iterators_t
      = std::tuple<typename seq_t<Sequences>::iterator...>;

    /// Composite iterator.
    template <typename ValueType,
              typename IteratorsType>
    struct cross_iterator
      : public boost::iterator_facade<
           cross_iterator<ValueType, IteratorsType>
         , ValueType
         , boost::forward_traversal_tag
         , ValueType // Reference.
        >
    {
      /// Underlying iterators.
      using iterators_type = IteratorsType;

      cross_iterator(const iterators_type& is,
                     const iterators_type& begins, const iterators_type& ends)
        : is_{is}
        , begins_{begins}
        , ends_{ends}
      {
        // If one of the ranges is empty, we are done already.
        if (empty())
          done_();
      }

      cross_iterator(const iterators_type& is, const iterators_type& ends)
        : cross_iterator{is, is, ends}
      {}

      template <typename OtherValue, typename OtherIterators>
      cross_iterator(cross_iterator<OtherValue, OtherIterators> const& that)
        : cross_iterator{that.is_, that.begins_, that.ends_}
      {}

      /// The current position.
      iterators_type is_;
      /// The begins.
      iterators_type begins_;
      /// The ends.
      iterators_type ends_;

      /// Whether some of the range is empty.
      bool empty() const
      {
        return empty_(indices_t{});
      }

      /// Advance to next position.
      void increment()
      {
        if (increment_() == -1)
          done_();
      }

      bool equal(const cross_iterator& that) const
      {
        return equal_(that, indices_t{});
      }

      value_type dereference() const
      {
        return dereference_(indices_t{});
      }

    private:
      friend class boost::iterator_core_access;

      template <std::size_t... I>
      bool empty_(seq<I...>) const
      {
        for (auto n: {(std::get<I>(begins_) == std::get<I>(ends_))...})
          if (n)
            return true;
        return false;
      }

      /// We have reached the end, move all the cursors to this end.
      void done_()
      {
        is_ = ends_;
      }

      /// Move to the next position.  Return the index of the lastest
      /// iterator that could move, -1 if we reached the end.
      int increment_()
      {
        auto res = increment_(indices_t{});
        // Reset all the iterators that are before the first one that could
        // advance.
        if (res != -1)
          reset_up_to_(res);
        return res;
      }

      template <std::size_t... I>
      int increment_(seq<I...>)
      {
        int res = -1;
        using swallow = int[];
        (void) swallow
          {
            (res == -1
             && std::get<size-1-I>(is_) != std::get<size-1-I>(ends_)
             && ++std::get<size-1-I>(is_) != std::get<size-1-I>(ends_))
              ? res = size-1-I
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
      bool equal_(const cross_iterator& that, seq<I...>) const
      {
        for (auto n: {(std::get<I>(is_) == std::get<I>(that.is_))...})
          if (!n)
            return false;
        return true;
      }

      /// Tuple of values.
      template <std::size_t... I>
      value_type dereference_(seq<I...>) const
      {
        return value_type{(*std::get<I>(is_))...};
      }
    };

    /// Mutable iterator.
    using iterator = cross_iterator<value_type, iterators_t>;

    /// Const iterator.
    using const_iterator = cross_iterator<const value_type, const_iterators_t>;

    const_iterator cbegin() const
    {
      auto res = cbegin_(indices_t{});
      return res;
    }

    const_iterator cend() const
    {
      return cend_(indices_t{});
    }

    const_iterator begin() const
    {
      return cbegin();
    }

    const_iterator end() const
    {
      return cend();
    }

    iterator begin()
    {
      auto res = begin_(indices_t{});
      return res;
    }

    iterator end()
    {
      return end_(indices_t{});
    }

    /// Whether the sequence is empty.
    bool empty() const
    {
      return cbegin() == cend();
    }


  private:
    template <std::size_t... I>
    const_iterator cbegin_(seq<I...>) const
    {
      // FIXME: clang-3.5: std::cbegin/cend are not supported, yet.
      using std::begin;
      using std::end;
      return {const_iterators_t{begin(std::get<I>(sequences_))...},
              const_iterators_t{end(std::get<I>(sequences_))...}};
    }

    template <std::size_t... I>
    const_iterator cend_(seq<I...>) const
    {
      using std::end;
      return {const_iterators_t{end(std::get<I>(sequences_))...},
              const_iterators_t{end(std::get<I>(sequences_))...}};
    }

    template <std::size_t... I>
    iterator begin_(seq<I...>)
    {
      using std::begin;
      using std::end;
      return {iterators_t{begin(std::get<I>(sequences_))...},
              iterators_t{end(std::get<I>(sequences_))...}};
    }

    template <std::size_t... I>
    iterator end_(seq<I...>)
    {
      using std::end;
      return {iterators_t{end(std::get<I>(sequences_))...},
              iterators_t{end(std::get<I>(sequences_))...}};
    }

    /// The sequences we iterate upon.
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
