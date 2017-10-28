#include <sstream>

#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/dyn/value.hh>
#include <vcsn/misc/configuration.hh>

namespace vcsn
{
  namespace dyn LIBVCSN_API
  {
    inline
    bool are_equal(const automaton& lhs, const automaton& rhs)
    {
      return compare(lhs, rhs) == 0;
    }

    inline
    bool are_equal(const expression& lhs, const expression& rhs)
    {
      return compare(lhs, rhs) == 0;
    }

    inline
    std::string configuration(const std::string& key)
    {
      return vcsn::configuration(key);
    }

    template <typename Value>
    std::string format(const Value& v,
                       const std::string& format = "default")
    {
      std::ostringstream os;
      print(v, os, format);
      return os.str();
    }

    inline
    bool less_than(const automaton& lhs, const automaton& rhs)
    {
      return compare(lhs, rhs) < 0;
    }

    inline
    bool less_than(const expression& lhs, const expression& rhs)
    {
      return compare(lhs, rhs) < 0;
    }

    inline
    automaton make_automaton(const std::string& data,
                             const std::string& format,
                             bool strip)
    {
      auto&& is = std::istringstream{data};
      try
        {
          auto res = read_automaton(is, format, strip);
          require(is.peek() == EOF,
                  "unexpected trailing characters: ", is);
          return res;
        }
      catch (const std::runtime_error& e)
        {
          raise(e, "  while reading automaton");
        }
    }

    inline
    expression make_expression(const context& ctx,
                               const std::string& s, identities ids,
                               const std::string& format)
    {
      std::istringstream is{s};
      try
        {
          auto res = read_expression(ctx, ids, is, format);
          require(is.peek() == EOF,
                  "unexpected trailing characters: ", is);
          return res;
        }
      catch (const std::runtime_error& e)
        {
          raise(e, "  while reading expression: ", str_quote(s));
        }
    }

    inline
    label make_label(const context& ctx, const std::string& s,
                     const std::string& format)
    {
      auto&& is = std::istringstream{s};
      try
        {
          auto res = read_label(ctx, is, format);
          require(is.peek() == EOF,
                  "unexpected trailing characters: ", is);
          return res;
        }
      catch (const std::runtime_error& e)
        {
          raise(e, "  while reading label: ", str_quote(s));
        }
    }

    inline
    polynomial make_polynomial(const context& ctx, const std::string& s)
    {
      auto&& is = std::istringstream{s};
      try
        {
          auto res = read_polynomial(ctx, is);
          require(is.peek() == EOF,
                  "unexpected trailing characters: ", is);
          return res;
        }
      catch (const std::runtime_error& e)
        {
          raise(e, "  while reading polynomial: ", str_quote(s));
        }
    }

    inline
    weight make_weight(const context& ctx, const std::string& s)
    {
      auto&& is = std::istringstream{s};
      try
        {
          auto res = read_weight(ctx, is);
          require(is.peek() == EOF,
                  "unexpected trailing characters: ", is);
          return res;
        }
      catch (const std::runtime_error& e)
        {
          raise(e, "  while reading weight: ", str_quote(s));
        }
    }

    inline
    label make_word(const context& ctx, const std::string& s,
                    const std::string& format)
    {
      return make_label(make_word_context(ctx), s, format);
    }


    /*---------------.
    | Comparisons.   |
    `---------------*/

    template <typename Lhs, typename Rhs>
    auto operator==(const Lhs& l, const Rhs& r)
      -> decltype(l->vname(), r->vname(), bool())
    {
      return compare(l, r) == 0;
    }

    template <typename Lhs, typename Rhs>
    auto operator!=(const Lhs& l, const Rhs& r)
      -> decltype(l->vname(), r->vname(), bool())
    {
      return !(l == r);
    }

    template <typename Lhs, typename Rhs>
    auto operator<(const Lhs& l, const Rhs& r)
      -> decltype(compare(l, r) < 0)
    {
      return compare(l, r) < 0;
    }

    template <typename Lhs, typename Rhs>
    auto operator<=(const Lhs& l, const Rhs& r)
      -> decltype(compare(l, r) <= 0)
    {
      return compare(l, r) <= 0;
    }

    template <typename Lhs, typename Rhs>
    auto operator>(const Lhs& l, const Rhs& r)
      -> decltype(compare(l, r) > 0)
    {
      return compare(l, r) > 0;
    }

    template <typename Lhs, typename Rhs>
    auto operator>=(const Lhs& l, const Rhs& r)
      -> decltype(compare(l, r) >= 0)
    {
      return compare(l, r) >= 0;
    }


    /*--------------.
    | Operations.   |
    `--------------*/

    template <typename Lhs, typename Rhs>
    auto operator+(const Lhs& l, const Rhs& r)
      -> decltype(add(l, r))
    {
      return add(l, r);
    }

    template <typename Lhs, typename Rhs>
    auto operator&(const Lhs& l, const Rhs& r)
      -> decltype(conjunction(l, r))
    {
      return conjunction(l, r);
    }

    template <typename Lhs, typename Rhs>
    auto operator*(const Lhs& l, const Rhs& r)
      -> decltype(multiply(l, r))
    {
      return multiply(l, r);
    }
  }
}
