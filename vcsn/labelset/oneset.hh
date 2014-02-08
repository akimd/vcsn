#ifndef VCSN_LABELSET_ONESET_HH
# define VCSN_LABELSET_ONESET_HH

# include <iostream>
# include <set>
# include <stdexcept>

# include <vcsn/empty.hh>
# include <vcsn/core/kind.hh>
# include <vcsn/misc/hash.hh>
# include <vcsn/misc/raise.hh>

namespace vcsn
{
  namespace ctx
  {
    /// Implementation of labels are ones: there is a single instance
    /// of label.
    struct oneset
    {
      using value_t = vcsn::empty_t;

      using kind_t = labels_are_one;

      oneset() = default;

      static std::string sname()
      {
        return "lao";
      }

      static std::string vname(bool = true)
      {
        return sname();
      }

      /// Build from the description in \a is.
      static oneset make(std::istream& is)
      {
        // name: lao_ratexpset<law_char(xyz)_b>
        //       ^^^ ^^^^^^^^^^^^^^^^^^^^^^^^^^
        //       kind         weightset
        //
        // There is no "char(...)_".
        kind_t::make(is);
        return {};
      }

      /// Whether \a l == \a r.
      static bool
      equals(const value_t, const value_t)
      {
        return true;
      }

      /// Whether \a l < \a r.
      static bool less_than(const value_t, const value_t)
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

      static empty_t concat(empty_t, empty_t)
      {
        return {};
      }

      static std::ostream& print(std::ostream& o, empty_t,
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
    };

    /// The meet of two labelsets.
    inline
    oneset
    meet(const oneset&, const oneset&)
    {
      return {};
    }

    /// The union of two labelsets.
    inline
    oneset
    join(const oneset&, const oneset&)
    {
      return {};
    }
  }
}

#endif // !VCSN_LABELSET_ONESET_HH
