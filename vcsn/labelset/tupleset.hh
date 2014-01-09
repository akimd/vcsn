#ifndef VCSN_LABELSET_TUPLESET_HH
# define VCSN_LABELSET_TUPLESET_HH

# include <iosfwd>
# include <istream>
# include <tuple>

# include <vcsn/misc/escape.hh>
# include <vcsn/misc/stream.hh>
# include <vcsn/weights/b.hh>

namespace vcsn
{
  namespace ctx
  {
    namespace detail
    {

      // See "Pretty-print std::tuple"
      // <http://stackoverflow.com/questions/6245735>.
      template<std::size_t...> struct seq{};

      template<std::size_t N, std::size_t... Is>
      struct gen_seq : gen_seq<N-1, N-1, Is...>{};

      template<std::size_t... Is>
      struct gen_seq<0, Is...> : seq<Is...>{};

    }

    template <typename... LabelSets>
    class tupleset
    {
    public:
      using labelsets_t = std::tuple<LabelSets...>;
      using indices_t = detail::gen_seq<sizeof...(LabelSets)>;
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

      bool
      equals(const value_t& l, const value_t& r) const
      {
        return equals_(l, r, indices_t{});
      }

      /// Whether \a l < \a r.
      static bool less_than(const value_t l, const value_t r)
      {
        return less_than_(l, r, indices_t{});
      }

      value_t
      special() const
      {
        return special_(indices_t{});
      }

      bool
      is_special(const value_t& l) const
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

      value_t
      one() const
      {
        return one_(indices_t{});
      }

      bool
      is_one(const value_t& l) const
      {
        return is_one_(l, indices_t{});
      }

      bool
      show_one() const
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
      star(const value_t& l) const
      {
        return star_(l, indices_t{});
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

      /// Read one letter from i, return the corresponding label.
      value_t
      conv(std::istream& i) const
      {
        value_t res = conv_(i, indices_t{});
        eat(i, ')');
        return res;
      }

      // FIXME: remove, see todo.txt:scanners.
      value_t
      conv(const std::string& s) const
      {
        return ::vcsn::conv(*this, s);
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

      std::ostream&
      print(std::ostream& o, const value_t& l) const
      {
        return print_(o, l, indices_t{});
      }

      std::string
      format(const value_t& l) const
      {
        std::ostringstream o;
        print(o, l);
        return o.str();
      }

    private:
      template <std::size_t... I>
      static std::string sname_(detail::seq<I...>)
      {
        std::string res = "lat<";
        const char *sep = "";
        for (auto n: {(std::tuple_element<I, labelsets_t>::type::sname())...})
          {
            res += sep;
            res += n;
            sep = ", ";
          }
        res += ">";
        return res;
      }

      template <std::size_t... I>
      std::string vname_(bool full, detail::seq<I...>) const
      {
        std::string res = "lat<";
        const char *sep = "";
        for (auto n: {(std::get<I>(sets_).vname(full))...})
          {
            res += sep;
            res += n;
            sep = ", ";
          }
        res += ">";
        return res;
      }

      template <std::size_t... I>
      static tupleset make_(std::istream& i, detail::seq<I...>)
      {
        return {((eat_separator_<I>(i, '<', ','),
                  std::tuple_element<I, labelsets_t>::type::make(i)))...};
      }

      template <std::size_t... I>
      bool
      equals_(const value_t& l, const value_t& r, detail::seq<I...>) const
      {
        for (auto n: {(std::get<I>(sets_).equals(std::get<I>(l),
                                                 std::get<I>(r)))...})
          if (!n)
            return false;
        return true;
      }

      template <std::size_t... I>
      static bool
      less_than_(const value_t& l, const value_t& r, detail::seq<I...>)
      {
        for (auto n: {(std::tuple_element<I, labelsets_t>::type::less_than(std::get<I>(l),
                                                                           std::get<I>(r)))...})
          if (n)
            return true;
        return false;
      }

      template <std::size_t... I>
      value_t
      special_(detail::seq<I...>) const
      {
        return std::make_tuple((std::get<I>(sets_).special())...);
      }

      template <std::size_t... I>
      bool
      is_special_(const value_t& l, detail::seq<I...>) const
      {
        for (auto n: {(std::get<I>(sets_).is_special(std::get<I>(l)))...})
          if (!n)
            return false;
        return true;
      }

      template <std::size_t... I>
      value_t
      zero_(detail::seq<I...>) const
      {
        return std::make_tuple((std::get<I>(sets_).zero())...);
      }

      template <std::size_t... I>
      bool
      is_zero_(const value_t& l, detail::seq<I...>) const
      {
        for (auto n: {(std::get<I>(sets_).is_zero(std::get<I>(l)))...})
          if (!n)
            return false;
        return true;
      }

      template <std::size_t... I>
      value_t
      one_(detail::seq<I...>) const
      {
        return std::make_tuple((std::get<I>(sets_).one())...);
      }

      template <std::size_t... I>
      bool
      is_one_(const value_t& l, detail::seq<I...>) const
      {
        for (auto n: {(std::get<I>(sets_).is_one(std::get<I>(l)))...})
          if (!n)
            return false;
        return true;
      }

      template <std::size_t... I>
      bool
      show_one_(detail::seq<I...>) const
      {
        for (auto n: {(std::tuple_element<I, labelsets_t>::type::show_one())...})
          if (n)
            return true;
        return false;
      }

      template <std::size_t... I>
      value_t
      add_(const value_t& l, const value_t& r, detail::seq<I...>) const
      {
        return std::make_tuple((std::get<I>(sets_).add(std::get<I>(l),
                                                       std::get<I>(r)))...);
      }

      template <std::size_t... I>
      value_t
      mul_(const value_t& l, const value_t& r, detail::seq<I...>) const
      {
        return std::make_tuple((std::get<I>(sets_).mul(std::get<I>(l),
                                                       std::get<I>(r)))...);
      }

      template <std::size_t... I>
      value_t
      star_(value_t const& l, detail::seq<I...>) const
      {
        return std::make_tuple((std::get<I>(sets_).star(std::get<I>(l)))...);
      }

      template <std::size_t... I>
      value_t
      concat_(const value_t& l, const value_t& r, detail::seq<I...>) const
      {
        return std::make_tuple((std::get<I>(sets_).concat(std::get<I>(l),
                                                          std::get<I>(r)))...);
      }

      template <std::size_t... I>
      value_t
      conv_(std::istream& i, detail::seq<I...>) const
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
      print_(std::ostream& o, value_t const& l, detail::seq<I...>) const
      {
        if (!is_special(l))
          {
            using swallow = int[];
            (void) swallow
              {
                (o << (I == 0 ? "(" : ", "),
                 std::get<I>(sets_).print(o, std::get<I>(l)),
                 0)...
              };
            o << ")";
          }
        return o;
      }

      friend
      std::ostream&
      print_ls(const tupleset& ls,
               std::ostream& o, const std::string& format)
      {
        return print_ls_(ls, o, format, indices_t{});
      }

      template <std::size_t... I>
      friend
      std::ostream&
      print_ls_(const tupleset& ls,
                std::ostream& o, const std::string& format,
                detail::seq<I...>)
      {
        const char *sep = "";
        for (auto n: {(std::get<I>(ls.sets_))...})
          {
            o << sep;
            print_ls(n, o, format);
            if (format == "latex")
              sep = " \\times ";
            else if (format == "text")
              sep = " x ";
            else
              throw std::runtime_error("invalid format: " + format);
          }
        return o;
      }

      template <std::size_t... I>
      value_t
      transpose_(value_t const& l, detail::seq<I...>) const
      {
        return std::make_tuple((std::get<I>(sets_).transpose(std::get<I>(l)))...);
      }

      /// The intersection with another tupleset.
      template <std::size_t... I>
      tupleset
      meet_(const tupleset& rhs, detail::seq<I...>) const
      {
        return {(meet(std::get<I>(sets_),
                      std::get<I>(rhs.sets_)))...};
      }

      /// The join with another tupleset.
      template <std::size_t... I>
      tupleset
      join_(const tupleset& rhs, detail::seq<I...>) const
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
}
#endif // !VCSN_LABELSET_TUPLESET_HH
