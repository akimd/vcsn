#ifndef VCSN_CTX_GENSET_LABELSET_HH
# define VCSN_CTX_GENSET_LABELSET_HH

# include <memory>
# include <set>

# include <vcsn/core/kind.hh>

namespace vcsn
{
  namespace ctx
  {
    /// This class has no modeling purpose, it only serves to factor
    /// code common to letterset, nullableset and wordset.
    template <typename GenSet>
    struct genset_labelset
    {
      using genset_t = GenSet;
      using genset_ptr = std::shared_ptr<const genset_t>;

      using letter_t = typename genset_t::letter_t;
      using word_t = typename genset_t::word_t;
      using letters_t = std::set<letter_t>;

      genset_labelset(const genset_ptr& gs)
        : gs_{gs}
      {}

      genset_labelset(const genset_t& gs = {})
        : genset_labelset{std::make_shared<const genset_t>(gs)}
      {}

      const genset_ptr& genset() const
      {
        return gs_;
      }

      static std::string sname()
      {
        return genset_t::sname();
      }

      std::string vname(bool full = true) const
      {
        return this->genset()->vname(full);
      }

      /// Use the implementation from genset.
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
      DEFINE(conv);
      DEFINE(end);
      DEFINE(equals);
      DEFINE(format);
      DEFINE(has);
      DEFINE(is_letter);
      DEFINE(print);
      DEFINE(to_word);
      DEFINE(transpose);

# undef DEFINE

    private:
      genset_ptr gs_;
    };
  }
}

#endif // !VCSN_CTX_GENSET_LABELSET_HH
