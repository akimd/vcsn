#ifndef VCSN_LABELSET_WORDSET_HH
# define VCSN_LABELSET_WORDSET_HH

# include <memory>
# include <set>

# include <boost/algorithm/string/predicate.hpp> // starts_with

# include <vcsn/core/kind.hh>
# include <vcsn/labelset/fwd.hh>
# include <vcsn/labelset/genset-labelset.hh>
# include <vcsn/labelset/labelset.hh>
# include <vcsn/misc/attributes.hh>
# include <vcsn/misc/hash.hh>
# include <vcsn/misc/raise.hh>

namespace vcsn
{
  /// Implementation of labels are words.
  template <typename GenSet>
  class wordset: public detail::genset_labelset<GenSet>
  {
  public:
    using genset_t = GenSet;
    using super_t = detail::genset_labelset<genset_t>;
    using self_type = wordset;
    using genset_ptr = std::shared_ptr<const genset_t>;

    using letter_t = typename genset_t::letter_t;
    using word_t = typename genset_t::word_t;
    using letters_t = std::set<letter_t>;

    using value_t = word_t;

    using kind_t = labels_are_words;

    wordset(const genset_ptr& gs)
      : super_t{gs}
    {}

    wordset(const genset_t& gs = {})
      : wordset{std::make_shared<const genset_t>(gs)}
    {}

    static std::string sname()
    {
      return "wordset<" + super_t::sname() + ">";
    }

    std::string vname(bool full = true) const
    {
      return "wordset<" + super_t::vname(full) + ">";
    }

    /// Build from the description in \a is.
    static wordset make(std::istream& is)
    {
      // name: wordset<char(abc)>.
      //       ^^^^^^^ ^^^^^^^^^
      //      kind   genset
      eat(is, "wordset<");
      auto gs = genset_t::make(is);
      eat(is, ">");
      return gs;
    }

    /// Whether unknown letters should be added, or rejected.
    /// \param o   whether to accept
    /// \returns   the previous status.
    bool open(bool o) const
    {
      return this->genset().open(o);
    }

    static constexpr bool is_free()
    {
      return false;
    }

    /// Value constructor.
    template <typename... Args>
    value_t value(Args&&... args) const
    {
      return value_t{std::forward<Args>(args)...};
    }

    /// Convert to a word.
    word_t word(const value_t& v) const
    {
      return v;
    }

    /// Prepare to iterate over the letters of v.
    static word_t
    letters_of(word_t v)
    {
      return v;
    }

    /// Whether \a l == \a r.
    static bool
    equals(const value_t l, const value_t r)
    {
      return l == r;
    }

    /// Whether \a l < \a r.
    static bool less_than(const value_t l, const value_t r)
    {
      return (std::forward_as_tuple(l.size(), l)
              < std::forward_as_tuple(r.size(), r));
    }

    static value_t
    special()
    {
      return genset_t::template special<value_t>();
    }

    static bool
    is_special(const value_t& v)
    {
      return v == special();
    }

    bool
    is_valid(const value_t& v) const
    {
      for (auto l: v)
        if (!this->has(l))
          return false;
      return true;
    }

    static constexpr bool
    is_ratexpset()
    {
      return false;
    }

    static constexpr bool
    has_one()
    {
      return true;
    }

    static constexpr bool
    is_letterized()
    {
      return false;
    }

    static value_t
    one()
    {
      return genset_t::empty_word();
    }

    static bool
    is_one(const value_t& l) ATTRIBUTE_PURE
    {
      return genset_t::is_empty_word(l);
    }

    static size_t size(const value_t& v)
    {
      return v.length();
    }

    static size_t hash(const value_t& v)
    {
      return hash_value(v);
    }

    static value_t
    conv(self_type, value_t v)
    {
      return v;
    }

    // FIXME: Why do I need to repeat this?
    // It should be inherited from genset-labelset.
    value_t
    conv(std::istream& i) const
    {
      return this->genset().conv(i);
    }

    /// Read a range of labels.
    ///
    /// Stream \a i is right on a '['.  Read up to the closing ']',
    /// and return the list of the matching labels.
    ///
    /// For instance "[a-d0-9_]".
    std::set<value_t>
    convs(std::istream& i) const
    {
      std::set<value_t> res;
      for (auto r : this->convs_(i))
        res.insert(value_t{r});
      return res;
    }

    std::ostream&
    print(const value_t& l, std::ostream& o,
          const std::string& format = "text") const
    {
      if (is_one(l))
        o << (format == "latex" ? "\\varepsilon" : "\\e");
      else if (!is_special(l))
        {
          if (format == "latex" && ! this->is_letter(l))
            o << "\\mathit{";
          o << str_escape(l);
          if (format == "latex" && ! this->is_letter(l))
            o << "}";
        }
      return o;
    }

    std::ostream&
    print_set(std::ostream& o, const std::string& format = "text") const
    {
      if (format == "latex")
        {
          this->genset().print_set(o, format);
          o << "^*";
        }
      else if (format == "text")
        o << vname(true);
      else
        raise("invalid format: ", format);
      return o;
    }

    /// The longest common prefix.
    static value_t lgcd(const value_t& w1, const value_t& w2)
    {
      return {w1.begin(), std::mismatch(w1.begin(), w1.end(), w2.begin()).first};
    }

    /// Compute w1 \ w2 = w1^{-1}w2.
    /// Precondition: w1 is prefix of w2.
    static value_t ldiv(const value_t& w1, const value_t& w2)
    {
      using boost::algorithm::starts_with;
      require(starts_with(w2, w1), "ldiv: invalid arguments: ", str_escape(w1),
              ", ", str_escape(w2));
      return w2.substr(size(w1));
    }

    /// w2 := w1 \ w2.
    static value_t& ldiv_here(const value_t& w1, value_t& w2)
    {
      w2 = ldiv(w1, w2);
      return w2;
    }
  };

  namespace detail
  {
    template <typename GenSet>
    struct law_traits<wordset<GenSet>>
    {
      using type = wordset<GenSet>;
      static type value(const wordset<GenSet>& ls)
      {
        return ls;
      }
    };

    /*-------.
    | Join.  |
    `-------*/

    /// Declare that Lhs v Rhs => Rhs (on the union of alphabets).
#define DEFINE(Lhs, Rhs)                                \
    template <typename GenSet>                          \
    struct join_impl<Lhs, Rhs>                          \
    {                                                   \
      using type = Rhs;                                 \
      static type join(const Lhs& lhs, const Rhs& rhs)  \
      {                                                 \
        return {get_union(lhs.genset(), rhs.genset())}; \
      }                                                 \
    }

    /// The join with another labelset.
    DEFINE(letterset<GenSet>,              wordset<GenSet>);
    DEFINE(nullableset<letterset<GenSet>>, wordset<GenSet>);
    DEFINE(wordset<GenSet>,                wordset<GenSet>);
#undef DEFINE
  }

  /// Compute the meet with another alphabet.
  // FIXME: Factor in genset_labelset?
  template <typename GenSet>
  wordset<GenSet>
  meet(const wordset<GenSet>& lhs, const wordset<GenSet>& rhs)
  {
    return {intersection(lhs.genset(), rhs.genset())};
  }
}

#endif // !VCSN_LABELSET_WORDSET_HH
