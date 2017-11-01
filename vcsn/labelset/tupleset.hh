#pragma once

#include <iosfwd>
#include <istream>
#include <set>
#include <tuple>

#include <boost/optional.hpp>
#include <boost/range/join.hpp>

#include <vcsn/config.hh> // VCSN_HAVE_CORRECT_LIST_INITIALIZER_ORDER
#include <vcsn/ctx/traits.hh>
//#include <vcsn/core/rat/expressionset.hh> needed, but breaks everythying...
#include <vcsn/labelset/fwd.hh>
#include <vcsn/labelset/labelset.hh>
#include <vcsn/misc/cross.hh>
#include <vcsn/misc/escape.hh>
#include <vcsn/misc/filter.hh>
#include <vcsn/misc/raise.hh>
#include <vcsn/misc/static-if.hh>
#include <vcsn/misc/stream.hh>
#include <vcsn/misc/tuple.hh> // tuple_element_t
#include <vcsn/misc/zip.hh>
#include <vcsn/weightset/b.hh>

namespace vcsn
{
  namespace detail
  {

  /// A traits so that tupleset may define types that may exist.
  ///
  /// The types genset_t, letter_t, and word_t, exists only for
  /// tuples of labelsets, characterized as featuring a word_t type.
  template <typename Enable = void, typename... ValueSets>
  struct labelset_types_impl
  {
    using genset_t = void;
    using genset_ptr = void;
    using letter_t = void;
    using letters_t = void;
    /// Same as value_t.
    using word_t = std::tuple<typename ValueSets::value_t...>;
    constexpr static bool is_labelset = false;
  };

  /// Specialization for tuples of labelsets.
  template <typename... ValueSets>
  struct labelset_types_impl<decltype(pass{std::declval<ValueSets>().genset()...},
                                      void()),
                             ValueSets...>
  {
    using genset_t
      = cross_sequences<decltype(std::declval<ValueSets>().generators())...>;
    using genset_ptr = std::tuple<typename ValueSets::genset_ptr...>;
    using letter_t = std::tuple<typename ValueSets::letter_t...>;
    using letters_t = std::set<letter_t,
                               vcsn::less<tupleset<ValueSets...>, letter_t>>;
    using word_t = std::tuple<typename ValueSets::word_t...>;
    constexpr static bool is_labelset = true;
  };

  template <typename... ValueSets>
  using labelset_types = labelset_types_impl<void, ValueSets...>;

  /// A ValueSet which is a Cartesian product of ValueSets.
  ///
  /// Exposes a LabelSet interface for products of LabelSets, and similarly
  /// for WeightSets.
  template <typename... ValueSets>
  class tupleset_impl
  {
  public:
    using valuesets_t = std::tuple<ValueSets...>;
    using indices_t = make_index_sequence<sizeof...(ValueSets)>;
    static constexpr indices_t indices{};
    template <std::size_t... I>
    using seq = index_sequence<I...>;

    /// The Ith valueset type.
    template <std::size_t I>
    using valueset_t = tuple_element_t<I, valuesets_t>;

  public:
    using self_t = tupleset<ValueSets...>;

    /// A tuple of values.
    using value_t = std::tuple<typename ValueSets::value_t...>;

    /// A tuple of base letters if meaningful, void otherwise.
    using letter_t = typename labelset_types<ValueSets...>::letter_t;
    /// A set of letters if meaningful, void otherwise.
    using letters_t = typename labelset_types<ValueSets...>::letters_t;
    /// A tuple of generators if meaningful, void otherwise.
    //    using genset_t = typename labelset_types<ValueSets...>::genset_t;
    using genset_ptr = typename labelset_types<ValueSets...>::genset_ptr;
    /// A tuple of words if meaningful, void otherwise.
    using word_t = typename labelset_types<ValueSets...>::word_t;

    /// Whether this models a labelset.
    constexpr static bool is_labelset
      = labelset_types<ValueSets...>::is_labelset;

    /// To be iterable.
    using value_type = letter_t;

    using kind_t = labels_are_tuples;

    tupleset_impl(valuesets_t vs)
      : sets_(std::move(vs))
    {}

    tupleset_impl(ValueSets... ls)
      : sets_(std::move(ls)...)
    {}

    ~tupleset_impl()
    {}

    static symbol sname()
    {
      static auto res = symbol{sname_(indices)};
      return res;
    }

    /// Number of tapes.
    static constexpr std::size_t size()
    {
      return sizeof...(ValueSets);
    }

    /// Get the max of the sizes of the tapes.
    template <typename Value>
    static size_t size(const Value& v)
    {
      return size_(v, indices);
    }

    static constexpr bool
    is_commutative()
    {
      return is_commutative_(indices);
    }

    static constexpr bool
    is_idempotent()
    {
      return is_idempotent_(indices);
    }

    /// Build from the description in \a is.
    static self_t make(std::istream& is)
    {
      // name: lat<law_char(abc), law_char(xyz)>
      kind_t::make(is);
      eat(is, '<');
      auto res = make_(is, indices);
      eat(is, '>');
      return res;
    }

    /// The componants valuesets, as a tuple.
    const valuesets_t& sets() const
    {
      return sets_;
    }

    /// The Ith component valueset.
    template <size_t I>
    const valueset_t<I>& set() const
    {
      return std::get<I>(sets());
    }

    /// The type of the I-th tape valueset.
    template <size_t I>
    using project_t = valueset_t<I>;

    /// The Ith component valueset.
    ///
    /// Alias for `set<I>`.
    template <size_t I>
    const valueset_t<I>& project() const
    {
      return set<I>();
    }

    /// The I-th component of the value.
    template <size_t I>
    auto project(const value_t& v) const
    {
      return std::get<I>(v);
    }

    /// Whether unknown letters should be added, or rejected.
    /// \param o   whether to accept
    /// \returns   the previous status.
    bool open(bool o) const
    {
      return this->open_(o, indices);
    }

    /// Construct a value.
    template <typename... Args>
    value_t value(const std::tuple<Args...>& args) const
    {
      return this->value_(args, indices);
    }

    /// Construct a value.
    template <typename... Args>
    value_t tuple(Args&&... args) const
    {
      return value_t{args...};
    }

    /// Compose, aka `join` in the world of databases.
    /// Beware that we don't even check that the common tapes match.
    template <typename... LS1, typename... LS2>
    auto compose(const tupleset<LS1...>& ls1,
                 const typename tupleset<LS1...>::value_t& l1,
                 const tupleset<LS2...>& ls2,
                 const typename tupleset<LS2...>::value_t& l2) const
      -> std::enable_if_t<are_labelsets_composable<tupleset<LS1...>,
                                                   tupleset<LS2...>>{},
                          value_t>
    {
      // Tape of the lhs on which we compose.
      constexpr auto out = tupleset<LS1...>::size() - 1;
      // Tape of the rhs on which we compose.
      constexpr auto in = 0;
      using indices1_t = punched_sequence<tupleset<LS1...>::size(), out>;
      using indices2_t = punched_sequence<tupleset<LS2...>::size(), in>;
      return compose_(ls1, l1, ls2, l2,
                      indices1_t{}, indices2_t{});
    }

    genset_ptr
    genset() const
    {
      return this->genset_(indices);
    }

    /// The pregenerators.  Meaningful for labelsets only.
    auto
    pregenerators() const
    {
      return this->pregenerators_(indices);
    }

    /// The generators.  Meaningful for labelsets only.
    auto
    generators() const
    {
      if (has_one())
        return pregenerators().skip_first();
      else
        return pregenerators();
    }

    /// Convert to a word.
    template <typename... Args>
    auto
    word(const std::tuple<Args...>& v) const
      -> word_t
    {
      return this->word_(v, indices);
    }

    /// Run a function per set, and return the tuple of results.
    template <typename Fun>
    auto
    map(Fun&& fun) const
    {
      return map_impl_(std::forward<Fun>(fun), indices);
    }

    /// Whether \a l equals \a r.
    static bool
    equal(const value_t& l, const value_t& r)
    {
      return equal_(l, r, indices);
    }

    /// Three-way comparison between \a l and \a r.
    template <typename LhsValue, typename RhsValue>
    static auto
    compare(const LhsValue& l, const RhsValue& r)
      -> int
    {
      return less(r, l) - less(l, r);
    }

    /// Whether \a l < \a r.
    template <typename LhsValue, typename RhsValue>
    static auto
    less(const LhsValue& l, const RhsValue& r)
      -> bool
    {
      auto sl = size(l);
      auto sr = size(r);
      if (sl < sr)
        return true;
      else if (sr < sl)
        return false;
      else
        return less_(l, r, indices);
    }

    static value_t
    special()
    {
      return special_(indices);
    }

    static bool
    is_special(const value_t& l)
    {
      return is_special_(l, indices);
    }

    bool
    is_zero(const value_t& l) const
    {
      return is_zero_(l, indices);
    }

    static constexpr bool
    has_lightening_weights()
    {
      return has_lightening_weights_(indices);
    }

    static constexpr bool
    has_one()
    {
      return has_one_(indices);
    }

    static constexpr bool
    is_expressionset()
    {
      return is_expressionset_(indices);
    }

    static constexpr bool
    is_letterized()
    {
      return is_letterized_(indices);
    }

  private:
    template <std::size_t... I>
    static auto one_(seq<I...>)
      -> decltype(value_t{valueset_t<I>::one()...})
    {
      return value_t{valueset_t<I>::one()...};
    }

  public:
    /// A tuple of ones.
    ///
    /// Template + decltype so that this is not defined when not all
    /// the valuesets support one().
    template <typename Indices = indices_t>
    static auto one() -> decltype(one_(Indices{}))
    {
      return one_(Indices{});
    }

    static bool
    is_one(const value_t& l)
    {
      return is_one_(l, indices);
    }

    static bool
    show_one()
    {
      return show_one_(indices);
    }

    /// Check if one appears for one of the valueset in the given tuple.
    static bool
    show_one(const value_t& v)
    {
      return show_one_(v, indices);
    }

    bool
    is_letter(const value_t&) const
    {
      // FIXME: why???  Is this for the printer of expressions?
      return false;
    }

    /// Pointwise addition.
    value_t
    add(const value_t& l, const value_t& r) const
    {
      return map_(l, r,
                  [](const auto& vs, const auto& l, const auto& r)
                  {
                    return vs.add(l, r);
                  });
    }

    /// The product (possibly concatenation) of \a l and \a r.
    ///
    /// Templated in order to work with tuples of letters and/or words
    /// in the case of labelsets.
    template <typename LhsValue, typename RhsValue>
    auto
    mul(const LhsValue& l, const RhsValue& r) const
      -> word_t
    {
      return this->mul_(l, r, indices);
    }

    /// Pointwise left GCD.
    value_t
    lgcd(const value_t& l, const value_t& r) const
    {
      return map_(l, r,
                  [](const auto& vs, const auto& l, const auto& r)
                  {
                    return vs.lgcd(l, r);
                  });
    }

    /// Pointwise right division (l / r).
    value_t
    rdivide(const value_t& l, const value_t& r) const
    {
      return map_(l, r,
                  [](const auto& vs, const auto& l, const auto& r)
                  {
                    return vs.rdivide(l, r);
                  });
    }

    boost::optional<value_t>
    maybe_rdivide(const value_t& l, const value_t& r) const
    {
      bool valid = true;
      auto res = map_(l, r,
                 [&valid](const auto& vs, const auto& l, const auto& r)
                 {
                   if (auto res = vs.maybe_rdivide(l, r))
                    return *res;
                   else
                    {
                      valid = false;
                      return l;
                    }
                 });

      if (valid)
        return res;
      else
        return boost::none;
    }

    /// Pointwise left division (l \ r).
    value_t
    ldivide(const value_t& l, const value_t& r) const
    {
      return map_(l, r,
                  [](const auto& vs, const auto& l, const auto& r)
                  {
                    return vs.ldivide(l, r);
                  });
    }

    boost::optional<value_t>
    maybe_ldivide(const value_t& l, const value_t& r) const
    {
      bool valid = true;
      auto res = map_(l, r,
                 [&valid](const auto& vs, const auto& l, const auto& r)
                 {
                   if (auto res = vs.maybe_ldivide(l, r))
                    return *res;
                   else
                    {
                      valid = false;
                      return l;
                    }
                 });

      if (valid)
        return res;
      else
        return boost::none;
    }

    /// Eliminate the LGCD between all the tapes.  E.g., `(abc, abd)
    /// => `(c, d)`.
    ///
    /// This tupleset must be homegeneous.
    typename valueset_t<0>::value_t
    lnormalize_here(value_t& v) const
    {
      return this->lnormalize_here_(v, indices);
    }

    /// Pointwise star.
    value_t
    star(const value_t& v) const
    {
      return map_(v,
                  [](const auto& vs, const auto& v)
                  {
                    return vs.star(v);
                  });
    }

    /// Add the special character first and last.
    ///
    /// Templated by Value so that we work for both word_t and label_t.
    /// Besides, avoids the problem of instantiation with weightsets
    /// that do not provide a word_t type.
    template <typename Value>
    Value
    delimit(const Value& l) const
    {
      return this->delimit_(l, indices);
    }

    /// Remove first and last characters, that must be "special".
    template <typename Value>
    Value
    undelimit(const Value& l) const
    {
      return this->undelimit_(l, indices);
    }

    // FIXME: this needs to be computed.
    static constexpr star_status_t star_status()
    {
      return star_status_t::STARRABLE;
    }

    /// Transpose a word_t or a value_t.
    template <typename Value>
    Value
    transpose(const Value& l) const
    {
      return this->transpose_(l, indices);
    }

    static size_t
    hash(const value_t& v)
    {
      return hash_(v, indices);
    }

    static value_t
    conv(self_t, value_t v)
    {
      return v;
    }

    value_t
    conv(b, b::value_t v) const
    {
      return v ? one() : zero();
    }

    /// Convert a value from tupleset<...> to value_t.
    template <typename... VS>
    value_t
    conv(const tupleset<VS...>& vs,
         const typename tupleset<VS...>::value_t& v) const
    {
      return this->conv_(vs, v, indices);
    }

    /// Convert a single tape expression to multitape.
    template <typename VS>
    value_t
    conv(const VS& vs, const typename VS::value_t& v) const
    {
      return this->partial_identity_(vs, v, indices);
    }

    /// Read one label from i, return the corresponding value.
    value_t
    conv(std::istream& i, bool quoted = true) const
    {
      constexpr auto has_label_one
        = is_labelset && has_one_mem_fn<self_t>{};
      return conv_(i, quoted, bool_constant<has_label_one>{});
    }

    /// Fun: (label_t) -> void.
    template <typename Fun>
    void convs(std::istream& i, Fun&& fun) const
    {
      eat(i, '[');
      conv_label_class_(*this, i,
                        [this,fun](const letter_t& l)
                        {
                          fun(this->value(l));
                        });
      eat(i, ']');
    }

    std::ostream&
    print_set(std::ostream& o, format fmt = {}) const
    {
      return this->print_set_(o, fmt, indices);
    }

    std::ostream&
    print(const value_t& l, std::ostream& o = std::cout,
          format fmt = {}) const
    {
      return this->print_(l, o,
                          // 1, (2, 3) is different from 1, 2, 3:
                          // delimit components.
                          fmt.delimit(true),
                          fmt.delimit() ? "(" : "",
                          fmt.is_for_labels() ? "|" : ",",
                          fmt.delimit() ? ")" : "",
                          indices);
    }

  private:
    template <std::size_t... I>
    static std::string sname_(seq<I...>)
    {
      std::string res = "lat<";
      const char *sep = "";
      for (auto n: {valueset_t<I>::sname()...})
        {
          res += sep;
          res += n;
          sep = ", ";
        }
      res.push_back('>');
      return res;
    }

    template <std::size_t... I>
    static constexpr bool
    is_commutative_(seq<I...>)
    {
      return all_<valueset_t<I>::is_commutative()...>();
    }

    template <std::size_t... I>
    static constexpr bool
    is_idempotent_(seq<I...>)
    {
      return all_<valueset_t<I>::is_idempotent()...>();
    }

    template <std::size_t... I>
    static self_t make_(std::istream& i, seq<I...>)
    {
#if VCSN_HAVE_CORRECT_LIST_INITIALIZER_ORDER
      return self_t{(eat_separator_<I>(i),
                     valueset_t<I>::make(i))...};
#else
      return make_gcc_tuple
        ((eat_separator_<sizeof...(ValueSets)-1 -I>(i),
          valueset_t<sizeof...(ValueSets)-1 -I>::make(i))...);
#endif
    }

    template <std::size_t... I>
    bool open_(bool o, seq<I...>) const
    {
      using swallow = int[];
      (void) swallow { set<I>().open(o)... };
      std::swap(o, open__);
      return o;
    }

    /// The size of the Ith element, if its valueset features a size()
    /// function.
    template <typename Value, std::size_t I>
    static auto size_(const Value& v, int)
      -> decltype(valueset_t<I>::size(std::get<I>(v)))
    {
      return valueset_t<I>::size(std::get<I>(v));
    }

    /// The size of the Ith element, if its valueset does not feature
    /// a size() function.
    template <typename Value, std::size_t I>
    static constexpr auto size_(const Value&, ...)
      -> size_t
    {
      return 0;
    }

    template <typename Value, std::size_t... I>
    static size_t size_(const Value& v, seq<I...>)
    {
      return std::max({size_<Value, I>(v, 0)...});
    }

    template <typename... Args, std::size_t... I>
    value_t value_(const std::tuple<Args...>& args, seq<I...>) const
    {
      return value_t{set<I>().value(std::get<I>(args))...};
    }

    template <typename... LS1, typename... LS2,
              std::size_t... I1, std::size_t... I2>
    value_t compose_(const tupleset<LS1...>&,
                     const typename tupleset<LS1...>::value_t& l1,
                     const tupleset<LS2...>&,
                     const typename tupleset<LS2...>::value_t& l2,
                     seq<I1...>, seq<I2...>) const
    {
      return value_t{std::get<I1>(l1)..., std::get<I2>(l2)...};
    }

    template <std::size_t... I>
    genset_ptr
    genset_(seq<I...>) const
    {
      return genset_ptr{set<I>().genset()...};
    }

    template <std::size_t... I>
    auto
    pregenerators_(seq<I...>) const
    {
      return vcsn::cross(set<I>().pregenerators()...);
    }

    template <typename... Args, std::size_t... I>
    word_t
    word_(const std::tuple<Args...>& l, seq<I...>) const
    {
      return word_t{set<I>().word(std::get<I>(l))...};
    }

    template <std::size_t... I>
    static bool
    equal_(const value_t& l, const value_t& r, seq<I...>)
    {
      for (auto n: {valueset_t<I>::equal(std::get<I>(l),
                                         std::get<I>(r))...})
        if (!n)
          return false;
      return true;
    }

    template <typename LhsValue, typename RhsValue, std::size_t... I>
    static auto
    less_(const LhsValue& l, const RhsValue& r, seq<I...>)
      -> bool
    {
      for (auto n: {std::make_pair(valueset_t<I>::less(std::get<I>(l),
                                                       std::get<I>(r)),
                                   valueset_t<I>::less(std::get<I>(r),
                                                       std::get<I>(l)))...})
        if (n.first)
          return true;
        else if (n.second)
          return false;
      return false;
    }

    template <std::size_t... I>
    static std::size_t
    hash_(const value_t& v, seq<I...>)
    {
      std::size_t res = 0;
      for (auto h: {valueset_t<I>::hash(std::get<I>(v))...})
        hash_combine(res, h);
      return res;
    }

    template <std::size_t... I>
    static value_t
    special_(seq<I...>)
    {
      return std::make_tuple(valueset_t<I>::special()...);
    }

    template <std::size_t... I>
    static bool
    is_special_(const value_t& l, seq<I...>)
    {
      for (auto n: {valueset_t<I>::is_special(std::get<I>(l))...})
        if (!n)
          return false;
      return true;
    }

    template <std::size_t... I>
    static auto zero_(seq<I...>)
      -> decltype(value_t{valueset_t<I>::zero()...})
    {
      return value_t{valueset_t<I>::zero()...};
    }

  public:
    /// A tuple of zeros.
    ///
    /// Template + decltype so that this is not defined when not all
    /// the valuesets support zero().
    template <typename Indices = indices_t>
    static auto zero() -> decltype(zero_(Indices{}))
    {
      return zero_(Indices{});
    }

  private:
    template <std::size_t... I>
    bool
    is_zero_(const value_t& l, seq<I...>) const
    {
      for (auto n: {set<I>().is_zero(std::get<I>(l))...})
        if (!n)
          return false;
      return true;
    }

    template <std::size_t... I>
    static constexpr bool
    has_lightening_weights_(seq<I...>)
    {
      return all_<valueset_t<I>::has_lightening_weights()...>();
    }

    template <std::size_t... I>
    static constexpr bool
    has_one_(seq<I...>)
    {
      return all_<valueset_t<I>::has_one()...>();
    }

    template <std::size_t... I>
    static constexpr bool
    is_expressionset_(seq<I...>)
    {
      return all_<valueset_t<I>::is_expressionset()...>();
    }

    template <std::size_t... I>
    static constexpr bool
    is_letterized_(seq<I...>)
    {
      return all_<valueset_t<I>::is_letterized()...>();
    }

    template <std::size_t... I>
    static bool
    is_one_(const value_t& l, seq<I...>)
    {
      for (auto n: {valueset_t<I>::is_one(std::get<I>(l))...})
        if (!n)
          return false;
      return true;
    }

    template <std::size_t... I>
    static bool
    show_one_(seq<I...>)
    {
      for (auto n: {valueset_t<I>::show_one()...})
        if (n)
          return true;
      return false;
    }

    template <std::size_t... I>
    static bool
    show_one_(const value_t& v, seq<I...>)
    {
      return any{}(valueset_t<I>::is_one(std::get<I>(v))...);
    }

    /// Run a nullary function pointwise, and return the tuple of results.
    template <typename Fun, std::size_t... I>
    auto
    map_impl_(Fun&& fun, seq<I...>) const
    {
      return std::make_tuple(fun(set<I>())...);
    }

    /// Apply a unary function pointwise, and return the tuple of results.
    template <typename Fun>
    value_t
    map_(const value_t& v, Fun&& fun) const
    {
      return map_impl_(v, std::forward<Fun>(fun), indices);
    }

    template <typename Fun, std::size_t... I>
    value_t
    map_impl_(const value_t& v, Fun&& fun, seq<I...>) const
    {
      return value_t{fun(set<I>(), std::get<I>(v))...};
    }

    /// Apply a binary function pointwise, and return the tuple of results.
    template <typename Fun>
    value_t
    map_(const value_t& l, const value_t& r, Fun&& fun) const
    {
      return map_impl_(l, r, std::forward<Fun>(fun), indices);
    }

    template <typename Fun, std::size_t... I>
    value_t
    map_impl_(const value_t& l, const value_t& r, Fun&& fun, seq<I...>) const
    {
      return value_t{fun(set<I>(), std::get<I>(l), std::get<I>(r))...};
    }

    template <typename LhsValue, typename RhsValue, std::size_t... I>
    auto
    mul_(const LhsValue& l, const RhsValue& r, seq<I...>) const
      -> word_t
    {
      return word_t{set<I>().mul(std::get<I>(l), std::get<I>(r))...};
    }

    template <std::size_t... I>
    typename valueset_t<0>::value_t
    lnormalize_here_(value_t& vs, seq<I...>) const
    {
      typename valueset_t<0>::value_t res = std::get<0>(vs);
      for (auto v: {std::get<I>(vs)...})
        res = set<0>().lgcd(res, v);
      using swallow = int[];
      (void) swallow { (set<0>().ldivide_here(res, std::get<I>(vs)), 0)... };
      return res;
    }

    template <typename Value, std::size_t... I>
    Value
    delimit_(const Value& l, seq<I...>) const
    {
      return Value{set<I>().delimit(std::get<I>(l))...};
    }

    template <typename Value, std::size_t... I>
    Value
    undelimit_(const Value& l, seq<I...>) const
    {
      return Value{set<I>().undelimit(std::get<I>(l))...};
    }

    template <typename... VS, std::size_t... I>
    value_t
    conv_(const tupleset<VS...>& vs,
          const typename tupleset<VS...>::value_t& v,
          seq<I...>) const
    {
      return value_t{set<I>().conv(vs.template set<I>(), std::get<I>(v))...};
    }

    template <typename VS, std::size_t... I>
    value_t
    partial_identity_(const VS& vs, const typename VS::value_t& v,
                      seq<I...>) const
    {
      return value_t{set<I>().conv(vs, v)...};
    }

    /// When the valuesets are labelsets and support one, accept the
    /// empty string to denote one.
    value_t
    conv_(std::istream& i, bool quoted, std::true_type) const
    {
      if (i.peek() == EOF)
        return one();
      else
        // This is not the empty string, bounce to the regular case.
        return conv_(i, quoted, std::false_type{});
    }

    /// Read a tuple in the stream, possibly parenthesized.
    value_t
    conv_(std::istream& i, bool quoted, std::false_type) const
    {
      bool par = i.peek() == '(';
      if (par)
        eat(i, '(');
      value_t res = conv_(i, quoted, indices);
      if (par)
        eat(i, ')');
      return res;
    }

    template <std::size_t... I>
    value_t
    conv_(std::istream& i, bool quoted, seq<I...>) const
    {
#if VCSN_HAVE_CORRECT_LIST_INITIALIZER_ORDER
      return value_t{(eat_separator_<I>(i),
                      set<I>().conv(i, quoted))...};
#else
      constexpr auto S = sizeof...(ValueSets)-1;
      return
        detail::make_gcc_tuple((eat_separator_<S - I>(i),
                                set<S - I>().conv(i, quoted))...);
#endif
    }

    /// Read the separator from the input stream \a i if I is not 0.
    ///
    /// The separator is '|' (for labels), but ',' is accepted too
    /// (for weights).
    template <std::size_t I>
    static void
    eat_separator_(std::istream& i)
    {
      if (I)
        eat(i, i.peek() == ',' ? ',' : '|');
      while (isspace(i.peek()))
        i.ignore();
    }


    template <std::size_t... I>
    std::ostream&
    print_(const value_t& l, std::ostream& o,
           format fmt,
           const char* pre,
           const char* sep,
           const char* post,
           seq<I...>) const
    {
      if (!is_special(l))
        {
          using swallow = int[];
          (void) swallow
            {
              (o << (I == 0 ? pre : sep),
               set<I>().print(std::get<I>(l), o, fmt),
               0)...
            };
          o << post;
        }
      return o;
    }

    template <std::size_t... I>
    std::ostream&
    print_set_(std::ostream& o, format fmt,
               seq<I...>) const
    {
      const char *sep = "";
      const char *close = "";
      if (fmt.delimit() && fmt.kind() != format::sname)
        o << '(';
      switch (fmt.kind())
        {
        case format::latex:
          sep = " \\times ";
          break;
        case format::sname:
          o << "lat<";
          sep = ", ";
          close = ">";
          break;
        case format::text:
          sep = " x ";
          break;
        case format::utf8:
          sep = " × ";
          break;
        case format::raw:
          assert(0);
          break;
        }
      using swallow = int[];
      (void) swallow
        {
          (o << (I == 0 ? "" : sep),
           set<I>().print_set(o, fmt.delimit(true)),
           0)...
        };
      o << close;
      if (fmt.delimit() && fmt.kind() != format::sname)
        o << ')';
      return o;
    }

    template <typename Value, std::size_t... I>
    Value
    transpose_(const Value& l, seq<I...>) const
    {
      return Value{set<I>().transpose(std::get<I>(l))...};
    }

    /// The intersection with another tupleset.
    template <std::size_t... I>
    self_t
    meet_(const self_t& rhs, seq<I...>) const
    {
      return self_t{meet(set<I>(), rhs.template set<I>())...};
    }

    /// The meet with another tupleset.
    friend
    self_t
    meet(const self_t& lhs, const self_t& rhs)
    {
      return lhs.meet_(rhs, indices);
    }

    /// The meet with the B weightset.
    friend
    self_t
    meet(const self_t& lhs, const b&)
    {
      return lhs;
    }

    /// The meet with the B weightset.
    friend
    self_t
    meet(const b&, const self_t& rhs)
    {
      return rhs;
    }

    /// The tupled valuesets.
    valuesets_t sets_;
    /// Whether this valueset is open.
    mutable bool open__ = false;

  private:
    /// Must be declared before, as we use its result in decltype.
    template <std::size_t... I>
    auto
    get_letter_(std::istream& i, bool quoted, seq<I...>) const
      -> letter_t
    {
#if VCSN_HAVE_CORRECT_LIST_INITIALIZER_ORDER
      return letter_t{(eat_separator_<I>(i),
                      set<I>().get_letter(i, quoted))...};
#else
      constexpr auto S = sizeof...(ValueSets)-1;
      return
        detail::make_gcc_tuple((eat_separator_<S - I>(i),
                                set<S - I>().get_letter(i, quoted))...);
#endif
    }

    /// Must be declared before, as we use its result in decltype.
    template <typename Value, std::size_t... I>
    static auto
    letters_of_(const Value& v, seq<I...>)
      -> decltype(zip(valueset_t<I>::letters_of(std::get<I>(v))...))
    {
      return zip(valueset_t<I>::letters_of(std::get<I>(v))...);
    }

    /// Must be declared before, as we use its result in decltype.
    template <typename Value, typename... Defaults, std::size_t... I>
    auto
    letters_of_padded_(const Value& v,
                       const std::tuple<Defaults...>& def, seq<I...>) const
      -> decltype(zip_with_padding(def,
                                   this->set<I>().letters_of_padded(std::get<I>(v),
                                                                    std::get<I>(def))...))
    {
      return zip_with_padding(def,
                              set<I>().letters_of_padded(std::get<I>(v),
                                                         std::get<I>(def))...);
    }

  public:
    template <std::size_t... I>
    auto
    get_letter(std::istream& i, bool quoted = true) const
      -> decltype(this->get_letter_(i, quoted, indices))
    {
      bool par = i.peek() == '(';
      if (par)
        eat(i, '(');
      auto res = get_letter_(i, quoted, indices);
      if (par)
        eat(i, ')');
      return res;
    }

    /// Iterate over the letters of v.
    ///
    /// Templated by Value so that we work for both word_t and
    /// label_t.  Besides, avoids the problem of instantiation with
    /// weightsets that do not provide a word_t type.
    template <typename Value>
    static auto
    letters_of(const Value& v)
      -> decltype(letters_of_(v, indices))
    {
      return letters_of_(v, indices);
    }

    /// Iterate over the letters of v.
    ///
    /// When the tapes are not the same length, instead of stopping when the
    /// shortest tape is consumed, continue until the end of every tape,
    /// padding with the default values provided.
    template <typename Value, typename... Defaults>
    auto
    letters_of_padded(const Value& v, const std::tuple<Defaults...>& def) const
      -> decltype(this->letters_of_padded_(v, def, indices))
    {
      return letters_of_padded_(v, def, indices);
    }
  };

  template <typename... ValueSets>
  tupleset<ValueSets...>
  make_tupleset(const ValueSets&... vss)
  {
    return {vss...};
  }


  /*----------------.
  | is_multitape.   |
  `----------------*/

  template <typename T1, typename T2>
  struct concat_tupleset;

  // Sure, we'd like to use tuple<> instead of
  // weightset_mixin<tupleset_impl<>>, however then we hit a Clang
  // 3.5.0 bug.
  //
  // https://llvm.org/bugs/show_bug.cgi?id=19372
  template <typename... T1, typename... T2>
  struct concat_tupleset<weightset_mixin<tupleset_impl<T1...>>,
                         weightset_mixin<tupleset_impl<T2...>>>
  {
    using type = weightset_mixin<tupleset_impl<T1..., T2...>>;
  };

  /// Conversion to letterized.
  template <typename... LabelSets>
  struct letterized_traits<tupleset<LabelSets...>>
  {
    using indices_t = make_index_sequence<sizeof...(LabelSets)>;

    template <std::size_t... I>
    using seq = index_sequence<I...>;

    template <size_t I>
    using letterized_traits_t =
      letterized_traits<tuple_element_t<I, std::tuple<LabelSets...>>>;
    template <std::size_t... I>
    static constexpr bool is_letterized_(seq<I...>)
    {
      return all_<letterized_traits_t<I>::is_letterized...>();
    }
    static constexpr bool is_letterized = is_letterized_(indices_t{});

    using labelset_t =
        tupleset<typename letterized_traits<LabelSets>::labelset_t...>;

    static labelset_t labelset(const tupleset<LabelSets...>& ls)
    {
      return labelset_(ls, indices_t{});
    }

    template <std::size_t... I>
    static labelset_t labelset_(const tupleset<LabelSets...>& ls,
                                seq<I...>)
    {
      return {make_letterized(std::get<I>(ls.sets()))...};
    }
  };


  /// Conversion to wordset.
  template <typename... LabelSets>
  struct law_traits<tupleset<LabelSets...>>
  {
    using labelset_t = tupleset<LabelSets...>;
    using type = tupleset<law_t<LabelSets>...>;

    template <std::size_t... I>
    static type value(const labelset_t& ls, index_sequence<I...>)
    {
      return {make_wordset(ls.template set<I>())...};
    }

    static type value(const labelset_t& ls)
    {
      return value(ls, make_index_sequence<sizeof...(LabelSets)>{});
    }
  };

  /// Join between two tuplesets, of the same size.
  template <typename... VS1, typename... VS2>
  struct join_impl<tupleset<VS1...>, tupleset<VS2...>>
  {
    static_assert(sizeof...(VS1) == sizeof...(VS2),
                  "join: tuplesets must have the same sizes");
    using vs1_t = tupleset<VS1...>;
    using vs2_t = tupleset<VS2...>;
    /// The resulting type.
    using type = tupleset<join_t<VS1, VS2>...>;

    template <std::size_t... I>
    static type join(const vs1_t& lhs, const vs2_t& rhs,
                     index_sequence<I...>)
    {
      return {::vcsn::join(lhs.template set<I>(), rhs.template set<I>())...};
    }

    /// The resulting valueset.
    static type join(const vs1_t& lhs, const vs2_t& rhs)
    {
      return join(lhs, rhs,
                  make_index_sequence<sizeof...(VS1)>{});
    }
  };

  /// Join between a tupleset, and a non tuple.
  template <typename... VS1, typename VS2>
  struct join_impl<tupleset<VS1...>, VS2>
  {
    using vs1_t = tupleset<VS1...>;
    using vs2_t = VS2;
    /// The resulting type.
    using type = tupleset<join_t<VS1, VS2>...>;

    template <std::size_t... I>
    static type join(const vs1_t& lhs, const vs2_t& rhs,
                     index_sequence<I...>)
    {
      return {::vcsn::join(lhs.template set<I>(), rhs)...};
    }

    /// The resulting valueset.
    static type join(const vs1_t& lhs, const vs2_t& rhs)
    {
      return join(lhs, rhs,
                  make_index_sequence<sizeof...(VS1)>{});
    }
  };


  /*------------------.
  | project_labelset. |
  `------------------*/

  /// The type of the resulting apparent LabelSet when keeping only
  /// tape Tape.
  template <size_t Tape, typename LabelSet>
  struct project_labelset_impl;

  /// The type of the resulting apparent LabelSet when keeping only
  /// tape Tape.  Undefined when not applicable.
  template <size_t Tape, typename LabelSet>
  using project_labelset = typename project_labelset_impl<Tape, LabelSet>::type;

  /// Case of tuplesets.
  template <size_t Tape, typename... LabelSets>
  struct project_labelset_impl<Tape, tupleset<LabelSets...>>
  {
    using valueset_t = tupleset<LabelSets...>;
    using type = typename valueset_t::template project_t<Tape>;
  };

  /// Case of multitape expressionsets.
  template <size_t Tape, typename Context>
  struct project_labelset_impl<Tape, expressionset<Context>>
  {
    using valueset_t = expressionset<Context>;
    using type = typename valueset_t::template project_t<Tape>;
  };


  /*------------------.
  | project_context.  |
  `------------------*/

  /// The type of the resulting apparent context when keeping only tape Tape.
  template <size_t Tape, typename Context>
  using project_context
    = context<project_labelset<Tape, labelset_t_of<Context>>,
              weightset_t_of<Context>>;

  }// detail::

  template <typename... ValueSet>
  struct is_multitape<tupleset<ValueSet...>>
    : std::true_type
  {};

  template <typename... ValueSets>
  struct number_of_tapes<tupleset<ValueSets...>>
  {
    constexpr static auto value = sizeof...(ValueSets);
  };


  /*----------------.
  | random_label.   |
  `----------------*/

  /// Random label from tupleset.
  template <typename... LabelSet,
            typename RandomGenerator = std::mt19937>
  typename tupleset<LabelSet...>::value_t
  random_label(const tupleset<LabelSet...>& ls,
               const std::string& spec,
               RandomGenerator& gen = make_random_engine())
  {
    return random_label_(ls, spec, gen, ls.indices);
  }


  /// Implementation detail for random label from tupleset.
  template <typename... LabelSet,
            size_t... I,
            typename RandomGenerator = std::mt19937>
  typename tupleset<LabelSet...>::value_t
  random_label_(const tupleset<LabelSet...>& ls,
                const std::string& spec,
                RandomGenerator& gen,
                detail::index_sequence<I...>)
  {
    // No need to check for the emptiness here: it will be checked in
    // each sub-labelset.
    return ls.tuple(random_label(ls.template set<I>(), spec, gen)...);
  }
}// vcsn::
