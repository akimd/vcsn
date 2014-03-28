#ifndef VCSN_LABELSET_TUPLESET_HH
# define VCSN_LABELSET_TUPLESET_HH

# include <iosfwd>
# include <istream>
# include <tuple>

# include <vcsn/labelset/fwd.hh>
# include <vcsn/misc/escape.hh>
# include <vcsn/misc/raise.hh>
# include <vcsn/misc/stream.hh>
# include <vcsn/misc/tuple.hh>
# include <vcsn/weightset/b.hh>

namespace vcsn
{

  template <typename... LabelSets>
  class tupleset
  {
  public:
    using labelsets_t = std::tuple<LabelSets...>;
    using indices_t = vcsn::detail::make_index_sequence<sizeof...(LabelSets)>;
    template <std::size_t... I>
    using seq = vcsn::detail::index_sequence<I...>;

    /// The Ith labelset type.
    template <std::size_t I>
    using labelset_t = typename std::tuple_element<I, labelsets_t>::type;

  public:
    using self_type = tupleset;
    using value_t = std::tuple<typename LabelSets::value_t...>;
    using kind_t = labels_are_tuples;

    tupleset(LabelSets... ls)
      : sets_(ls...)
    {}

    static std::string sname()
    {
      return sname_(indices_t{});
    }

    std::string vname(bool full = true) const
    {
      return vname_(full, indices_t{});
    }

    /// Build from the description in \a is.
    static tupleset make(std::istream& is)
    {
      // name: lat<law_char(abc), law_char(xyz)>
      kind_t::make(is);
      auto res = make_(is, indices_t{});
      eat(is, '>');
      return res;
    }

    static
    bool
    equals(const value_t& l, const value_t& r)
    {
      return equals_(l, r, indices_t{});
    }

    /// Whether \a l < \a r.
    static bool less_than(const value_t l, const value_t r)
    {
      return less_than_(l, r, indices_t{});
    }

    static value_t
    special()
    {
      return special_(indices_t{});
    }

    static bool
    is_special(const value_t& l)
    {
      return is_special_(l, indices_t{});
    }

    value_t
    zero() const
    {
      return zero_(indices_t{});
    }

    bool
    is_zero(const value_t& l) const
    {
      return is_zero_(l, indices_t{});
    }

    static constexpr bool
    has_one()
    {
      return has_one_(indices_t{});
    }

    static value_t
    one()
    {
      return one_(indices_t{});
    }

    static bool
    is_one(const value_t& l)
    {
      return is_one_(l, indices_t{});
    }

    static bool
    show_one()
    {
      return show_one_(indices_t{});
    }

    bool
    is_letter(const value_t&) const
    {
      return false;
    }

    value_t
    add(const value_t& l, const value_t& r) const
    {
      return add_(l, r, indices_t{});
    }

    value_t
    mul(const value_t& l, const value_t& r) const
    {
      return mul_(l, r, indices_t{});
    }

    value_t
    rdiv(const value_t& l, const value_t& r) const
    {
      return rdiv_(l, r, indices_t{});
    }

    value_t
    ldiv(const value_t& l, const value_t& r) const
    {
      return ldiv_(l, r, indices_t{});
    }

    value_t
    star(const value_t& l) const
    {
      return star_(l, indices_t{});
    }

    // FIXME: this needs to be computed.
    static constexpr star_status_t star_status()
    {
      return star_status_t::STARRABLE;
    }

    value_t
    concat(const value_t& l, const value_t& r) const
    {
      return concat_(l, r, indices_t{});
    }

    value_t
    transpose(const value_t& l) const
    {
      return transpose_(l, indices_t{});
    }

    static
    size_t
    hash(const value_t& v)
    {
      return hash_(v, indices_t{});
    }

    static value_t
    conv(self_type, value_t v)
    {
      return v;
    }

    value_t
    conv(b, b::value_t v) const
    {
      return v ? one() : zero();
    }

    /// Read one letter from i, return the corresponding label.
    value_t
    conv(std::istream& i) const
    {
      value_t res = conv_(i, indices_t{});
      eat(i, ')');
      return res;
    }

    std::set<value_t> convs(std::istream&) const
    {
      raise("tupleset: ranges not implemented");
    }

    std::ostream&
    print_set(std::ostream& o, const std::string& format) const
    {
      return print_set_(o, format, indices_t{});
    }

    std::ostream&
    print(std::ostream& o, const value_t& l,
          const std::string& format = "text") const
    {
      return print_(o, l, format, indices_t{});
    }

  private:
    template <std::size_t... I>
    static std::string sname_(seq<I...>)
    {
      std::string res = "lat<";
      const char *sep = "";
      for (auto n: {(labelset_t<I>::sname())...})
        {
          res += sep;
          res += n;
          sep = ",";
        }
      res.push_back('>');
      return res;
    }

    template <std::size_t... I>
    std::string vname_(bool full, seq<I...>) const
    {
      std::string res = "lat<";
      const char *sep = "";
      for (auto n: {(std::get<I>(sets_).vname(full))...})
        {
          res += sep;
          res += n;
          sep = ",";
        }
      res.push_back('>');
      return res;
    }

    template <std::size_t... I>
    static tupleset make_(std::istream& i, seq<I...>)
    {
      return {((eat_separator_<I>(i, '<', ','),
                labelset_t<I>::make(i)))...};
    }

    template <std::size_t... I>
    static bool
    equals_(const value_t& l, const value_t& r, seq<I...>)
    {
      for (auto n: {(labelset_t<I>::equals(std::get<I>(l),
                                           std::get<I>(r)))...})
        if (!n)
          return false;
      return true;
    }

    template <std::size_t... I>
    static bool
    less_than_(const value_t& l, const value_t& r, seq<I...>)
    {
      for (auto n: {(labelset_t<I>::less_than(std::get<I>(l),
                                              std::get<I>(r)))...})
        if (n)
          return true;
      return false;
    }

    template <std::size_t... I>
    static std::size_t
    hash_(const value_t& v, seq<I...>)
    {
      std::size_t res = 0;
      for (auto h: {(labelset_t<I>::hash(std::get<I>(v)))...})
        std::hash_combine(res, h);
      return res;
    }

    template <std::size_t... I>
    static value_t
    special_(seq<I...>)
    {
      return std::make_tuple((labelset_t<I>::special())...);
    }

    template <std::size_t... I>
    static bool
    is_special_(const value_t& l, seq<I...>)
    {
      for (auto n: {(labelset_t<I>::is_special(std::get<I>(l)))...})
        if (!n)
          return false;
      return true;
    }

    template <std::size_t... I>
    value_t
    zero_(seq<I...>) const
    {
      return std::make_tuple((std::get<I>(sets_).zero())...);
    }

    template <std::size_t... I>
    bool
    is_zero_(const value_t& l, seq<I...>) const
    {
      for (auto n: {(std::get<I>(sets_).is_zero(std::get<I>(l)))...})
        if (!n)
          return false;
      return true;
    }

    template <std::size_t... I>
    static constexpr bool
    has_one_(seq<I...>)
    {
      return all_<labelset_t<I>::has_one()...>();
    }

    template <std::size_t... I>
    static value_t
    one_(seq<I...>)
    {
      return std::make_tuple(labelset_t<I>::one()...);
    }

    template <std::size_t... I>
    static bool
    is_one_(const value_t& l, seq<I...>)
    {
      for (auto n: {(labelset_t<I>::is_one(std::get<I>(l)))...})
        if (!n)
          return false;
      return true;
    }

    template <std::size_t... I>
    static bool
    show_one_(seq<I...>)
    {
      for (auto n: {(labelset_t<I>::show_one())...})
        if (n)
          return true;
      return false;
    }

    template <std::size_t... I>
    value_t
    add_(const value_t& l, const value_t& r, seq<I...>) const
    {
      return std::make_tuple((std::get<I>(sets_).add(std::get<I>(l),
                                                     std::get<I>(r)))...);
    }

    template <std::size_t... I>
    value_t
    mul_(const value_t& l, const value_t& r, seq<I...>) const
    {
      return std::make_tuple((std::get<I>(sets_).mul(std::get<I>(l),
                                                     std::get<I>(r)))...);
    }

    template <std::size_t... I>
    value_t
    rdiv_(const value_t& l, const value_t& r, seq<I...>) const
    {
      return std::make_tuple((std::get<I>(sets_).rdiv(std::get<I>(l),
                                                      std::get<I>(r)))...);
    }

    template <std::size_t... I>
    value_t
    ldiv_(const value_t& l, const value_t& r, seq<I...>) const
    {
      return std::make_tuple((std::get<I>(sets_).ldiv(std::get<I>(l),
                                                      std::get<I>(r)))...);
    }

    template <std::size_t... I>
    value_t
    star_(value_t const& l, seq<I...>) const
    {
      return std::make_tuple((std::get<I>(sets_).star(std::get<I>(l)))...);
    }

    template <std::size_t... I>
    value_t
    concat_(const value_t& l, const value_t& r, seq<I...>) const
    {
      return std::make_tuple((std::get<I>(sets_).concat(std::get<I>(l),
                                                        std::get<I>(r)))...);
    }

    template <std::size_t... I>
    value_t
    conv_(std::istream& i, seq<I...>) const
    {
      return std::make_tuple(((eat_separator_<I>(i, '(', ','),
                               std::get<I>(sets_).conv(i)))...);
    }

    /// Read the separator from the input stream \a i.
    /// If \a I is 0, then the separator is '(',
    /// otherwise it is ',' (possibly followed by spaces).
    template <std::size_t I>
    static void
    eat_separator_(std::istream& i, char first, char tail)
    {
      eat(i, I == 0 ? first : tail);
      while (isspace(i.peek()))
        i.ignore();
    }


    template <std::size_t... I>
    std::ostream&
    print_(std::ostream& o, value_t const& l,
           const std::string& format, seq<I...>) const
    {
      if (!is_special(l))
        {
          vcsn::detail::pass
            {
              (o << (I == 0 ? "(" : ", "),
               std::get<I>(sets_).print(o, std::get<I>(l), format),
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
          sep = ",";
        }
      else
        raise("invalid format: ", format);
      vcsn::detail::pass
        {
          (o << (I == 0 ? "" : sep),
           std::get<I>(sets_).print_set(o, format),
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
      return std::make_tuple((std::get<I>(sets_).transpose(std::get<I>(l)))...);
    }

    /// The intersection with another tupleset.
    template <std::size_t... I>
    tupleset
    meet_(const tupleset& rhs, seq<I...>) const
    {
      return {(meet(std::get<I>(sets_),
                    std::get<I>(rhs.sets_)))...};
    }

    /// The join with another tupleset.
    template <std::size_t... I>
    tupleset
    join_(const tupleset& rhs, seq<I...>) const
    {
      return {(join(std::get<I>(sets_),
                    std::get<I>(rhs.sets_)))...};
    }

    /// The meet with another tupleset.
    friend
    tupleset
    meet(const tupleset& lhs, const tupleset& rhs)
    {
      return lhs.meet_(rhs, indices_t{});
    }

    /// The meet with the B weightset.
    friend
    tupleset
    meet(const tupleset& lhs, const b&)
    {
      return lhs;
    }

    /// The meet with the B weightset.
    friend
    tupleset
    meet(const b&, const tupleset& rhs)
    {
      return rhs;
    }

    /// The join with another tupleset.
    friend
    tupleset
    join(const tupleset& lhs, const tupleset& rhs)
    {
      return lhs.join_(rhs, indices_t{});
    }

    /// The join with the B weightset.
    friend
    tupleset
    join(const tupleset& lhs, const b&)
    {
      return lhs;
    }

    /// The join with the B weightset.
    friend
    tupleset
    join(const b&, const tupleset& rhs)
    {
      return rhs;
    }

    labelsets_t sets_;
  };

}
#endif // !VCSN_LABELSET_TUPLESET_HH
