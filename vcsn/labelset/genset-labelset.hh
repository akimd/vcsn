#pragma once

#include <memory>
#include <set>

#include <boost/optional.hpp>
#include <boost/range/algorithm/for_each.hpp>

#include <vcsn/core/kind.hh>
#include <vcsn/misc/raise.hh>

namespace vcsn
{
  namespace detail
  {
    /// This class has no modeling purpose, it only serves to factor
    /// code common to letterset and wordset.
    template <typename GenSet>
    struct genset_labelset
    {
      using genset_t = GenSet;
      using genset_ptr = std::shared_ptr<const genset_t>;

      using letter_t = typename genset_t::letter_t;
      using word_t = typename genset_t::word_t;
      using letters_t = typename genset_t::letters_t;

      genset_labelset(const genset_ptr& gs)
        : gs_{gs}
      {}

      genset_labelset(const genset_t& gs = {})
        : genset_labelset{std::make_shared<const genset_t>(gs)}
      {}

      genset_ptr genset() const
      {
        return gs_;
      }

      const genset_t& generators() const
      {
        return *gs_;
      }

      static symbol sname()
      {
        static auto res = symbol{genset_t::sname()};
        return res;
      }

      /// Read and process a class of letters.  Letters are sorted,
      /// and uniqued.
      ///
      /// Stream \a i is on the '[', read up to the closing ']',
      /// included.  Apply \a fun to all the letters.  Take negation
      /// into account.  Classes can never be empty.
      ///
      /// For instance "[a-d0-9_]", or "[^a-fz], or "[^]", but not
      /// "[]".
      ///
      /// \pre  i does not start with ']'.
      template <typename Fun>
      void
      convs_(std::istream& i, Fun fun) const
      {
        eat(i, '[');
        // Working via a set looks weird, instead of just calling
        // `conv_label_class_(*this, i, fun)`, but it allows to ensure
        // that letter classes never count letters several times.
        // Otherwise [aa] would be <2>a, instead of a.
        boost::for_each(conv_label_class_(*this, i), fun);
        eat(i, ']');
      }

      /// Read one letter from i.
      letter_t
      get_letter(std::istream& i, bool quoted = true) const
      {
        letter_t res = this->genset()->get_letter(i, quoted);
        require(this->has(res),
                "invalid label: unexpected ", str_escape(res));
        return res;
      }

      /// Use the implementation from genset.
# define DEFINE(Name, Attribute)                                        \
      template <typename... Args>                                       \
      Attribute                                                         \
      auto                                                              \
      Name(Args&&... args) const                                        \
        -> decltype(this->genset()->Name(std::forward<Args>(args)...))  \
      {                                                                 \
        return this->genset()->Name(std::forward<Args>(args)...);       \
      }

      DEFINE(begin,);
      DEFINE(delimit,);
      DEFINE(end,);
      DEFINE(equal, ATTRIBUTE_PURE);
      DEFINE(get_word,);
      DEFINE(has, ATTRIBUTE_PURE);
      DEFINE(is_letter, ATTRIBUTE_PURE);
      DEFINE(less, ATTRIBUTE_PURE);
      DEFINE(mul,);
      DEFINE(transpose, ATTRIBUTE_PURE);
      DEFINE(undelimit,);

# undef DEFINE

    private:
      genset_ptr gs_;
    };
  }
}
