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
      using label_t = vcsn::empty_t;
      // FIXME: fishy, but that's what the previous version of the
      // code used to do.
      using letter_t = char;
      using word_t = vcsn::empty_t;
      using letters_t = std::set<letter_t>;

      using kind_t = labels_are_one;

      oneset() = default;
      // FIXME: KILL
      oneset(const letters_t&)
      {}

      static std::string sname()
      {
        return "lao";
      }

      std::string vname(bool = true) const
      {
        return sname();
      }

      bool
      equals(empty_t, empty_t) const
      {
        return true;
      }

      label_t
      special() const
      {
        return {};
      }

      /// The special label is indistinguishable for the others.
      constexpr static bool
      is_special(label_t)
      {
        return true;
      }

      empty_t
      one() const
      {
        return {};
      }

      bool
      is_one(empty_t) const
      {
        return true;
      }

      empty_t
      transpose(empty_t) const
      {
        return {};
      }

      bool
      is_letter(empty_t) const
      {
        return false;
      }

      empty_t
      concat(empty_t, empty_t) const
      {
        return {};
      }

      bool
      has(letter_t) const
      {
        return false;
      }

      std::ostream&
      print(std::ostream& o, empty_t) const
      {
        return o;
      }

      label_t
      conv(std::istream& i) const
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
      label_t
      conv(const std::string& s) const
      {
        return ::vcsn::conv(*this, s);
      }

      std::string
      format(empty_t) const
      {
        return "\\e";
      }
    };
  }
}

#endif // !VCSN_LABELSET_ONESET_HH
