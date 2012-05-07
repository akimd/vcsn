#ifndef VCSN_ALPHABETS_CHAR_HH
# define VCSN_ALPHABETS_CHAR_HH

#include <string>
#include <iostream>

namespace vcsn
{
  class char_letters
  {
  public:
    typedef char letter_t;
    typedef std::string word_t;

    word_t
    to_word(const letter_t l) const
    {
      return word_t{l};
    }

    const word_t&
    to_word(const word_t& l) const
    {
      return l;
    }

    word_t
    concat(const letter_t l, const letter_t r) const
    {
      return word_t{l, r};
    }

    word_t
    concat(const word_t l, const letter_t r) const
    {
      return l + r;
    }

    word_t
    concat(const letter_t l, const word_t r) const
    {
      return l + r;
    }

    word_t
    concat(const word_t l, const word_t r) const
    {
      return l + r;
    }

    word_t
    identity() const
    {
      return word_t{};
    }

    bool
    is_identity(const word_t& w) const
    {
      return w.empty();
    }

    bool
    equals(const word_t& w1, const word_t& w2) const
    {
      return w1 == w2;
    }

    bool
    equals(const letter_t& l1, const letter_t& l2) const
    {
      return l1 == l2;
    }

  protected:
    letter_t special_letter() const { return 255; }

  public:
    std::ostream&
    output(std::ostream& o, const letter_t& l) const
    {
      if (l != special_letter())
	return o << l;
    }

    std::ostream&
    output(std::ostream& o, const word_t& w) const
    {
      return o << format(w);
    }

    const std::string
    format(const letter_t l) const
    {
      if (l != special_letter())
	return std::string{l};
      else
	return std::string();
    }

    inline
    const std::string
    format(const word_t& w) const
    {
      size_t s = w.size();

      if (s == 0)
	return "\\e";

      // If the string starts or ends with the special
      // letter, just skip it.  If the resulting string
      // is empty, just format it this way.  (We DON'T
      // wan't to format it as "\\e".)
      if (w[0] == special_letter())
	return (s == 1) ? "" : w.substr(1);

      if (s > 1 && w[s - 1] == special_letter())
	return w.substr(0, s - 1);

      return w;
    }

    // Special character, used to label transitions
    // from pre() and post()
    template<class T = letter_t>
    inline
    T special() const;

    // word_t mirror(const word_t& w)
  };

  template<>
  inline
  char_letters::letter_t
  char_letters::special<char_letters::letter_t>() const
  {
    return special_letter();
  }

  template<>
  inline
  char_letters::word_t
  char_letters::special<char_letters::word_t>() const
  {
    return word_t(1, special_letter());
  }

}

#endif // !VCSN_ALPHABETS_CHAR_HH
