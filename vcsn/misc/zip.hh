#pragma once

#include <boost/iterator/iterator_facade.hpp>

#include <vcsn/misc/raise.hh> // pass
#include <vcsn/misc/tuple.hh>
#include <vcsn/misc/type_traits.hh> // std::enable_if_t

namespace vcsn
{

  template <typename... Sequences>
  struct zip_sequences
  {
    /// Type of the tuple of all the maps.
    using sequences_t = std::tuple<Sequences...>;

    /// Type of index sequences.
    template <std::size_t... I>
    using seq = std::index_sequence<I...>;

    /// Number of sequences.
    static constexpr size_t size = sizeof...(Sequences);

    /// Index sequence for our sequences.
    using indices_t = std::make_index_sequence<sizeof...(Sequences)>;

    /// The type of the underlying sequences, without reference.
    template <typename Seq>
    using seq_t = typename std::remove_reference<Seq>::type;

    /// The type of the members.
    using value_type
      = std::tuple<typename seq_t<Sequences>::value_type...>;

    zip_sequences(const sequences_t& sequences)
      : sequences_(sequences)
    {}

    zip_sequences(Sequences... sequences)
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
    struct zip_iterator
      : public boost::iterator_facade<
           zip_iterator<ValueType, IteratorsType>
         , ValueType
         , boost::forward_traversal_tag
        >
    {
      /// Underlying iterators.
      using iterators_type = IteratorsType;

      zip_iterator(const iterators_type& is, const iterators_type& ends)
        : is_{is}
        , ends_{ends}
      {}

      template <typename OtherValue, typename OtherIterators>
      zip_iterator(zip_iterator<OtherValue, OtherIterators> const& that)
        : is_{that.is_}
        , ends_{that.ends_}
      {}

      /// The current position.
      iterators_type is_;
      /// The ends.
      iterators_type ends_;

      /// Advance to next position.
      zip_iterator& operator++()
      {
        if (!next_())
          done_();
        return *this;
      }

      void increment()
      {
        ++(*this);
      }

      void advance(std::size_t n)
      {
        for (size_t i = 0; i < n; ++i)
          increment();
      }

      value_type operator*() const
      {
        return dereference_(indices_t{});
      }

      const iterators_type& iterators() const
      {
        return is_;
      }

      const iterators_type& end() const
      {
        return ends_;
      }

    protected:
      friend class boost::iterator_core_access;

      /// We have reached the end, move all the cursors to this end.
      void done_()
      {
        is_ = ends_;
      }

      /// Move to the next position.  Return the index of the lastest
      /// iterator that could move, -1 if we reached the end.
      bool next_()
      {
        return next_(indices_t{});
      }

      template <std::size_t... I>
      bool next_(seq<I...>)
      {
        bool res = true;
        using swallow = int[];
        (void) swallow
          {
            res
            && (++std::get<I>(is_) == std::get<I>(ends_)
                ? res = false
                : true)
              ...
          };
        return res;
      }

      template <typename OtherValue, typename OtherIterators>
      bool equal(const zip_iterator<OtherValue, OtherIterators>& that) const
      {
        return equal_(that, indices_t{});
      }

      template <typename OtherValue, typename OtherIterators,
                std::size_t... I>
      bool equal_(const zip_iterator<OtherValue, OtherIterators>& that,
                  seq<I...>) const
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
    using iterator = zip_iterator<value_type, iterators_t>;

    /// Const iterator.
    using const_iterator = zip_iterator<const value_type, const_iterators_t>;

    const_iterator cbegin() const
    {
      return cbegin_(indices_t{});
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
      return begin_(indices_t{});
    }

    iterator end()
    {
      return end_(indices_t{});
    }

  private:
    template <std::size_t... I>
    const_iterator cbegin_(seq<I...>) const
    {
      return {const_iterators_t{std::get<I>(sequences_).cbegin()...},
              const_iterators_t{std::get<I>(sequences_).cend()...}};
    }

    template <std::size_t... I>
    const_iterator cend_(seq<I...>) const
    {
      return {const_iterators_t{std::get<I>(sequences_).cend()...},
              const_iterators_t{std::get<I>(sequences_).cend()...}};
    }


    template <std::size_t... I>
    iterator begin_(seq<I...>)
    {
      return {iterators_t{std::get<I>(sequences_).begin()...},
              iterators_t{std::get<I>(sequences_).end()...}};
    }

    template <std::size_t... I>
    iterator end_(seq<I...>)
    {
      return {iterators_t{std::get<I>(sequences_).end()...},
              iterators_t{std::get<I>(sequences_).end()...}};
    }

    /// The sequences we iterate upon.
    sequences_t sequences_;
  };



  /*------------------------.
  | zip_sequences_padded.   |
  `------------------------*/

  template <typename ZipSequences>
  struct zip_sequences_padded
  {
    /// Type of the wrapped zip sequence
    using zip_sequences_t = ZipSequences;

    /// Type of the tuples of all the maps
    using sequences_t = typename zip_sequences_t::sequences_t;

    /// Type of index sequences.
    template <std::size_t... I>
    using seq = std::index_sequence<I...>;

    /// Index sequence for our sequences.
    using indices_t = typename zip_sequences_t::indices_t;

    /// The type of the members.
    using value_type = typename zip_sequences_t::value_type;

    /// Tuple of const_iterators.
    using const_iterators_t = typename zip_sequences_t::const_iterators_t;

    /// Tuple of iterators.
    using iterators_t = typename zip_sequences_t::iterators_t;

    using padding_t = value_type;

    template <typename... S>
    zip_sequences_padded(const value_type& pad,
                         const std::tuple<S...>& sequences)
      : sequences_{sequences}, paddings_{pad}
    {}

    template <typename... S>
    zip_sequences_padded(const value_type& pad, S... sequences)
      : sequences_(sequences...), paddings_{pad}
    {}

    template <typename ValueType, typename IteratorsType>
    struct zip_iterator
      : public zip_sequences_t::template zip_iterator<ValueType, IteratorsType>
    {

      using super_t =
        typename zip_sequences_t::template zip_iterator<ValueType,
                                                        IteratorsType>;

      /// Underlying iterators.
      using iterators_type = IteratorsType;

      zip_iterator(const iterators_type& is, const iterators_type& ends,
                   const padding_t& pad)
        : super_t(is, ends)
        , pad_(pad)
      {}

      template <typename OtherValue, typename OtherIterators>
      zip_iterator(zip_iterator<OtherValue, OtherIterators> const& that,
                   const padding_t& pad)
        : super_t(that.is_, that.ends_)
        , pad_(pad)
      {}
      value_type operator*() const
      {
        return dereference_(indices_t{});
      }

      /// Advance to next position.
      zip_iterator& operator++()
      {
        if (!next_())
          this->done_();
        return *this;
      }


    private:

      /// Move to the next position.  Return the index of the lastest
      /// iterator that could move, -1 if we reached the end.
      bool next_()
      {
        return next_(indices_t{});
      }

      template <std::size_t... I>
      bool next_(seq<I...>)
      {
        bool res = false;
        using swallow = int[];
        (void) swallow
          {
            (std::get<I>(this->is_) == std::get<I>(this->ends_)
             ? false
             : ++std::get<I>(this->is_) == std::get<I>(this->ends_)
             ? false
             : res = true)
            ...
          };
        return res;
      }

      /// Tuple of values.
      template <std::size_t... I>
      value_type dereference_(seq<I...>) const
      {
        return value_type{(std::get<I>(this->is_) == std::get<I>(this->ends_)
                           ? std::get<I>(pad_)
                           : *std::get<I>(this->is_))...};
      }

      padding_t pad_;
    };

    /// Mutable iterator.
    using iterator = zip_iterator<value_type, iterators_t>;

    /// Const iterator.
    using const_iterator = zip_iterator<const value_type, const_iterators_t>;

    const_iterator cbegin() const
    {
      return cbegin_(indices_t{});
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
      return begin_(indices_t{});
    }

    iterator end()
    {
      return end_(indices_t{});
    }

  private:
    template <std::size_t... I>
    const_iterator cbegin_(seq<I...>) const
    {
      return {const_iterators_t{std::get<I>(sequences_).cbegin()...},
              const_iterators_t{std::get<I>(sequences_).cend()...},
              paddings_};
    }

    template <std::size_t... I>
    const_iterator cend_(seq<I...>) const
    {
      return {const_iterators_t{std::get<I>(sequences_).cend()...},
              const_iterators_t{std::get<I>(sequences_).cend()...},
              paddings_};
    }


    template <std::size_t... I>
    iterator begin_(seq<I...>)
    {
      return {iterators_t{std::get<I>(sequences_).begin()...},
              iterators_t{std::get<I>(sequences_).end()...},
              paddings_};
    }

    template <std::size_t... I>
    iterator end_(seq<I...>)
    {
      return {iterators_t{std::get<I>(sequences_).end()...},
              iterators_t{std::get<I>(sequences_).end()...},
              paddings_};
    }

  private:
    sequences_t sequences_;
    padding_t paddings_;
  };

  template <typename... Sequences>
  zip_sequences<Sequences...>
  zip(Sequences&&... seqs)
  {
    return {std::forward<Sequences>(seqs)...};
  }

  template <typename... Sequences>
  zip_sequences<Sequences...>
  zip_tuple(const std::tuple<Sequences...>& seqs)
  {
    return {seqs};
  }

  template <typename... Sequences>
  zip_sequences_padded<zip_sequences<Sequences...>>
  zip_with_padding(const std::tuple<typename Sequences::value_type...>& pad,
                   const Sequences&... seq)
  {
    return {pad, std::make_tuple(seq...)};
  }
}
