#pragma once

#include <cassert>
#include <map>
#include <set>
#include <type_traits>
#include <unordered_map>

#include <boost/iterator/iterator_facade.hpp>

#include <vcsn/ctx/traits.hh> // labelset_t_of
#include <vcsn/misc/dynamic_bitset.hh>
#include <vcsn/misc/empty.hh>
#include <vcsn/misc/functional.hh> // vcsn::equal_to
#include <vcsn/misc/type_traits.hh>

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

  enum class wet_kind_t
    {
      bitset,
      map,
      set,
      unordered_map,
    };


  namespace detail
  {

    /*-----------------------.
    | wet_map<Key, Value>.   |
    `-----------------------*/

    /// General case.
    template <typename Key, typename Value,
              typename Compare>
    class wet_map
    {
    private:
      using map_t = std::map<Key, Value, Compare>;
      map_t map_;

    public:
      using self_t = wet_map;
      static constexpr wet_kind_t kind = wet_kind_t::map;
      using key_t = Key;
      using value_t = Value;
      using welement_t = welement<key_t, value_t>;
      using mapped_type = typename map_t::mapped_type;
      using value_type = typename map_t::value_type;
      using iterator = typename map_t::iterator;
      using const_iterator = typename map_t::const_iterator;

      wet_map() = default;

      wet_map(const std::initializer_list<value_type>& l)
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

      DEFINE(key_comp, const);
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


    /*---------------------------------.
    | wet_unordered_map<Key, Value>.   |
    `---------------------------------*/

    /// General case.
    template <typename Key, typename Value,
              typename Hash, typename KeyEqual>
    class wet_unordered_map
    {
    private:
      using map_t = std::unordered_map<Key, Value, Hash, KeyEqual>;
      map_t map_;

    public:
      using self_t = wet_unordered_map;
      static constexpr wet_kind_t kind = wet_kind_t::unordered_map;
      using key_t = Key;
      using value_t = Value;
      using welement_t = welement<key_t, value_t>;
      using value_type = typename map_t::value_type;
      using iterator = typename map_t::iterator;
      using const_iterator = typename map_t::const_iterator;

      wet_unordered_map() = default;

      wet_unordered_map(const std::initializer_list<value_type>& l)
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


    /*----------------------.
    | wet_set<Key, bool>.   |
    `----------------------*/


    template <typename Key, typename Compare>
    class wet_set
    {
    private:
      using set_t = std::set<Key, Compare>;
      set_t set_;

    public:
      using self_t = wet_set;
      static constexpr wet_kind_t kind = wet_kind_t::set;
      using key_t = Key;
      using value_t = bool;
      using welement_t = welement<key_t, value_t>;
      using value_type = welement_t;

      wet_set() = default;
      wet_set(const std::initializer_list<value_type>& p)
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


    /*---------------------------------------.
    | wet_bitset<unsigned, bool>: bitsets.   |
    `---------------------------------------*/

    class wet_bitset
    {
    private:
      using set_t = boost::dynamic_bitset<>;
      set_t set_;

    public:
      using self_t = wet_bitset;
      static constexpr wet_kind_t kind = wet_kind_t::bitset;
      using key_t = unsigned;
      using value_t = bool;
      using welement_t = welement<key_t, value_t>;
      using value_type = welement_t;

      // A plain "wet_bitset() = default;" fails with GCC 4.8 or 4.9.0:
      // error: converting to 'const boost::dynamic_bitset<>'
      // from initializer list would use explicit constructor
      // 'boost::dynamic_bitset<Block, Allocator>::dynamic_bitset([...])
      //
      //        wet_bitset() = default;
      //        ^
      wet_bitset() {}

      wet_bitset(size_t size)
        : set_{size}
      {}

      wet_bitset(const std::initializer_list<value_type>& p)
      {
        for (const auto& m: p)
          set(label_of(m), weight_of(m));
      }

      wet_bitset(set_t&& set)
        : set_{std::move(set)}
      {}

      const set_t& set() const
      {
        return set_;
      }

      /// FIXME: Avoid this by exposing more interfaces.
      set_t& set()
      {
        return set_;
      }

      void clear()
      {
        set_.reset();
      }

      bool empty() const
      {
        return set_.none();
      }

      size_t size() const
      {
        return set_.count();
      }

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

      /// Special for char: map -1 to 255 (MAX_UCHAR), not MAX_INT.
      static key_t key(char k)
      {
        return static_cast<unsigned char>(k);
      }

      void set(const key_t k, value_t v = true)
      {
        assert(v); (void) v;
        set_.set(k);
      }

      void set(const char k, value_t v = true)
      {
        set(key(k), v);
      }

      // When called, the key is already defined.
      void set(const iterator&, const value_t v = true)
      {
        assert(v); (void) v;
      }

      auto erase(const_iterator pos)
        -> void
      {
        set_.reset(pos.iterator());
      }

      void erase(key_t k)
      {
        set_.reset(k);
      }

      void erase(char k)
      {
        erase(key(k));
      }

      auto find(key_t k) const
        -> const_iterator
      {
        if (set_[k])
          return {set_, k};
        else
          return end();
      }

      auto find(key_t k)
        -> iterator
      {
        if (set_[k])
          return {set_, k};
        else
          return end();
      }

      auto find(char k) const
        -> const_iterator
      {
        return find(key(k));
      }

      auto find(char k)
        -> iterator
      {
        return find(key(k));
      }

      /// Operators for when wet_bitset is used as a bitset.
      self_t operator-(const self_t& rhs) const
      {
        return {set_ - rhs.set_};
      }

      self_t operator&(const self_t& rhs) const
      {
        return {set_ & rhs.set_};
      }

      /// Allow wet_bitset to be used in containers.
      friend bool operator<(const self_t& lhs, const self_t& rhs)
      {
        return lhs.set_ < rhs.set_;
      }
    };

    /*------------------------.
    | wet_kind<Key, Value>.   |
    `------------------------*/

    // wet_impl<Key, Value>: map.
    template <typename Key, typename Value>
    struct wet_kind_impl
    {
      static constexpr wet_kind_t kind = wet_kind_t::map;
    };

    // wet_impl<Key, bool>: set.
    template <typename Key>
    struct wet_kind_impl<Key, bool>
    {
      static constexpr wet_kind_t kind = wet_kind_t::set;
    };

    // wet_impl<char, bool>: bitsets.
    template <>
    struct wet_kind_impl<char, bool>
    {
      static constexpr wet_kind_t kind = wet_kind_t::bitset;
    };

    template <typename Key, typename Value>
    constexpr wet_kind_t wet_kind()
    {
      return wet_kind_impl<Key, Value>::kind;
    }


    /*------------------------.
    | wet_impl<Key, Value>.   |
    `------------------------*/

    // wet_impl<Key, Value>: map.
    template <wet_kind_t Kind,
              typename Key, typename Value,
              typename Compare, typename Hash, typename KeyEqual>
    using wet_impl
      = std::conditional_t<Kind == wet_kind_t::bitset,
                           wet_bitset,
        std::conditional_t<Kind == wet_kind_t::set,
                           wet_set<Key, Compare>,
        std::conditional_t<Kind == wet_kind_t::map,
                           wet_map<Key, Value, Compare>,
        std::conditional_t<Kind == wet_kind_t::unordered_map,
                           wet_unordered_map<Key, Value, Hash, KeyEqual>,
        void> > > >;

  }

  template <typename Key, typename Value,
            wet_kind_t Kind = detail::wet_kind<Key, Value>(),
            typename Compare = std::less<Key>,
            typename Hash = std::hash<Key>,
            typename KeyEqual = std::equal_to<Key>>
  using wet = detail::wet_impl<Kind,
                               Key, Value, Compare, Hash, KeyEqual>;

  /// The corresponding wet for a LabelSet -> WeightSet context.
  template <typename Context,
            wet_kind_t Kind = detail::wet_kind<labelset_t_of<Context>,
                                               weightset_t_of<Context>>()>
  using wet_of = wet<label_t_of<Context>,
                     weight_t_of<Context>,
                     Kind,
                     vcsn::less<labelset_t_of<Context>>,
                     vcsn::hash<labelset_t_of<Context>>,
                     vcsn::equal_to<labelset_t_of<Context>>>;
}
