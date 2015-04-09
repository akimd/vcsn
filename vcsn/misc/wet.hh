#pragma once

#include <cassert>
#include <map>
#include <set>
#include <type_traits>
#include <unordered_map>

#include <boost/iterator/iterator_facade.hpp>
#include <boost/dynamic_bitset.hpp>

#include <vcsn/misc/empty.hh>
#include <vcsn/misc/functional.hh> // vcsn::equal_to

namespace vcsn
{
  namespace detail
  {

    /*------------------.
    | welement_label.   |
    `------------------*/

    template <typename Label>
    struct welement_label
    {
      using label_t = Label;
      welement_label(const label_t& l)
        : label_(l)
      {}

      const label_t& label() const { return label_; }
      void label(const label_t& l) { label_ = l; }

    private:
      label_t label_;
    };

    template <>
    struct welement_label<empty_t>
    {
      using label_t = empty_t;
      welement_label(const label_t&)
      {}
      label_t label() const { return {}; }
      void label(label_t) {}
    };


    /*-------------------.
    | welement_weight.   |
    `-------------------*/

    template <typename Weight>
    struct welement_weight
    {
      using weight_t = Weight;
      welement_weight(const weight_t& w)
        : weight_(w)
      {}
      const weight_t& weight() const { return weight_; }
      void weight(const weight_t& k) { weight_ = k; }

    private:
      weight_t weight_;
    };

    template <>
    struct welement_weight<bool>
    {
      using weight_t = bool;
      welement_weight(const weight_t& w)
      {
        (void) w; assert(w);
      }
      weight_t weight() const { return true; }
      void weight(const weight_t& w) { (void) w; assert(w); }
    };
  }

  /*------------.
  | welement.   |
  `------------*/

  template <typename Label, typename Weight>
  struct welement
    : detail::welement_label<Label>
    , detail::welement_weight<Weight>
  {
    using welement_label_t = detail::welement_label<Label>;
    using welement_weight_t = detail::welement_weight<Weight>;

    using label_t = typename welement_label_t::label_t;
    using weight_t = typename welement_weight_t::weight_t;

    using value_t = welement<typename std::remove_reference<label_t>::type,
                             typename std::remove_reference<weight_t>::type>;

    welement(const label_t& l, const weight_t& w)
      : welement_label_t(l)
      , welement_weight_t(w)
    {}

    operator value_t() const
    {
      return {this->label(), this->weight()};
    }

    auto operator<(const welement& that) const
      -> bool
    {
      return this->label() < that.label();
    }

    auto operator==(const welement& that) const
      -> bool
    {
      return this->label() == that.label();
    }
  };

  template <typename Label, typename Weight>
  auto label_of(const welement<Label, Weight>& m)
    -> decltype(m.label())
  {
    return m.label();
  }

  template <typename Label, typename Weight>
  auto weight_of(const welement<Label, Weight>& m)
    -> decltype(m.weight())
  {
    return m.weight();
  }

  template <typename Label, typename Weight>
  void weight_set(welement<Label, Weight>& m, const Weight& w)
  {
    m.weight(w);
  }

  template <typename Label, typename Weight>
  const Label& label_of(const std::pair<Label, Weight>& m)
  {
    return m.first;
  }

  template <typename Label, typename Weight>
  const Weight& weight_of(const std::pair<Label, Weight>& m)
  {
    return m.second;
  }

  template <typename Label, typename Weight>
  Label& label_of(std::pair<Label, Weight>& m)
  {
    return m.first;
  }

  template <typename Label, typename Weight>
  void weight_set(std::pair<Label, Weight>& m, const Weight& w)
  {
    m.second = w;
  }

  enum wet_kind
    {
      map,
      set,
      bitset
    };


  namespace detail
  {
#if 1
    /*-------------------------.
    | wset_impl<Key, Value>.   |
    `-------------------------*/

    /// General case.
    template <typename Key, typename Value,
              typename Compare = std::less<Key>,
              typename Hash = std::hash<Key>,
              typename KeyEqual = std::equal_to<Key>>
    class wet_impl
    {
    private:
      using map_t = std::map<Key, Value, Compare>;
      map_t map_;

    public:
      static constexpr wet_kind kind = wet_kind::map;
      using self_t = wet_impl;
      using key_t = Key;
      using value_t = Value;
      using welement_t = welement<key_t, value_t>;
      using value_type = typename map_t::value_type;
      using iterator = typename map_t::iterator;
      using const_iterator = typename map_t::const_iterator;

      wet_impl() = default;

      wet_impl(const std::initializer_list<value_type>& l)
        : map_{l}
      {}

      static const key_t& key_of(const value_type& p)
      {
        return p.first;
      }

      static const value_t& value_of(const value_type& p)
      {
        return p.second;
      }

      void set(const key_t& k, const value_t& v)
      {
        map_[k] = v;
      }

      void set(const iterator& i, const value_t& v)
      {
        i->second = v;
      }

      template <typename Fun>
      void for_each(Fun f) const
      {
        std::for_each(begin(map_), end(map_),
                      f);
      }

#define DEFINE(Name, Const)                                     \
      template <typename... Args>                               \
      auto                                                      \
      Name(Args&&... args) Const                                \
        -> decltype(map_.Name(std::forward<Args>(args)...))     \
      {                                                         \
        return map_.Name(std::forward<Args>(args)...);          \
      }

      DEFINE(emplace,);
      DEFINE(clear,);
      DEFINE(begin,const);
      DEFINE(find,const);
      DEFINE(find,);
      DEFINE(erase,);
      DEFINE(end,  const);
      DEFINE(begin,);
      DEFINE(end,);
      DEFINE(empty,const);
      DEFINE(size, const);
#undef DEFINE
    };

#else

    /*-------------------------.
    | wset_impl<Key, Value>.   |
    `-------------------------*/

    /// General case.
    template <typename Key, typename Value,
              typename Compare = std::less<Key>,
              typename Hash = std::hash<Key>,
              typename KeyEqual = std::equal_to<Key>>
    class wet_impl
    {
    private:
      using map_t = std::unordered_map<Key, Value, Hash, KeyEqual>;
      map_t map_;

    public:
      static constexpr wet_kind kind = wet_kind::map;
      using self_t = wet_impl;
      using key_t = Key;
      using value_t = Value;
      using welement_t = welement<key_t, value_t>;
      using value_type = typename map_t::value_type;
      using iterator = typename map_t::iterator;
      using const_iterator = typename map_t::const_iterator;

      wet_impl() = default;

      wet_impl(const std::initializer_list<value_type>& l)
        : map_{l}
      {}

      static const key_t& key_of(const value_type& p)
      {
        return p.first;
      }

      static const value_t& value_of(const value_type& p)
      {
        return p.second;
      }

      void set(const key_t& k, const value_t& v)
      {
        map_[k] = v;
      }

      void set(const iterator& i, const value_t& v)
      {
        i->second = v;
      }

      template <typename Fun>
      void for_each(Fun f) const
      {
        std::for_each(begin(map_), end(map_),
                      f);
      }

#define DEFINE(Name, Const)                                     \
      template <typename... Args>                               \
      auto                                                      \
      Name(Args&&... args) Const                                \
        -> decltype(map_.Name(std::forward<Args>(args)...))     \
      {                                                         \
        return map_.Name(std::forward<Args>(args)...);          \
      }

      DEFINE(emplace,);
      DEFINE(clear,);
      DEFINE(begin,const);
      DEFINE(find,const);
      DEFINE(find,);
      DEFINE(erase,);
      DEFINE(end,  const);
      DEFINE(begin,);
      DEFINE(end,);
      DEFINE(empty,const);
      DEFINE(size, const);
#undef DEFINE
    };

#endif

    /*-----------------------.
    | wet_impl<Key, bool>.   |
    `-----------------------*/


    template <typename Key, typename Compare, typename Hash, typename KeyEqual>
    class wet_impl<Key, bool, Compare, Hash, KeyEqual>
    {
    private:
      using set_t = std::set<Key, Compare>;
      set_t set_;

    public:
      static constexpr wet_kind kind = wet_kind::set;
      using self_t = wet_impl;
      using key_t = Key;
      using value_t = bool;
      using welement_t = welement<key_t, value_t>;
      using value_type = welement_t;

      wet_impl() = default;
      wet_impl(const std::initializer_list<value_type>& p)
      {
        for (const auto& m: p)
          set(label_of(m), weight_of(m));
      }

      /// Iterator.
      template <typename Value, typename Iterator>
      struct iterator_impl
        : public boost::iterator_facade<iterator_impl<Value, Iterator>
                                        , Value
                                        , boost::forward_traversal_tag
                                        >
      {
        /// Underlying iterator.
        using iterator_t = Iterator;
        using value_t = Value;
        using reference_t = value_t&;

        template <typename OtherValue, typename OtherIterator>
        iterator_impl(const iterator_impl<OtherValue, OtherIterator>& that)
          : it_(that.iterator())
        {}

        iterator_impl(const iterator_t& it)
          : it_(it)
        {}

        const iterator_t& iterator() const
        {
          return it_;
        }

        welement_t operator*() const
        {
          return {*it_, true};
        }

      private:
        friend class boost::iterator_core_access;

        /// Advance to next position.
        void increment()
        {
          ++it_;
        }

        void decrement()
        {
          --it_;
        }

        void advance(int n)
        {
          it_ += n;
        }

        template <typename OtherValue, typename OtherIterator>
        bool equal(const iterator_impl<OtherValue, OtherIterator>& that) const
        {
          return iterator() == that.iterator();
        }

        iterator_t it_;
      };

      /// Mutable iterator.
      using iterator = iterator_impl<value_type, typename set_t::iterator>;

      /// Const iterator.
      using const_iterator
        = iterator_impl<const value_type, typename set_t::const_iterator>;

      auto begin()        -> iterator       { return {set_.begin()}; }
      auto end()          -> iterator       { return {set_.end()}; }
      auto cbegin() const -> const_iterator { return {set_.cbegin()}; }
      auto cend()   const -> const_iterator { return {set_.cend()}; }
      auto begin()  const -> const_iterator { return cbegin(); }
      auto end()    const -> const_iterator { return cend(); }

      void set(const key_t& k, const value_t& v)
      {
        assert(v); (void) v;
        set_.emplace(k);
      }

      // When called, the key is already defined.
      void set(const iterator&, const value_t& v)
      {
        assert(v); (void) v;
      }

      auto erase(const_iterator pos)
        -> iterator
      {
        return set_.erase(pos.iterator());
      }

      template <typename... Args>
      auto find(Args&&... args) const
        -> const_iterator
      {
        return set_.find(std::forward<Args>(args)...);
      }

      template <typename... Args>
      auto find(Args&&... args)
        -> iterator
      {
        return set_.find(std::forward<Args>(args)...);
      }


#define DEFINE(Name, Const)                                     \
      template <typename... Args>                               \
      auto                                                      \
      Name(Args&&... args) Const                                \
        -> decltype(set_.Name(std::forward<Args>(args)...))     \
      {                                                         \
        return set_.Name(std::forward<Args>(args)...);          \
      }

      DEFINE(clear,);
      DEFINE(erase,);
      DEFINE(empty,const);
      DEFINE(size, const);
#undef DEFINE
    };


    /*---------------------------------.
    | wet_impl<char, bool>: bitsets.   |
    `---------------------------------*/

    template <typename Compare, typename Hash, typename KeyEqual>
    class wet_impl<char, bool, Compare, Hash, KeyEqual>
    {
    private:
      using set_t = boost::dynamic_bitset<>;
      set_t set_{256};

    public:
      static constexpr wet_kind kind = wet_kind::bitset;
      using self_t = wet_impl;
      using key_t = char;
      using value_t = bool;
      using welement_t = welement<key_t, value_t>;
      using value_type = welement_t;

      // A plain "wet_impl() = default;" fails with GCC 4.8 or 4.9.0:
      // error: converting to 'const boost::dynamic_bitset<>'
      // from initializer list would use explicit constructor
      // 'boost::dynamic_bitset<Block, Allocator>::dynamic_bitset([...])
      //
      //        wet_impl() = default;
      //        ^
      wet_impl() {}

      wet_impl(const std::initializer_list<value_type>& p)
      {
        for (const auto& m: p)
          set(label_of(m), weight_of(m));
      }

      wet_impl(set_t&& set)
        : set_{std::move(set)}
      {}

      static constexpr size_t npos = set_t::npos;

      /// Iterator.
      template <typename Value, typename Set>
      struct iterator_impl
        : public boost::iterator_facade<iterator_impl<Value, Set>
                                        , Value
                                        , boost::forward_traversal_tag
                                        >
      {
        /// Underlying "iterator".
        using set_t = Set;
        using iterator_t = size_t;
        using value_t = Value;
        using reference_t = value_t&;

        template <typename OtherValue, typename OtherSet>
        iterator_impl(const iterator_impl<OtherValue, OtherSet>& that)
          : set_(that.set())
          , it_(that.iterator())
        {}

        iterator_impl(set_t& set, size_t pos)
          : set_(set)
          , it_(pos)
        {}

        iterator_impl(set_t& set)
          : set_(set)
          , it_(set.find_first())
        {}

        iterator_impl& operator=(const iterator_impl& that)
        {
          assert(&set_ == &that.set_);
          it_ = that.it_;
          return *this;
        }

        const set_t& set() const
        {
          return set_;
        }

        const iterator_t& iterator() const
        {
          return it_;
        }

        welement_t operator*() const
        {
          return {key_t(it_), true};
        }

      private:
        friend class boost::iterator_core_access;

        /// Advance to next position.
        void increment()
        {
          it_ = set_.find_next(it_);
        }

        template <typename OtherValue, typename OtherSet>
        bool equal(const iterator_impl<OtherValue, OtherSet>& that) const
        {
          return (iterator() == that.iterator()
                  && set() == that.set());
        }

        /// Underlying bitset.
        set_t& set_;
        iterator_t it_;
      };

      /// Mutable iterator.
      using iterator = iterator_impl<value_type, set_t>;

      /// Const iterator.
      using const_iterator = iterator_impl<const value_type, const set_t>;

      auto begin()        -> iterator       { return {set_}; }
      auto end()          -> iterator       { return {set_, npos}; }

      auto cbegin() const -> const_iterator { return {set_}; }
      auto cend()   const -> const_iterator { return {set_, npos}; }

      auto begin()  const -> const_iterator { return cbegin(); }
      auto end()    const -> const_iterator { return cend(); }

      void set(const key_t& k, const value_t& v)
      {
        assert(v); (void) v;
        set_.set((unsigned char)k);
      }

      // When called, the key is already defined.
      void set(const iterator&, const value_t& v)
      {
        assert(v); (void) v;
      }

      auto erase(const_iterator pos)
        -> void
      {
        set_.reset(pos.iterator());
      }

      void erase(const key_t& k)
      {
        set_.reset((unsigned) k);
      }

      auto find(const key_t& k) const
        -> const_iterator
      {
        if (set_[(unsigned char)k])
          return {set_, (unsigned char)k};
        else
          return end();
      }

      auto find(const key_t& k)
        -> iterator
      {
        if (set_[(unsigned char)k])
          return {set_, (unsigned char)k};
        else
          return end();
      }

      size_t size() const
      {
        return set_.count();
      }

      size_t empty() const
      {
        return set_.none();
      }
    };
  }

  template <typename Key, typename Value,
            typename Compare = std::less<Key>,
            typename Hash = std::hash<Key>,
            typename KeyEqual = std::equal_to<Key>>
  using wet = detail::wet_impl<Key, Value, Compare, Hash, KeyEqual>;

  /// The corresponding wet for a LabelSet -> WeightSet context.
  template <typename Context>
  using wet_of = wet<label_t_of<Context>,
                     weight_t_of<Context>,
                     vcsn::less<labelset_t_of<Context>>,
                     vcsn::hash<labelset_t_of<Context>>,
                     vcsn::equal_to<labelset_t_of<Context>>>;
}
