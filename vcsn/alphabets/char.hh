#ifndef VCSN_ALPHABETS_CHAR_HH
#define VCSN_ALPHABETS_CHAR_HH

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
      return word_t(1, l);
    }

    const word_t&
    to_word(const word_t& l) const
    {
      return l;
    }

    word_t
    concat(const letter_t l, const letter_t r) const
    {
      word_t w(l, 2);
      w[1] = r;
      return w;
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
      return std::string();
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

    std::ostream&
    output(std::ostream& o, const letter_t& w) const
    {
      return o << w;
    }

    std::ostream&
    output(std::ostream& o, const word_t& w) const
    {
      return o << format(w);
    }

    const std::string
    format(const letter_t w) const
    {
      return std::string(1, w);
    }

    const std::string&
    format(const word_t& w) const
    {
      static std::string emptyword("\\e");
      return w.empty() ? emptyword : w;
    }

    // word_t mirror(const word_t& w)

  };
}

#endif // VCSN_ALPHABETS_CHAR_HH
