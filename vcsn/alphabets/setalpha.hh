#ifndef VCSN_ALPHABETS_SETALPHA_HH
# define VCSN_ALPHABETS_SETALPHA_HH

# include <initializer_list>
# include <set>

namespace vcsn
{
  template <class L>
  class set_alphabet: public L
  {
  public:
    using letter_t = typename L::letter_t;
    using letters_t = std::set<letter_t>;

    static std::string sname()
    {
      return "char";
    }

    virtual std::string vname(bool full = true) const
    {
      std::string res = sname();
      if (full && !alphabet_.empty())
        {
          res += '(';
          for (letter_t c: alphabet_)
            res += c;
          res += ')';
        }
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

    set_alphabet&
    add_letter(letter_t l)
    {
      assert(l != alphabet_.special_letter());
      alphabet_.insert(l);
      return *this;
    }

    /// Whether \a l is a letter.
    bool
    has(letter_t l) const
    {
      return alphabet_.find(l) != alphabet_.end();
    }

    using iterator_t = typename letters_t::const_iterator;
    iterator_t begin() const
    {
      return alphabet_.begin();
    }

    iterator_t end() const
    {
      return alphabet_.end();
    }

  private:
    letters_t alphabet_;
  };
}

#endif // !VCSN_ALPHABETS_SETALPHA_HH
