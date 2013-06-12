#ifndef VCSN_CTX_NULLABLESET_HH
# define VCSN_CTX_NULLABLESET_HH

# include <memory>
# include <set>
# include <sstream>

# include <vcsn/alphabets/setalpha.hh> // intersect
# include <vcsn/core/kind.hh>
# include <vcsn/ctx/genset-labelset.hh>

namespace vcsn
{
  namespace ctx
  {
    /// Implementation of labels are nullables (letter or empty).
    template <typename GenSet>
    struct nullableset: genset_labelset<GenSet>
    {
      using genset_t = GenSet;
      using super_type = genset_labelset<genset_t>;
      using genset_ptr = std::shared_ptr<const genset_t>;

      using label_t = typename genset_t::letter_t;
      using letter_t = typename genset_t::letter_t;
      using word_t = typename genset_t::word_t;
      using letters_t = std::set<letter_t>;

      using kind_t = labels_are_nullable;

      nullableset(const genset_ptr& gs)
        : super_type{gs}
      {}

      nullableset(const genset_t& gs = {})
        : nullableset{std::make_shared<const genset_t>(gs)}
      {}

      const super_type&
      super() const
      {
        return static_cast<const super_type&>(*this);
      }

      static std::string sname()
      {
        return "lan_" + super_type::sname();
      }

      std::string vname(bool full = true) const
      {
        return "lan_" + super().vname(full);
      }

      constexpr label_t
      special() const
      {
        return this->genset()->template special<label_t>();
      }

      constexpr label_t
      identity() const
      {
        return this->genset()->identity_letter();
      }

      bool
      is_identity(label_t l) const
      {
        return l == identity();
      }

      bool
      is_valid(label_t v) const
      {
        return this->has(v) || is_identity(v);
      }

      std::ostream&
      print(std::ostream& o, label_t l) const
      {
        if (is_identity(l))
          o << "\\e";
        else if (l != special())
          o << l;
        return o;
      }

      std::string
      format(label_t v) const
      {
        std::ostringstream o;
        print(o, v);
        return o.str();
      }
    };

    /// Compute the intersection with another alphabet.
    template <typename GenSet>
    nullableset<GenSet>
    intersect(const nullableset<GenSet>& lhs, const nullableset<GenSet>& rhs)
    {
      return {intersect(*lhs.genset(), *rhs.genset())};
    }

  }
}

#endif // !VCSN_CTX_NULLABLESET_HH
