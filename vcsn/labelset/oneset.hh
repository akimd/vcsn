#ifndef VCSN_LABELSET_ONESET_HH
# define VCSN_LABELSET_ONESET_HH

# include <iostream>
# include <set>
# include <stdexcept>

# include <vcsn/empty.hh>

# include <vcsn/core/kind.hh>
# include <vcsn/misc/stream.hh> // conv.

namespace vcsn
{
  namespace ctx
  {
    /// Implementation of labels are ones: there is a single instance
    /// of label.
    struct oneset
    {
      using value_t = vcsn::empty_t;
      using label_t = value_t;

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

      static bool equals(empty_t, empty_t)
      {
        return true;
      }

      static label_t special()
      {
        return {};
      }

      /// The special label is indistinguishable for the others.
      constexpr static bool
      is_special(label_t)
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

      static std::ostream& print(std::ostream& o, empty_t)
      {
        return o;
      }

      static label_t conv(std::istream& i)
      {
        if (i.peek() == '\\')
          {
            i.ignore();
            char c = i.peek();
            if (c != 'e')
              throw std::domain_error("invalid label: unexpected \\"
                                      + std::string{c});
            i.ignore();
          }
        return {};
      }

      // FIXME: remove, see todo.txt:scanners.
      static label_t conv(const std::string& s)
      {
        return ::vcsn::conv(oneset{}, s);
      }

      static std::string format(empty_t)
      {
        return "\\e";
      }
    };

    /// The intersection of two labelsets.
    inline
    oneset
    intersection(const oneset&, const oneset&)
    {
      return {};
    }

    /// The union of two labelsets.
    inline
    oneset
    get_union(const oneset&, const oneset&)
    {
      return {};
    }
  }
}

#endif // !VCSN_LABELSET_ONESET_HH
