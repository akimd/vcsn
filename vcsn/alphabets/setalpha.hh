#ifndef VCSN_ALPHABETS_SETALPHA_HH
# define VCSN_ALPHABETS_SETALPHA_HH

#include <initializer_list>
#include <set>

namespace vcsn
{
  template <class L>
  class set_alphabet: public L
  {
  private:
    typedef std::set<typename L::letter_t> genset_t;
    genset_t alphabet_;

  public:
    set_alphabet() = default;
    set_alphabet(const std::initializer_list<typename L::letter_t>& l)
      : alphabet_(l)
    {}

    set_alphabet&
    add_letter(typename L::letter_t l)
    {
      assert(l != alphabet_.special_letter());
      alphabet_.insert(l);
      return *this;
    }

    /// Whether \a l is a letter.
    bool
    has(typename L::letter_t l) const
    {
      return alphabet_.find(l) != alphabet_.end();
    }

    typedef typename genset_t::const_iterator iterator_t;
    iterator_t begin() const
    {
      return alphabet_.begin();
    }

    iterator_t end() const
    {
      return alphabet_.end();
    }
  };
}

#endif // !VCSN_ALPHABETS_SETALPHA_HH
