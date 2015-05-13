#pragma once

#include <iosfwd>
#include <istream>
#include <set>
#include <tuple>

#include <vcsn/config.hh> // VCSN_HAVE_CORRECT_LIST_INITIALIZER_ORDER
#include <vcsn/labelset/fwd.hh>
#include <vcsn/labelset/labelset.hh>
#include <vcsn/misc/escape.hh>
#include <vcsn/misc/raise.hh>
#include <vcsn/misc/stream.hh>
#include <vcsn/misc/tuple.hh>
#include <vcsn/misc/cross.hh>
#include <vcsn/misc/raise.hh>
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
    using letter_t = void;
    /// Same as value_t.
    using word_t = std::tuple<typename ValueSets::value_t...>;
  };

  /// Specialization for tuples of labelsets.
  template <typename... ValueSets>
  struct labelset_types_impl<decltype(pass{typename ValueSets::word_t()...}, void()),
                             ValueSets...>
  {
    using genset_t
      = cross_sequences<decltype(std::declval<ValueSets>().genset())...>;
    using letter_t = std::tuple<typename ValueSets::letter_t...>;
    using word_t = std::tuple<typename ValueSets::word_t...>;
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
    using valueset_t = typename std::tuple_element<I, valuesets_t>::type;

  public:
    using self_t = tupleset<ValueSets...>;

    /// A tuple of values.
    using value_t = std::tuple<typename ValueSets::value_t...>;

    /// A tuple of letters if meaningful, void otherwise.
    using letter_t = typename labelset_types<ValueSets...>::letter_t;
    /// A tuple of gensets if meaningful, void otherwise.
    using genset_t = typename labelset_types<ValueSets...>::genset_t;
    /// A tuple of words if meaningful, void otherwise.
    using word_t = typename labelset_types<ValueSets...>::word_t;

    /// To be iterable.
    using value_type = letter_t;

    using kind_t = labels_are_tuples;

    tupleset_impl(valuesets_t vs)
      : sets_(vs)
    {}

    tupleset_impl(ValueSets... ls)
      : sets_(ls...)
    {}

    static symbol sname()
    {
      static symbol res(sname_(indices));
      return res;
    }

    static constexpr std::size_t size()
    {
      return sizeof...(ValueSets);
    }

    /// Get the max of the sizes of the tapes
    static size_t size(const value_t& v)
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

    /// The generators.  Meaningful for labelsets only.
    genset_t
    genset() const
    {
      return this->genset_(indices);
    }

    static constexpr bool is_free()
    {
      return is_free_(indices);
    }

  private:
    template <typename LhsValue, typename RhsValue, std::size_t... I>
    auto
    mul_(const LhsValue& l, const RhsValue& r, seq<I...>) const
      -> word_t
    {
      return word_t{set<I>().mul(std::get<I>(l), std::get<I>(r))...};
    }

  public:
    /// Convert to a word.
    template <typename... Args>
    auto
    word(const std::tuple<Args...>& v) const
      -> word_t
    {
      return this->word_(v, indices);
    }

    /// Whether \a l equals \a r.
    static bool
    equal(const value_t& l, const value_t& r)
    {
      return equal_(l, r, indices);
    }

    /// Whether \a l < \a r.
    static bool
    less(const value_t& l, const value_t& r)
    {
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

    value_t
    zero() const
    {
      return this->zero_(indices);
    }

    bool
    is_zero(const value_t& l) const
    {
      return is_zero_(l, indices);
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

    static value_t
    one()
    {
      return one_(indices);
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

    bool
    is_letter(const value_t&) const
    {
      return false;
    }

    value_t
    add(const value_t& l, const value_t& r) const
    {
      return this->add_(l, r, indices);
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

    value_t
    rdiv(const value_t& l, const value_t& r) const
    {
      return this->rdiv_(l, r, indices);
    }

    value_t
    ldiv(const value_t& l, const value_t& r) const
    {
      return this->ldiv_(l, r, indices);
    }

    /// This tupleset must be homegeneous.
    typename valueset_t<0>::value_t
    lnormalize_here(value_t& v) const
    {
      return this->lnormalize_here_(v, indices);
    }

    value_t
    star(const value_t& l) const
    {
      return this->star_(l, indices);
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

    value_t
    transpose(const value_t& l) const
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

    /// Convert a value from nullableset<tupleset<...>> to value_t.
    template <typename... VS>
    value_t
    conv(const nullableset<tupleset<VS...>>& vs,
         const typename nullableset<tupleset<VS...>>::value_t& v) const
    {
      return conv(*vs.labelset(), vs.get_value(v));
    }

    /// Read one label from i, return the corresponding value.
    value_t
    conv(std::istream& i) const
    {
      bool par = i.peek() == '(';
      if (par)
        eat(i, '(');
      value_t res = conv_(i, indices);
      if (par)
        eat(i, ')');
      return res;
    }

    std::set<value_t> convs(std::istream&) const
    {
      raise("tupleset: ranges not implemented");
    }

    std::ostream&
    print_set(std::ostream& o, const std::string& format = "text") const
    {
      return this->print_set_(o, format, indices);
    }

    std::ostream&
    print(const value_t& l, std::ostream& o,
          const std::string& format = "text") const
    {
      return this->print_(l, o, format, indices);
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
      return self_t{(eat_separator_<I>(i, ','),
                     valueset_t<I>::make(i))...};
#else
      return make_gcc_tuple
        ((eat_separator_<sizeof...(ValueSets)-1 -I>(i, ','),
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

    template <std::size_t... I>
    static size_t size_(const value_t& v, seq<I...>)
    {
      return std::max({(valueset_t<I>::size(std::get<I>(v)))...});
    }

    template <typename... Args, std::size_t... I>
    value_t value_(const std::tuple<Args...>& args, seq<I...>) const
    {
      return value_t{set<I>().value(std::get<I>(args))...};
    }

    template <std::size_t... I>
    genset_t
    genset_(seq<I...>) const
    {
      return cross(set<I>().genset()...);
    }

    template <std::size_t... I>
    static constexpr bool
    is_free_(seq<I...>)
    {
      return all_<valueset_t<I>::is_free()...>();
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

    template <std::size_t... I>
    static bool
    less_(const value_t& l, const value_t& r, seq<I...>)
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
    value_t
    zero_(seq<I...>) const
    {
      return value_t{set<I>().zero()...};
    }

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
    static value_t
    one_(seq<I...>)
    {
      return value_t{valueset_t<I>::one()...};
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
    value_t
    add_(const value_t& l, const value_t& r, seq<I...>) const
    {
      return value_t{set<I>().add(std::get<I>(l), std::get<I>(r))...};
    }

    template <std::size_t... I>
    value_t
    rdiv_(const value_t& l, const value_t& r, seq<I...>) const
    {
      return value_t{set<I>().rdiv(std::get<I>(l), std::get<I>(r))...};
    }

    template <std::size_t... I>
    value_t
    ldiv_(const value_t& l, const value_t& r, seq<I...>) const
    {
      return value_t{set<I>().ldiv(std::get<I>(l), std::get<I>(r))...};
    }

    template <std::size_t... I>
    typename valueset_t<0>::value_t
    lnormalize_here_(value_t& vs, seq<I...>) const
    {
      typename valueset_t<0>::value_t res = std::get<0>(vs);
      for (auto v: {std::get<I>(vs)...})
        res = set<0>().lgcd(res, v);
      using swallow = int[];
      (void) swallow { (set<0>().ldiv_here(res, std::get<I>(vs)), 0)... };
      return res;
    }

    template <std::size_t... I>
    value_t
    star_(value_t const& l, seq<I...>) const
    {
      return value_t{set<I>().star(std::get<I>(l))...};
    }

    template <typename Value, std::size_t... I>
    Value
    delimit_(Value const& l, seq<I...>) const
    {
      return Value{set<I>().delimit(std::get<I>(l))...};
    }

    template <typename Value, std::size_t... I>
    Value
    undelimit_(Value const& l, seq<I...>) const
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

    template <std::size_t... I>
    value_t
    conv_(std::istream& i, seq<I...>) const
    {
#if VCSN_HAVE_CORRECT_LIST_INITIALIZER_ORDER
      return value_t{(eat_separator_<I>(i, ','),
                      set<I>().conv(i))...};
#else
      constexpr auto S = sizeof...(ValueSets)-1;
      return
        detail::make_gcc_tuple((eat_separator_<S - I>(i, ','),
                                std::get<S - I>(sets_).conv(i))...);
#endif
    }

    /// Read the separator from the input stream \a i.
    /// If \a I is 0, then the separator is '(',
    /// otherwise it is ',' (possibly followed by spaces).
    template <std::size_t I>
    static void
    eat_separator_(std::istream& i, char sep)
    {
      if (I)
        eat(i, sep);
      while (isspace(i.peek()))
        i.ignore();
    }


    template <std::size_t... I>
    std::ostream&
    print_(value_t const& l, std::ostream& o,
           const std::string& format, seq<I...>) const
    {
      if (!is_special(l))
        {
          using swallow = int[];
          (void) swallow
            {
              (o << (I == 0 ? '(' : ','),
               set<I>().print(std::get<I>(l), o, format),
               0)...
            };
          o << ')';
        }
      return o;
    }

    template <std::size_t... I>
    std::ostream&
    print_set_(std::ostream& o, const std::string& format,
               seq<I...>) const
    {
      const char *sep = "";
      if (format == "latex")
        sep = " \\times ";
      else if (format == "text")
        {
          o << "lat<";
          sep = ", ";
        }
      else
        raise("invalid format: ", format);
      using swallow = int[];
      (void) swallow
        {
          (o << (I == 0 ? "" : sep),
           set<I>().print_set(o, format),
           0)...
        };
      if (format == "text")
        o << '>';
      return o;
    }

    template <std::size_t... I>
    value_t
    transpose_(value_t const& l, seq<I...>) const
    {
      return value_t{(set<I>().transpose(std::get<I>(l)))...};
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

    valuesets_t sets_;
    mutable bool open__ = false;

  private:
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
    letters_of_padded_(const Value& v, const std::tuple<Defaults...>& def, seq<I...>) const
      -> decltype(zip_with_padding(def, std::get<I>(this->sets_).letters_of_padded(std::get<I>(v),
                                                                                   std::get<I>(def))...))
    {
      return zip_with_padding(def,
                              std::get<I>(sets_).letters_of_padded(std::get<I>(v),
                                                                   std::get<I>(def))...);
    }

  public:
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
        letterized_traits<typename std::tuple_element<I, std::tuple<LabelSets...>>::type>;
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

  /// Conversion to a nullableset: all the labelsets support one.
  template <typename... LabelSets>
  struct nullableset_traits<tupleset<LabelSets...>,
                            enable_if_t<tupleset<LabelSets...>::has_one()>>
  {
    using labelset_t = tupleset<LabelSets...>;
    using type = labelset_t;
    static type value(const labelset_t& ls)
    {
      return ls;
    }
  };

  /// Conversion to a nullableset: not all the labelsets support one.
  template <typename... LabelSets>
  struct nullableset_traits<tupleset<LabelSets...>,
                            enable_if_t<!tupleset<LabelSets...>::has_one()>>
  {
    using labelset_t = tupleset<LabelSets...>;
    using type = nullableset<labelset_t>;

    static type value(const labelset_t& ls)
    {
      return ls;
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
    static type join(const vs1_t lhs, const vs2_t& rhs,
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

  }// detail::
}// vcsn::
