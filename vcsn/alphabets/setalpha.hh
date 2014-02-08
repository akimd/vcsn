#ifndef VCSN_ALPHABETS_SETALPHA_HH
# define VCSN_ALPHABETS_SETALPHA_HH

# include <initializer_list>
# include <stdexcept>

# include <vcsn/misc/raise.hh>
# include <vcsn/misc/set.hh>
# include <vcsn/misc/stream.hh> // eat.

namespace vcsn
{
  template <typename L>
  class set_alphabet: public L
  {
  public:
    using letter_t = typename L::letter_t;
    using word_t = typename L::word_t;
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

    static set_alphabet make(std::istream& is)
    {
      // name: char(abc)_ratexpset<law_char(xyz)_b>.
      //       ^^^^ ^^^  ^^^^^^^^^^^^^^^^^^^^^^^^^
      //        |    |        weightset
      //        |    +-- gens
      //        +-- letter_type
      std::string letter_type;
      {
        char c;
        while (is >> c)
          {
            if (c == '(')
              {
                is.unget();
                break;
              }
            letter_type.append(1, c);
          }
      }
      // The list of generators (letters).
      letters_t gens;
      // Previously read character.
      int prev = -1;
      {
        eat(is, '(');
        char l;
        while (is >> l && l != ')')
          switch (l)
            {
            case '-':
              if (prev == -1)
                goto insert;
              else
                {
                  int l2 = is.get();
                  require(l2 != EOF,
                          "unexpected end in character class");
                  if (l2 == '\\')
                    {
                      l2 = is.get();
                      require(l2 != EOF,
                              "unexpected end after escape in character class");
                    }
                  for (l = prev; l <= l2; ++l)
                    gens.insert(l);
                  prev = -1;
                  continue;
                }

            case '\\':
              {
                int l2 = is.get();
                require(l2 != EOF,
                        "unexpected end after escape");
                l = l2;
                goto insert;
              }

            default:
            insert:
              gens.insert(l);
              prev = l;
          }
      }
      return gens;
    }

    set_alphabet() = default;
    set_alphabet(const set_alphabet&) = default;
    set_alphabet(const std::initializer_list<letter_t>& l)
      : alphabet_{l}
    {}

    set_alphabet(const letters_t& l)
      : alphabet_{l}
    {}

    /// Modify \a this by adding \a l, and return *this.
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
      return ::vcsn::has(alphabet_, l);
    }

    /// Extract and return the next word from \a i.
    word_t
    conv(std::istream& i) const
    {
      word_t res;
      if (i.peek() == '\\')
        {
          i.ignore();
          char c = i.peek();
          if (c != 'e')
            throw std::domain_error("invalid label: unexpected \\"
                                    + std::string{c});
          i.ignore();
        }
      else
        // FIXME: This wrongly assumes that letters are characters.
        // It will break with integer or string alphabets.
        while (has(i.peek()))
          res = this->concat(res, letter_t(i.get()));
      return res;
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
    letters_t alphabet_;
  };

  template <typename L>
  inline
  std::ostream&
  print_set(const set_alphabet<L>& alphabet,
            std::ostream& o, const std::string& format)
  {
    if (format == "latex")
      {
	const char *sep = "\\{";
	for (auto c: alphabet)
	  {
	    o << sep << c;
	    sep = ", ";
	  }
	o << "\\}";
      }
    else if (format == "text")
      o << alphabet.vname(true);
    else
      raise("invalid format: ", format);
    return o;
  }

}

#endif // !VCSN_ALPHABETS_SETALPHA_HH
