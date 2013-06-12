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
    /// Implementation of labels are words.
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

      const super_type&
      super() const
      {
        return static_cast<const super_type&>(*this);
      }

      static std::string sname()
      {
        return "law_" + super_type::sname();
      }

      std::string vname(bool full = true) const
      {
        return "law_" + super().vname(full);
      }

      label_t
      special() const
      {
        return this->genset()->template special<label_t>();
      }

      bool
      is_special(const label_t& v) const
      {
        return v == special();
      }

      bool
      is_valid(const label_t& v) const
      {
        for (auto l: v)
          if (!this->has(l))
            return false;
        return true;
      }

      label_t
      identity() const
      {
        return this->empty_word();
      }

      bool
      is_identity(const label_t& l) const
      {
        return this->is_empty_word(l);
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
