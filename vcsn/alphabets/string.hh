#pragma once

#include <cassert>
#include <string>
#include <iostream>

#include <vcsn/misc/escape.hh>
#include <vcsn/misc/format.hh>
#include <vcsn/misc/functional.hh> // vcsn::lexicographical_cmp
#include <vcsn/misc/raise.hh>
#include <vcsn/misc/symbol.hh>

namespace vcsn
{
  /// Represent alphabets whose "letters" are strings.
  ///
  /// This is useful for linguistics where sometimes letters are words
  /// ("it is beautiful" has three letters: "it" "is" and "beautiful")
  /// but also to deal with UTF-8, since graphemes then have various
  /// widths.
  class string_letters
  {
  public:
    using self_t = string_letters;
    /// Internalize the letters to save trees.
    using letter_t = symbol;
    using word_t = std::vector<letter_t>;

    static symbol sname()
    {
      static auto res = symbol{"string_letters"};
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
    mul(const letter_t l, const letter_t r) const
    {
      if (l == one_letter())
        {
          if (r == one_letter())
            return {};
          else
            return {r};
        }
      else if (r == one_letter())
        return {l};
      else
        return {l, r};
    }

    word_t
    mul(const word_t& l, const letter_t r) const
    {
      if (r == one_letter())
        return l;
      else
        {
          word_t res;
          res.reserve(l.size() + 1);
          res.insert(end(res), begin(l), end(l));
          res.insert(end(res), r);
          return res;
        }
    }

    word_t
    mul(const letter_t l, const word_t& r) const
    {
      if (l == one_letter())
        return r;
      else
        {
          word_t res;
          res.reserve(1 + r.size());
          res.insert(end(res), l);
          res.insert(end(res), begin(r), end(r));
          return res;
        }
    }

    word_t
    mul(const word_t& l, const word_t& r) const
    {
      if (is_empty_word(l))
        return r;
      else if (is_empty_word(r))
        return l;
      else
        {
          word_t res;
          res.reserve(l.size() + r.size());
          res.insert(end(res), begin(l), end(l));
          res.insert(end(res), begin(r), end(r));
          return res;
        }
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
      return {begin(w) + 1, begin(w) + (s - 1)};
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

    /// Three-way comparison between two letters.
    static int compare(const letter_t l, const letter_t r)
    {
      return l.get().compare(r.get());
    }

    /// Three-way comparison between two words.
    static int compare(const word_t& l, const word_t& r)
    {
      return lexicographical_cmp(l, r,
                                 vcsn::detail::compare<self_t, letter_t>{});
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
      return l < r;
    }

    /// Whether \a l < \a r.
    static bool less(const word_t& l, const word_t& r)
    {
      return l < r;
    }

    bool
    is_letter(const letter_t) const
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
    static letter_t one_letter() { return letter_t(""); }

  private:
    /// The reserved letter used to forge the labels for initial and
    /// final transitions.
    ///
    /// Use the public special() interface.
    static letter_t special_letter() { return letter_t{std::string{char(0)}}; }

  public:
    /// Read one letter from i.
    ///
    /// Either a single char, or a "letter" enclosed in single-quotes.
    static letter_t get_letter(std::istream& i, bool quoted = true)
    {
      std::string res;
      if (quoted)
        {
          int c = i.peek();
          if (c == '\'')
            {
              i.ignore();
              while (true)
                {
                  c = i.peek();
                  if (c == EOF)
                    raise(sname(), ": get_letter: invalid end-of-file");
                  else if (c == '\'')
                    {
                      i.ignore();
                      break;
                    }
                  else
                    res += get_char(i);
                }
            }
          else
            res = std::string{get_char(i)};
        }
      else
        res = std::string{std::istreambuf_iterator<char>(i), {}};
      return letter_t{res};
    }

    std::ostream&
    print(const letter_t l, std::ostream& o, const format& fmt = {}) const
    {
      if (l == one_letter() || l == special_letter())
        {}
      else
        switch (fmt.kind())
          {
          case format::latex:
            o << "`\\mathit{" << l << "}\\textrm{'}";
            break;

          case format::ere:
          case format::redgrep:
          case format::sname:
          case format::text:
          case format::utf8:
            if (l.get().size() == 1)
              str_escape(o, l, "|',[-]<>");
            else
              o << '\'' << l << '\'';
            break;

          raw:
          case format::raw:
            o << l;
            break;
          }
      return o;
    }

    std::ostream&
    print(const word_t& w, std::ostream& o, const format& fmt = {}) const
    {
      for (auto l: w)
        print(l, o, fmt);
      return o;
    }

    /// Special character, used to label transitions from pre() and to
    /// post().
    template <typename T = letter_t>
    static T special();
  };

  template <>
  inline
  string_letters::letter_t
  string_letters::special<string_letters::letter_t>()
  {
    return special_letter();
  }

  template <>
  inline
  string_letters::word_t
  string_letters::special<string_letters::word_t>()
  {
    return {special_letter()};
  }
}
