#ifndef VCSN_LABELSET_TUPLESET_HH
# define VCSN_LABELSET_TUPLESET_HH

# include <iosfwd>
# include <istream>
# include <tuple>

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
      using labelsets_t = std::tuple<LabelSets...>;

    public:
      using value_t = std::tuple<typename LabelSets::value_t...>;

      tupleset(LabelSets... ls)
        : sets_(ls...)
      {}

      value_t
      transpose(const value_t& l) const
      {
        return transpose_(l, detail::gen_seq<sizeof...(LabelSets)>());
      }

      std::ostream&
      print(std::ostream& o, const value_t& l) const
      {
        return print_(o, l, detail::gen_seq<sizeof...(LabelSets)>());
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
      std::ostream&
      print_(std::ostream& o, value_t const& l, detail::seq<I...>) const
      {
        using swallow = int[];
        (void) swallow
          {
            (o << (I == 0 ? "(" : ", "),
             std::get<I>(sets_).print(o, std::get<I>(l)),
             0)...
          };
        return o << ")";
      }

      friend
      std::ostream&
      print_ls(const tupleset& ls,
               std::ostream& o, const std::string& format)
      {
        return print_ls_(ls, o, format,
                         detail::gen_seq<sizeof...(LabelSets)>{});
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

      labelsets_t sets_;
    };
  }
}
#endif // !VCSN_LABELSET_TUPLESET_HH
