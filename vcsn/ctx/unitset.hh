#ifndef VCSN_CTX_UNITSET_HH
# define VCSN_CTX_UNITSET_HH

# include <iostream>
# include <set>

# include <vcsn/empty.hh>

# include <vcsn/core/kind.hh>
# include <stdexcept>

namespace vcsn
{
  namespace ctx
  {
    /// Implementation of labels are units: there is a single instance
    /// of label.
    struct unitset
    {
      using label_t = vcsn::empty_t;
      // FIXME: fishy, but that's what the previous version of the
      // code used to do.
      using letter_t = char;
      using word_t = vcsn::empty_t;
      using letters_t = std::set<letter_t>;

      using kind_t = labels_are_unit;

      unitset() = default;
      // FIXME: KILL
      unitset(const letters_t&)
      {}

      static std::string sname()
      {
        return "lau";
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

      // FIXME: fishy, but that's what the previous version of the
      // code used to do.
      empty_t
      empty_word() const
      {
        return {};
      }

      bool
      is_empty_word(empty_t) const
      {
        return true;
      }

      empty_t
      identity() const
      {
        return {};
      }

      bool
      is_identity(empty_t) const
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
        return o << "EMPTY1";
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

      // KILLME:
      word_t
      to_word(empty_t) const
      {
        return {};
      }

      std::string
      format(empty_t) const
      {
        return "\\e";
      }
    };
  }
}

#endif // !VCSN_CTX_UNITSET_HH
