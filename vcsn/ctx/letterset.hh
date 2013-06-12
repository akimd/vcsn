#ifndef VCSN_CTX_LETTERSET_HH
# define VCSN_CTX_LETTERSET_HH

# include <memory>
# include <set>

# include <vcsn/alphabets/setalpha.hh> // intersect
# include <vcsn/core/kind.hh>
# include <vcsn/ctx/genset-labelset.hh>

namespace vcsn
{
  namespace ctx
  {
    /// Implementation of labels are letters.
    template <typename GenSet>
    struct letterset: genset_labelset<GenSet>
    {
      using genset_t = GenSet;
      using super_type = genset_labelset<genset_t>;
      using genset_ptr = std::shared_ptr<const genset_t>;

      using label_t = typename genset_t::letter_t;
      using letter_t = typename genset_t::letter_t;
      using word_t = typename genset_t::word_t;
      using letters_t = std::set<letter_t>;

      using kind_t = labels_are_letters;

      letterset(const genset_ptr& gs)
        : super_type{gs}
      {}

      letterset(const genset_t& gs = {})
        : letterset{std::make_shared<const genset_t>(gs)}
      {}

      const super_type&
      super() const
      {
        return static_cast<const super_type&>(*this);
      }

      static std::string sname()
      {
        return "lal_" + super_type::sname();
      }

      std::string vname(bool full = true) const
      {
        return "lal_" + super().vname(full);
      }

      label_t
      special() const
      {
        return this->genset()->template special<label_t>();
      }

      bool
      is_special(label_t v) const
      {
        return v == special();
      }

      static constexpr bool
      is_identity(label_t)
      {
        return false;
      }

      bool
      is_valid(label_t v) const
      {
        return this->has(v);
      }
    };

    /// Compute the intersection with another alphabet.
    template <typename GenSet>
    letterset<GenSet>
    intersect(const letterset<GenSet>& lhs, const letterset<GenSet>& rhs)
    {
      return {intersect(*lhs.genset(), *rhs.genset())};
    }

  }
}

#endif // !VCSN_CTX_LETTERSET_HH
