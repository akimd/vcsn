#ifndef VCSN_ALPHABETS_CHAR_HH
# define VCSN_ALPHABETS_CHAR_HH

# include <cassert>
# include <cstring> // strchr
# include <string>
# include <iostream>

# include <vcsn/misc/escape.hh>
# include <vcsn/misc/raise.hh>
# include <vcsn/misc/symbol.hh>

namespace vcsn
{
  class char_letters
  {
  public:
    using letter_t = char;
    using word_t = std::string;

    static symbol sname()
    {
      static symbol res("char_letters");
      return res;
    }

    word_t
    to_word(const letter_t l) const
    {
      return {l};
    }

    const word_t&
    to_word(const word_t& l) const
    {
      return l;
    }

    word_t
    concat(const letter_t l, const letter_t r) const
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

    word_t
    concat(const word_t& l, const letter_t r) const
    {
      return r == one_letter() ? l : l + r;
    }

    word_t
    concat(const letter_t l, const word_t& r) const
    {
      return l == one_letter() ? r : l + r;
    }

    word_t
    concat(const word_t& l, const word_t& r) const
    {
      return l + r;
    }

    /// Add the special character first and last.
    word_t delimit(const word_t& w) const
    {
      return concat(concat(special_letter(), w), special_letter());
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

    static word_t
    empty_word()
    {
      return {};
    }

    static bool
    is_empty_word(const word_t& w)
    {
      return w.empty();
    }

    word_t
    transpose(const word_t& w) const
    {
      // C++11 lacks std::rbegin/rend...
      return {w.rbegin(), w.rend()};
    }

    letter_t
    transpose(letter_t l) const
    {
      return l;
    }

    bool
    equals(const letter_t& l1, const letter_t& l2) const
    {
      return l1 == l2;
    }

    bool
    equals(const word_t& w1, const word_t& w2) const
    {
      return w1 == w2;
    }

    bool
    is_letter(const letter_t&) const
    {
      return true;
    }

    bool
    is_letter(const word_t& w) const
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
    static constexpr letter_t special_letter() { return 127; }

  public:
    /// Read a single char, with possible \-escape support.
    /// EOF is an error.
    static char get_char(std::istream& i)
    {
      int res = i.get();
      if (res == '\\')
        {
          int c = i.peek();
          // \(, \) and \- are used in setalpha::make, e.g.,
          // char_letters(\(\-\)).
          if (strchr("\"\\()-", c))
            res = i.get();
        }
      require(res != EOF,
              sname(), ": conv: invalid label: unexpected end-of-file");
      return res;
    }


    /// Read one letter from i.
    ///
    /// Either a single char, or a "letter" enclosed in single-quotes.
    static letter_t get_letter(std::istream& i)
    {
      return get_char(i);
    }

    std::ostream&
    print(const letter_t& l, std::ostream& o,
          const std::string& format = "text") const
    {
      if (l == '\\')
        o << (format == "latex" ? "\\backslash{}" : "\\\\");
      else if (l != one_letter() && l != special_letter())
        o << l;
      return o;
    }

    std::ostream&
    print(const word_t& w, std::ostream& o,
          const std::string& format = "text") const
    {
      size_t s = w.size();

      if (s == 0
          || (s == 1 && w[0] == one_letter()))
        o << "\\e";
      else if (s == 1 && w[0] == special_letter())
        o << '$';
      else
        {
          if (format == "latex")
            o << "\\mathit{";
          o << str_escape(w);
          if (format == "latex")
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

#endif // !VCSN_ALPHABETS_CHAR_HH
