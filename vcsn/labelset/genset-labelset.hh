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
    /// code common to letterset, nullableset and wordset.
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
        static symbol res(genset_t::sname());
        return res;
      }

      /// Read and process a class of letters.
      ///
      /// Stream \a i is after the '[', read up to the closing ']',
      /// excluded.  Apply \a fun to all the letters.  Take negation
      /// into account.  Classes can never be empty.
      ///
      /// For instance "[a-d0-9_]", or "[^a-fz], or "[^]", but not
      /// "[]".
      ///
      /// \pre  i does not start with ']'.
      template <typename Fun>
      void
      convs_classes_(std::istream& i, Fun fun) const
      {
        if (i.peek() == '^')
          {
            i.ignore();
            auto alphabet = letters_t{};
            for (auto l : this->generators())
              alphabet.insert(l);
            boost::for_each(set_difference(alphabet, convs_classes_(i)),
                            fun);
          }
        else
          {
            // The last letter we read, for intervals.
            boost::optional<letter_t> prev;
            while (i.peek() != EOF && i.peek() != ']')
              if (i.peek() == '-')
                {
                  require(prev != boost::none,
                          "bracket cannot begin with '-'");
                  // With string_letter's letter_t, letter_t{'-'} may
                  // fail depending on the version of Boost and of
                  // libstdc++.  So rather, keep this dash.
                  auto dash = this->genset()->get_letter(i);
                  // Handle ranges.
                  if (i.peek() == ']')
                    // [abc-] does not denote an interval.
                    fun(dash);
                  else
                    {
                      // [prev - l2].
                      letter_t l2 = get_letter(i);
                      // Skip prev, which was already processed.
                      for (auto i = std::next(this->genset()->find(prev.get()));
                           i != this->genset()->end() && *i < l2;
                           ++i)
                        fun(*i);
                      // The last letter.  Do not do this in the loop,
                      // we might overflow the capacity of char.
                      // Check validity, so that 'z-a' is empty.
                      if (prev.get() < l2)
                        fun(l2);

                      prev = boost::none;
                    }
                }
              else
                {
                  letter_t l = get_letter(i);
                  fun(l);
                  prev = l;
                }
          }
      }

      /// Read a set of letters.
      letters_t
      convs_classes_(std::istream& i) const
      {
        letters_t res;
        convs_classes_(i, [&res](letter_t l){ res.insert(l); });
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
        boost::for_each(convs_classes_(i), fun);
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
