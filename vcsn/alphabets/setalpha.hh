#pragma once

#include <cassert>
#include <initializer_list>
#include <stdexcept>

#include <boost/optional.hpp>

#include <vcsn/misc/escape.hh>
#include <vcsn/misc/format.hh>
#include <vcsn/misc/raise.hh>
#include <vcsn/misc/set.hh>
#include <vcsn/misc/stream.hh> // eat.
#include <vcsn/misc/symbol.hh>
#include <vcsn/misc/type_traits.hh>

namespace vcsn
{
  /// A set of letters of type \a L.
  ///
  /// \a L is not simply `char` or so.  Rather, see char_letters.
  template <typename L>
  class set_alphabet: public L
  {
  public:
    using letter_t = typename L::letter_t;
    using word_t = typename L::word_t;
    using letters_t = std::set<letter_t, vcsn::less<L, letter_t>>;
    /// The type of our values, when seen as a container.
    using value_type = letter_t;

    static symbol sname()
    {
      static auto res = L::sname();
      return res;
    }

    static set_alphabet make(std::istream& is)
    {
      // name: char_letters(abc)
      //       ^^^^^^^^^^^^ ^^^
      //       letter_type  gens
      eat(is, sname());

      // The result.
      set_alphabet res;

      // This labelset might be open: no initial letter is given, they
      // will be discovered afterwards.
      if (is.peek() == '(')
        {
          is.ignore();
          // Previously read character, for intervals.  We don't
          // immediately add the letters: on 'a-z' we would firsts add
          // 'a', and then ask for the interval from 'a' to 'z', which
          // would add 'a' twice uselessly.
          //
          // Rather, keep the 'a' in \a prev, and flush prev when needed.
          boost::optional<letter_t> prev;
          while (true)
            switch (is.peek())
              {
              case EOF:
                raise(sname(), ": make: invalid end-of-file");
                break;

              case ')':
                eat(is, ')');
                goto done;

              case '-':
                if (prev)
                  {
                    eat(is, '-');
                    res.add_range(*prev, L::get_letter(is));
                    prev = boost::none;
                    break;
                  }
                else
                  goto insert;

              insert:
              default:
                {
                  if (prev)
                    res.add_letter(*prev);
                  prev = L::get_letter(is);
                  break;
                }
              }
    done:
          if (prev)
            res.add_letter(*prev);
          ;
        }
      else // is.peek() != '('
        res.open_ = true;
      return res;
    }

    set_alphabet() = default;
    set_alphabet(const set_alphabet&) = default;
    set_alphabet(const std::initializer_list<letter_t>& l)
      : alphabet_{l}
    {}

    set_alphabet(const letters_t& l)
      : alphabet_{l}
    {}

    /// Whether unknown letters should be added, or rejected.
    /// \param o   whether to accept
    /// \returns   the previous status.
    bool open(bool o) const
    {
      std::swap(o, open_);
      return o;
    }

    /// Modify \a this by adding \a l, and return *this.
    set_alphabet&
    add_letter(letter_t l)
    {
      require(l != this->template special<letter_t>(),
              "add_letter: the special letter is reserved: ", l);
      alphabet_.insert(l);
      return *this;
    }

    /// Whether the genset supports the range concept: whether we can
    /// use '++' on letters.
    template <typename Letter, typename Enable = void>
    struct has_range: std::false_type {};

    template <typename Letter>
    struct has_range<Letter,
                     decltype((++std::declval<Letter&>(), void()))>
      : std::true_type
    {};

    /// Add a range of letters, if it is accepted by the labelset.
    auto add_range(letter_t l1, letter_t l2)
      -> set_alphabet&
    {
      return add_range_<letter_t>(l1, l2);
    }

    template <typename Letter>
    auto add_range_(Letter l1, Letter l2)
      -> std::enable_if_t<has_range<Letter>{}, set_alphabet&>
    {
      for (/* empty */; L::less(l1, l2); ++l1)
        add_letter(l1);
      // The last letter.  Do not do this in the loop, we might
      // overflow the capacity of char.  Check validity, so that 'z-a'
      // is empty.
      if (L::equal(l1, l2))
        add_letter(l1);
      return *this;
    }

    template <typename Letter>
    auto add_range_(Letter, Letter)
      -> std::enable_if_t<!has_range<Letter>{}, set_alphabet&>
    {
      raise(sname(), ": does not support letter ranges");
    }

    /// Whether \a l is a letter.
    bool
    has(letter_t l) const
    {
      if (open_)
        {
          // FIXME: OMG...
          const_cast<set_alphabet&>(*this).add_letter(l);
          return true;
        }
      else
        return ::vcsn::has(alphabet_, l);
    }

    /// Extract and return the next word from \a i.
    word_t
    get_word(std::istream& i) const
    {
      require(!i.bad(), "conv: invalid stream");
      // Either an empty word: "\e", or a sequence of non-separators.
      if (i.good() && i.peek() == '\\')
        {
          i.ignore();
          int c = i.peek();
          if (c == 'e')
            {
              i.ignore();
              return {};
            }
          else
            i.unget();
        }

      // Stop as soon as it might be a special character (such as
      // delimiters in polynomials, or tuple separators).
      //
      // The empty word (i.e., an empty stream) is a valid
      // representation of the mpty word.  We want to be able to call
      // `aut.eval("")`, instead of mandating `aut.eval("\e")`.
      word_t res;
      int c = i.peek();
      while (i.good()
             && (c = i.peek()) != EOF
             && !isspace(c)
             && c != '+'
             && c != ','
             && c != '|'
             && c != '('
             && c != ')')
        {
          letter_t l = L::get_letter(i, true);
          VCSN_REQUIRE(has(l), *this, ": invalid letter: ", str_escape(l));
          // FIXME: in-place mul or temporary vector to build the
          // string.
          res = this->mul(res, l);
        }
      return res;
    }

    using iterator = typename letters_t::const_iterator;
    using const_iterator = typename letters_t::const_iterator;

    const_iterator begin() const
    {
      return alphabet_.begin();
    }

    const_iterator end() const
    {
      return alphabet_.end();
    }

    const_iterator cbegin() const
    {
      return alphabet_.begin();
    }

    const_iterator cend() const
    {
      return alphabet_.end();
    }

    const_iterator find(letter_t l) const
    {
      return alphabet_.find(l);
    }

    std::ostream&
    print_set(std::ostream& o, format fmt = {}) const
    {
      switch (fmt.kind())
        {
        case format::latex:
          {
            o << "\\{";
            const char *sep = "";
            for (letter_t l: alphabet_)
              {
                o << sep;
                if (! this->is_letter(l))
                  o << "\\mathit{";
                this->print(l, o, fmt);
                if (! this->is_letter(l))
                  o << '}';
                sep = ", ";
              }
            if (open_)
              o << sep << "\\ldots";
            o << "\\}";
          }
          break;

        case format::sname:
          o << sname() << '(';
          for (letter_t l: alphabet_)
            this->print(l, o, format::sname);
          // FIXME: Don't display openness here, as our "make()"
          // parser is not ready for it.
          o << ')';
          break;

        case format::text:
        case format::utf8:
          o << '{';
          for (letter_t l: alphabet_)
            this->print(l, o, format::sname);
          if (open_)
            o << "...";
          o << '}';
          break;

        case format::raw:
          assert(0);
          break;
        }
      return o;
    }

    /// Compute the intersection with another alphabet.
    friend set_alphabet
    set_intersection(const set_alphabet& lhs, const set_alphabet& rhs)
    {
      return {set_intersection(lhs.alphabet_, rhs.alphabet_)};
    }

    /// Compute the union with another alphabet.
    friend set_alphabet
    set_union(const set_alphabet& lhs, const set_alphabet& rhs)
    {
      return {set_union(lhs.alphabet_, rhs.alphabet_)};
    }

  private:
    // FIXME: OMG...
    mutable letters_t alphabet_;
    mutable bool open_ = false;
  };
}
