#ifndef VCSN_ALPHABETS_CHAR_HH
#define VCSN_ALPHABETS_CHAR_HH

#include <string>

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
      return word_t(l, 1);
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

    // word_t mirror(const word_t& w)

  };
}

#endif // VCSN_ALPHABETS_CHAR_HH
