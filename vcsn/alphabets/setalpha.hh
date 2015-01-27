#pragma once

#include <cassert>
#include <initializer_list>
#include <stdexcept>

#include <boost/optional.hpp>

#include <vcsn/misc/type_traits.hh>
#include <vcsn/misc/raise.hh>
#include <vcsn/misc/set.hh>
#include <vcsn/misc/stream.hh> // eat.
#include <vcsn/misc/symbol.hh>

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
    using letters_t = std::set<letter_t>;
    /// The type of our values, when seen as a container.
    using value_type = letter_t;

    static symbol sname()
    {
      static symbol res(L::sname());
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
          // Previously read character, for intervals.
          boost::optional<letter_t> prev;
          bool done = false;
          while (!done)
            switch (is.peek())
              {
              case EOF:
                raise(sname(), ": make: invalid end-of-file");
                break;

              case ')':
                eat(is, ')');
                done = true;
                break;

              case '-':
                if (prev == boost::none)
                  goto insert;
                else
                  {
                    eat(is, '-');
                    res.add_range(prev.get(), L::get_letter(is));
                    prev = boost::none;
                    break;
                  }

              insert:
              default:
                {
                  prev = L::get_letter(is);
                  res.add_letter(prev.get());
                  break;
                }
              }
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
      assert(l != this->template special<letter_t>());
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
      -> enable_if_t<has_range<Letter>{}, set_alphabet&>
    {
      for (/* empty */; l1 <= l2; ++l1)
        add_letter(l1);
      return *this;
    }

    template <typename Letter>
    auto add_range_(Letter, Letter)
      -> enable_if_t<!has_range<Letter>{}, set_alphabet&>
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
      word_t res;
      require(!i.bad(),
              "conv: invalid stream");
      // Either an empty word: "\e", or a sequence of non-separators.
      if (i.good() && i.peek() == '\\')
        {
          i.ignore();
          int c = i.get();
          require(c == 'e',
                  "invalid label: unexpected \\", str_escape(c));
        }
      else
        {
          // Stop as soon as it might be a special character (such as
          // delimiters in polynomials).
          int c;
          while (i.good()
                 && (c = i.peek()) != EOF
                 && !isspace(c)
                 && c != '+'
                 && c != ','
                 && c != '('
                 && c != ')')
            {
              letter_t l = L::get_letter(i);
              require(has(l),
                      sname(), ": invalid letter: ", str_escape(l));
              res = this->concat(res, l);
            }
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
    print_set(std::ostream& o, const std::string& format = "text") const
    {
      if (format == "latex")
        {
          o << "\\{";
          const char *sep = "";
          for (letter_t l: alphabet_)
            {
              o << sep;
              if (! this->is_letter(l))
                o << "\\mathit{";
              this->print(l, o, format);
              if (! this->is_letter(l))
                o << '}';
              sep = ", ";
            }
          if (open_)
            o << sep << "\\ldots";
          o << "\\}";
        }
      else if (format == "text")
        {
          o << sname() << '(';
          for (letter_t l: alphabet_)
            // FIXME: escape ')' and '-'.
            this->print(l, o, format);
          // Don't display openness here, as our "make()" parser is
          // not ready for it.
          o << ')';
        }
      else
        raise(sname(), ": print_set: invalid format: ", format);
      return o;
    }

    /// Compute the intersection with another alphabet.
    template <typename L2>
    friend set_alphabet<L2>
    intersection(const set_alphabet<L2>& lhs, const set_alphabet<L2>& rhs)
    {
      return {intersection(lhs.alphabet_, rhs.alphabet_)};
    }

    /// Compute the union with another alphabet.
    template <typename L2>
    friend set_alphabet<L2>
    get_union(const set_alphabet<L2>& lhs, const set_alphabet<L2>& rhs)
    {
      return {get_union(lhs.alphabet_, rhs.alphabet_)};
    }

  private:
    // FIXME: OMG...
    mutable letters_t alphabet_;
    mutable bool open_ = false;
  };
}
