#ifndef VCSN_CTX_WORDSET_HH
# define VCSN_CTX_WORDSET_HH

# include <memory>
# include <set>

# include <vcsn/core/kind.hh>
# include <vcsn/ctx/genset-labelset.hh>

namespace vcsn
{
  namespace ctx
  {
    template <typename GenSet>
    struct wordset: genset_labelset<GenSet>
    {
      using genset_t = GenSet;
      using super_type = genset_labelset<genset_t>;
      using genset_ptr = std::shared_ptr<const genset_t>;

      using label_t = typename genset_t::word_t;
      using letter_t = typename genset_t::letter_t;
      using word_t = typename genset_t::word_t;
      using letters_t = std::set<letter_t>;

      using kind_t = labels_are_words;

      wordset(const genset_ptr& gs)
        : super_type{gs}
      {}

      wordset(const genset_t& gs = {})
        : wordset{std::make_shared<const genset_t>(gs)}
      {}

      static std::string sname()
      {
        return "law_" + genset_t::sname();
      }

      std::string vname(bool full = true) const
      {
        return "law_" + this->genset()->vname(full);
      }

      label_t
      special() const
      {
        return {this->genset()->special_letter()};
      }
    };

    /// Compute the intersection with another alphabet.
    // FIXME: Factor in genset_labelset?
    template <typename GenSet>
    wordset<GenSet>
    intersect(const wordset<GenSet>& lhs, const wordset<GenSet>& rhs)
    {
      return {intersect(lhs->genset(), rhs->genset())};
    }

  }
}

#endif // !VCSN_CTX_WORDSET_HH
