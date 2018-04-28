#pragma once

#include <cassert>
#include <initializer_list>
#include <optional>
#include <stdexcept>

#include <boost/container/flat_set.hpp>
#include <boost/range/iterator_range_core.hpp>
#include <boost/version.hpp>

#include <vcsn/misc/escape.hh>
#include <vcsn/misc/format.hh>
#include <vcsn/misc/raise.hh>
#include <vcsn/misc/set.hh>
#include <vcsn/misc/stream.hh> // eat.
#include <vcsn/misc/symbol.hh>
#include <vcsn/misc/type_traits.hh>

namespace vcsn
{
  /// Whether \a e is member of \a s.
  template <typename Key, typename Compare, typename Allocator>
  ATTRIBUTE_PURE
  bool
  has(const boost::container::flat_set<Key, Compare, Allocator>& s,
      const Key& e)
  {
    return s.find(e) != s.end();
  }

  /// A set of letters of type \a L.
  ///
  /// \a L is not simply `char` or so.  Rather, see char_letters.
  template <typename L>
  class set_alphabet: public L
  {
  public:
    using letter_t = typename L::letter_t;
    using word_t = typename L::word_t;
    using letters_t
      = boost::container::flat_set<letter_t, vcsn::less<L, letter_t>>;
    /// The type of our values, when seen as a container.
    using value_type = letter_t;

    static symbol sname()
    {
      static auto res = L::sname();
      return res;
    }

    /// Build from a specification in this stream.
    static set_alphabet make(std::istream& is)
    {
      // name: char_letters(abc)
      //       ^^^^^^^^^^^^ ^^^
      //       letter_type  gens
      eat(is, sname());

      // The result.
      auto res = set_alphabet{};

      // No parens: empty open set.  With parens explicit letters,
      // possibly ranges, and `...` means open.
      if (is.peek() == '(')
        {
          is.ignore();
          // Previously read character, for intervals.  We don't
          // immediately add the letters: on 'a-z' we would first add
          // 'a', and then ask for the interval from 'a' to 'z', which
          // would add 'a' twice uselessly.
          //
          // Rather, keep the 'a' in \a prev, and flush prev when needed.
          auto prev = std::optional<letter_t>{};
          // The number of consecutive dots we saw.  Used to recognize
          // `...` which means that the set is open.
          int dots = 0;
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
                dots = 0;
                if (prev)
                  {
                    is.ignore();
                    res.add_range(*prev, L::get_letter(is));
                    prev = std::nullopt;
                  }
                else
                  goto insert;
                break;

              case '.':
                ++dots;
                if (dots == 1 && prev)
                  res.add_letter(*prev);
                prev = L::get_letter(is);
                if (dots == 3)
                  {
                    res.open_ = true;
                    prev = std::nullopt;
                    dots = 0;
                  }
                break;

              insert:
              default:
                if (prev)
                  res.add_letter(*prev);
                prev = L::get_letter(is);
                dots = 0;
                break;
              }
    done:
          if (prev)
            res.add_letter(*prev);
        }
      else // is.peek() != '('
        res.open_ = true;
      return res;
    }

    set_alphabet()
    {
      alphabet_.insert(L::one_letter());
    }
    set_alphabet(const set_alphabet&) = default;
    set_alphabet(std::initializer_list<letter_t> l)
#if 105700 <= BOOST_VERSION
      : alphabet_{l}
#else
      : alphabet_{l.begin(), l.end()}
#endif
    {
      alphabet_.insert(L::one_letter());
    }
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
      VCSN_REQUIRE(l != this->template special<letter_t>(),
                   *this,
                   ": add_letter: the special letter is reserved: ",
                   str_escape(l));
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
      raise(*this, ": does not support letter ranges");
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
      require(!i.bad(), *this, ": conv: invalid stream");
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
      // `aut.evaluate("")`, instead of mandating `aut.evaluate("\e")`.
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

    const_iterator cbegin() const
    {
      return alphabet_.begin() + 1;
    }

    const_iterator cend() const
    {
      return alphabet_.end();
    }

    const_iterator begin() const
    {
      return cbegin();
    }

    const_iterator end() const
    {
      return cend();
    }

    /// All the "pregenerators", including the empty word.
    auto pregenerators() const
    {
      return alphabet_;
    }

    /// All the generators.
    auto generators() const
    {
      return boost::make_iterator_range(cbegin(), cend());
    }

    /// Whether this alphabet has no letters.
    bool empty() const
    {
      return cbegin() == cend();
    }

    /// Number of letters.
    size_t size() const
    {
      return alphabet_.size();
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
            for (const auto l: *this)
              {
                o << sep;
                if (! this->is_letter(l))
                  o << "\\mathit{";
                this->print(l, o, fmt);
                if (! this->is_letter(l))
                  o << '}';
                sep = ", ";
              }
            if (open_ && fmt.show_open())
              o << sep << "\\ldots";
            o << "\\}";
          }
          break;

        case format::sname:
          o << sname();
          if (!empty() || !open_)
            {
              o << '(';
              for (const auto l: *this)
                this->print(l, o, fmt);
              if (open_ && fmt.show_open())
                o << "...";
              o << ')';
            }
          break;

        case format::text:
        case format::utf8:
          o << '[';
          for (const auto l: *this)
            this->print(l, o, fmt);
          if (open_ && fmt.show_open())
            o << "...";
          o << ']';
          break;

        case format::ere:
        case format::raw:
        case format::redgrep:
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
