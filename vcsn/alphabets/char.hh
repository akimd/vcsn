#pragma once

#include <cassert>
#include <string>
#include <iostream>

#include <vcsn/misc/escape.hh>
#include <vcsn/misc/format.hh>
#include <vcsn/misc/raise.hh>
#include <vcsn/misc/stream.hh>
#include <vcsn/misc/symbol.hh>

namespace vcsn
{
  /// Represent alphabets whose "letters" are plain chars.
  class char_letters
  {
  public:
    using letter_t = char;
    using word_t = std::string;

    static symbol sname()
    {
      static auto res = symbol{"char_letters"};
      return res;
    }

    /// Convert to word.
    word_t
    to_word(const letter_t l) const
    {
      return {l};
    }

    /// Convert to word.
    const word_t&
    to_word(const word_t& l) const
    {
      return l;
    }

    /// Concatenation.
    word_t
    mul(const letter_t l, const letter_t r) const
    {
      if (l == one_letter())
        {
          if (r == one_letter())
            return {};
          else
            return {l};
        }
      else if (r == one_letter())
        return {l};
      else
        return {l, r};
    }

    /// Concatenation.
    word_t
    mul(const word_t& l, const letter_t r) const
    {
      return r == one_letter() ? l : l + r;
    }

    /// Concatenation.
    word_t
    mul(const letter_t l, const word_t& r) const
    {
      return l == one_letter() ? r : l + r;
    }

    /// Concatenation.
    word_t
    mul(const word_t& l, const word_t& r) const
    {
      return l + r;
    }

    /// Add the special character first and last.
    word_t delimit(const word_t& w) const
    {
      return mul(mul(special_letter(), w), special_letter());
    }

    /// Remove first and last characters, that must be "special".
    word_t undelimit(const word_t& w) const
    {
      size_t s = w.size();
      assert(2 <= s);
      assert(w[0] == special_letter());
      assert(w[s-1] == special_letter());
      return w.substr(1, s-2);
    }

    /// One.
    static word_t
    empty_word()
    {
      return {};
    }

    /// Whether is one.
    static bool
    is_empty_word(const word_t& w)
    {
      return w.empty();
    }

    /// Mirror label.
    word_t
    transpose(const word_t& w) const
    {
      // C++11 lacks std::rbegin/rend...
      return {w.rbegin(), w.rend()};
    }

    /// Mirror label.
    letter_t
    transpose(letter_t l) const
    {
      return l;
    }

    /// Three-way comparison between two letters.
    static int compare(const letter_t l, const letter_t r)
    {
      return int(l) - int(r);
    }

    /// Three-way comparison between two words.
    static int compare(const word_t& l, const word_t& r)
    {
      return l.compare(r);
    }

    /// Whether \a l == \a r.
    static bool equal(const letter_t l, const letter_t r)
    {
      return l == r;
    }

    /// Whether \a l == \a r.
    static bool equal(const word_t& l, const word_t& r)
    {
      return l == r;
    }

    /// Whether \a l < \a r.
    static bool less(const letter_t l, const letter_t r)
    {
      // Be sure to convert the whole 8-bit spectrum.
      return uint8_t(l) < uint8_t(r);
    }

    /// Whether \a l < \a r.
    static bool less(const word_t& l, const word_t& r)
    {
      // FIXME: do we need an unsigned comparison?
      return l < r;
    }

    /// Whether is a letter.
    bool is_letter(const letter_t) const
    {
      return true;
    }

    /// Whether is a single-letter word.
    bool is_letter(const word_t& w) const
    {
      return w.size() == 1;
    }

    /// The reserved letter used to forge the "one" label (the unit,
    /// the identity).
    static constexpr letter_t one_letter() { return 0; }

  private:
    /// The reserved letter used to forge the labels for initial and
    /// final transitions.
    ///
    /// Use the public special() interface.
    ///
    /// The value is chosen to put subliminal transitions last when
    /// sorted.  Requires unsigned comparison.
    static constexpr letter_t special_letter() { return -1; }

  public:
    /// Read one letter from i.
    ///
    /// Either a single char, or a "letter" enclosed in single-quotes.
    static letter_t get_letter(std::istream& i, bool quoted = true)
    {
      letter_t res = i.peek();
      if (quoted && res == '\'')
        {
          eat(i, '\'');
          res = get_char(i);
          eat(i, '\'');
        }
      else
        res = get_char(i);
      return res;
    }

    /// Print a letter.
    std::ostream&
    print(const letter_t l, std::ostream& o, format fmt = {}) const
    {
      if (l == one_letter() || l == special_letter())
        {}
      else
        switch (fmt.kind())
          {
          case format::sname:
            // Escape dash and parens, which are special when parsed
            // (actually, only the closing paren is special, but treating
            // them symmetrically is better looking).
            //
            // Of course, escape single-quote and backslash, which are
            // used to escape.
            str_escape(o, l, "(-)\\'");
            break;

          case format::latex:
            if (l == '\\')
              o << "\\backslash{}";
            else
              str_escape(o, l, "#{}");
            break;

          case format::raw:
            o << l;
            break;

          case format::text:
          case format::utf8:
            if (l == '\\')
              o << "\\\\";
            else
              str_escape(o, l, fmt.meta() ? fmt.meta() : "|',[-]<> ");
          }
      return o;
    }

    /// Print a word.
    std::ostream&
    print(const word_t& w, std::ostream& o, format fmt = {}) const
    {
      size_t s = w.size();

      if (s == 0
          || (s == 1 && w[0] == one_letter()))
        o << "\\e";
      else if (s == 1 && w[0] == special_letter())
        o << '$';
      else if (fmt == format::raw)
        o << w;
      else
        {
          if (fmt == format::latex)
            o << "\\mathit{";
          for (auto c: w)
            print(c, o, fmt);
          if (fmt == format::latex)
            o << '}';
        }
      return o;
    }

    /// Special character, used to label transitions from pre() and to
    /// post().
    template <typename T = letter_t>
    static T special();
  };

  template <>
  inline
  char_letters::letter_t
  char_letters::special<char_letters::letter_t>()
  {
    return special_letter();
  }

  template <>
  inline
  char_letters::word_t
  char_letters::special<char_letters::word_t>()
  {
    return {special_letter()};
  }

}
