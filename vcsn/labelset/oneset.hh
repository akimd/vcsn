#pragma once

#include <iostream>
#include <set>
#include <stdexcept>

#include <vcsn/core/kind.hh>
#include <vcsn/misc/empty.hh>
#include <vcsn/labelset/labelset.hh>
#include <vcsn/misc/functional.hh>
#include <vcsn/misc/raise.hh>

namespace vcsn
{
  /// Implementation of labels are ones: there is a single instance
  /// of label.
  class oneset
  {
  public:
    using self_type = oneset;
    using value_t = vcsn::empty_t;

    using kind_t = labels_are_one;

    oneset() = default;

    static symbol sname()
    {
      static symbol res("lao");
      return res;
    }

    /// Build from the description in \a is.
    static oneset make(std::istream& is)
    {
      kind_t::make(is);
      return {};
    }

    /// Does not make a lot of sense.
    bool open(bool o) const
    {
      return !o;
    }

    static constexpr bool is_free()
    {
      // This is debatable.  However, in Vaucanson, if a labelset
      // is_free, then we expect to be able to iterate on its genset,
      // and I don't plan to provide a genset here.
      return false;
    }

    /// Value constructor.
    template <typename... Args>
    value_t value(Args&&... args) const
    {
      return value_t{std::forward<Args>(args)...};
    }

    /// Whether \a l == \a r.
    static bool
    equal(const value_t, const value_t)
    {
      return true;
    }

    /// Whether \a l < \a r.
    static bool less(const value_t, const value_t)
    {
      return false;
    }

    static value_t special()
    {
      return {};
    }

    /// The special label is indistinguishable for the others.
    constexpr static bool
    is_special(value_t)
    {
      return true;
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

    static empty_t one()
    {
      return {};
    }

    static bool is_one(empty_t)
    {
      return true;
    }

    static empty_t transpose(empty_t)
    {
      return {};
    }

    static bool is_letter(empty_t)
    {
      return false;
    }

    static empty_t mul(empty_t, empty_t)
    {
      return {};
    }

    static std::ostream& print(empty_t, std::ostream& o,
                               const std::string& = "text")
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
    conv(self_type, value_t v)
    {
      return v;
    }

    static value_t conv(std::istream& i)
    {
      if (i.peek() == '\\')
        {
          i.ignore();
          char c = i.peek();
          require(c == 'e',
                  "invalid label: unexpected \\", c);
          i.ignore();
        }
      return {};
    }

    static std::set<value_t> convs(std::istream&)
    {
      raise("oneset: ranges not implemented");
    }

    std::ostream&
    print_set(std::ostream& o, const std::string& format = "text") const
    {
      if (format == "latex")
        o << "1";
      else if (format == "text")
        o << sname();
      else
        raise("invalid format: ", format);
      return o;
    }
  };

  namespace detail
  {
    /// Conversion to a nullableset: identity.
    template <>
    struct nullableset_traits<oneset>
    {
      using type = oneset;
      static type value(oneset)
      {
        return {};
      }
    };

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
}
