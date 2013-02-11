#ifndef VCSN_CTX_LAU_HH
# define VCSN_CTX_LAU_HH

# include <iostream>
# include <set>

# include <vcsn/empty.hh>

# include <vcsn/core/kind.hh>

namespace vcsn
{
  namespace ctx
  {
    struct Lau
    {
      using label_t = vcsn::empty_t;
      // FIXME: fishy, but that's what the previous version of the
      // code used to do.
      using letter_t = char;
      using word_t = std::string;
      using letters_t = std::set<letter_t>;

      using kind_t = labels_are_unit;

      Lau() = default;
      // FIXME: KILL
      Lau(const letters_t&)
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
      word_t
      identity() const
      {
        return {};
      }

      empty_t
      transpose(empty_t) const
      {
        return {};
      }

      // FIXME: KILL ME!
      // Here to please polynomialset for entries in mutable_automaton.
      word_t
      transpose(word_t) const
      {
        return {};
      }

      bool
      is_letter(const empty_t&) const
      {
        return false;
      }

      // FIXME: KILL ME TOO!
      word_t
      concat(word_t, word_t) const
      {
        return {};
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
      print(std::ostream& o, const empty_t&) const
      {
        return o << "EMPTY1";
      }

      // FIXME: KILL ME PLEAAAAAAAAASE!
      std::ostream&
      print(std::ostream& o, const word_t&) const
      {
        return o << "EMPTY2";
      }

      // KILLME:
      word_t
      to_word(const empty_t) const
      {
        return {};
      }

      std::string
      format(const empty_t&) const
      {
        return "EMPTY3";
      }

      std::string
      format(const word_t&) const
      {
        return "\\e";
      }
    };
  }
}

#endif // !VCSN_CTX_LAU_HH
