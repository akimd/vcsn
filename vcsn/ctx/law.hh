#ifndef VCSN_CTX_LAW_HH
# define VCSN_CTX_LAW_HH

# include <memory>
# include <set>

# include <vcsn/core/kind.hh>

namespace vcsn
{
  namespace ctx
  {
    template <typename GenSet>
    struct Law
    {
      using genset_t = GenSet;
      using genset_ptr = std::shared_ptr<const genset_t>;

      using label_t = typename genset_t::word_t;
      using letter_t = typename genset_t::letter_t;
      using word_t = typename genset_t::word_t;
      using letters_t = std::set<letter_t>;

      using kind_t = labels_are_words;

      Law(const genset_ptr& gs)
        : gs_{gs}
      {}

      Law(const genset_t& gs = {})
        : Law{std::make_shared<const genset_t>(gs)}
      {}

      static std::string sname()
      {
        return "law_" + genset_t::sname();
      }

      std::string vname(bool full = true) const
      {
        return "law_" + genset()->vname(full);
      }

      const genset_ptr& genset() const
      {
        return gs_;
      }

      label_t
      special() const
      {
        return {genset()->special_letter()};
      }

# define DEFINE(Name)                                                   \
      template <typename... Args>                                       \
      auto                                                              \
      Name(Args&&... args) const                                        \
        -> decltype(this->genset()->Name(std::forward<Args>(args)...))  \
      {                                                                 \
        return this->genset()->Name(std::forward<Args>(args)...);       \
      }

      DEFINE(begin);
      DEFINE(concat);
      DEFINE(end);
      DEFINE(equals);
      DEFINE(format);
      DEFINE(has);
      DEFINE(identity);
      DEFINE(is_letter);
      DEFINE(print);
      DEFINE(to_word);
      DEFINE(transpose);

# undef DEFINE

    private:
      genset_ptr gs_;
    };

    /// Compute the intersection with another alphabet.
    template <typename GenSet>
    Law<GenSet>
    intersect(const Law<GenSet>& lhs, const Law<GenSet>& rhs)
    {
      return {intersect(lhs->genset(), rhs->genset())};
    }

  }
}

#endif // !VCSN_CTX_LAW_HH
