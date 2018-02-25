#pragma once

#include <iostream>
#include <set>
#include <stdexcept>

#include <vcsn/core/kind.hh>
#include <vcsn/labelset/labelset.hh>
#include <vcsn/misc/functional.hh>
#include <vcsn/misc/raise.hh>

namespace vcsn
{
  /// Implementation of labels are ones: there is a single instance of
  /// label.
  ///
  /// Actually, this is untrue: there are two labels: one() and
  /// special().
  class oneset
  {
  public:
    using self_t = oneset;
    using value_t = bool;

    using kind_t = labels_are_one;

    oneset() = default;

    static symbol sname()
    {
      static auto res = symbol{"oneset"};
      return res;
    }

    /// Build from the description in \a is.
    static oneset make(std::istream& is)
    {
      eat(is, sname());
      return {};
    }

    /// Does not make a lot of sense.
    bool open(bool o) const
    {
      return !o;
    }

    /// Value constructor.
    template <typename... Args>
    value_t value(Args&&... args) const
    {
      return value_t{std::forward<Args>(args)...};
    }

    /// Three-way comparison between \a l and \a r.
    static int compare(const value_t l, const value_t r)
    {
      return int(l) - int(r);
    }

    /// Whether \a l == \a r.
    static bool
    equal(const value_t l, const value_t r)
    {
      return l == r;
    }

    /// Whether \a l < \a r.
    static bool less(const value_t l, const value_t r)
    {
      return l < r;
    }

    static value_t special()
    {
      return false;
    }

    /// The special label is indistinguishable for the others.
    static bool is_special(const value_t v)
    {
      return v == special();
    }

    static constexpr bool
    is_expressionset()
    {
      return false;
    }

    static constexpr bool
    has_one()
    {
      return true;
    }

    static constexpr bool
    is_letterized()
    {
      return false;
    }

    static value_t one()
    {
      return true;
    }

    static bool is_one(const value_t v)
    {
      return v == one();
    }

    static value_t transpose(const value_t v)
    {
      return v;
    }

    static bool is_letter(const value_t)
    {
      return false;
    }

    static value_t mul(value_t l, value_t r)
    {
      return l && r;
    }

    static std::ostream& print(const value_t, std::ostream& o = std::cout,
                               format = {})
    {
      return o;
    }

    static size_t size(value_t)
    {
      return 0;
    }

    static size_t hash(value_t v)
    {
      return hash_value(v);
    }

    static value_t
    conv(self_t, value_t v)
    {
      return v;
    }

    /// Convert from labelset to oneset.
    template <typename LabelSet>
    value_t
    conv(const LabelSet& ls,
         typename LabelSet::value_t v) const
    {
      VCSN_REQUIRE(ls.is_one(v),
                   *this, ": conv: invalid label: ", to_string(ls, v));
      return one();
    }


    value_t conv(std::istream& i, bool = true) const
    {
      if (i.peek() == '\\')
        {
          i.ignore();
          int c = i.peek();
          require(c == 'e',
                  *this, ": invalid label: unexpected \\", c);
          i.ignore();
        }
      return one();
    }

    template <typename Fun>
    void convs(std::istream&, Fun) const
    {
      raise(*this, ": ranges not implemented");
    }

    std::ostream&
    print_set(std::ostream& o, format fmt = {}) const
    {
      switch (fmt.kind())
        {
        case format::latex:
          o << "\\{\\varepsilon\\}";
          break;
        case format::sname:
          o << sname();
          break;
        case format::text:
          o << "{\\e}";
          break;
        case format::utf8:
          o << "{ε}";
          break;
        case format::ere:
        case format::raw:
        case format::redgrep:
          assert(0);
          break;
        }
      return o;
    }
  };

  namespace detail
  {
    /// Conversion to a wordset: identity.
    template <>
    struct law_traits<oneset>
    {
      using type = oneset;
      static type value(oneset)
      {
        return {};
      }
    };

    /*-------.
    | Join.  |
    `-------*/

    /// `oneset v LS -> LS` if \c LS\::has_one.
    template <typename LS>
    struct join_impl<oneset, LS,
                     std::enable_if_t<LS::has_one()>>
    {
      using type = LS;
      static type join(const oneset&, const LS& ls)
      {
        return ls;
      }
    };

    template <>
    struct join_impl<oneset, oneset>
    {
      using type = oneset;
      static type join(const oneset&, const oneset&)
      {
        return {};
      }
    };
  }

  /// The meet of two labelsets.
  inline
  oneset
  meet(const oneset&, const oneset&)
  {
    return {};
  }

  /// Random label from oneset.
  template <typename RandomGenerator = std::mt19937>
  typename oneset::value_t
  random_label(const oneset& ls,
               const std::string&,
               RandomGenerator& = make_random_engine())
  {
    return ls.one();
  }
}
