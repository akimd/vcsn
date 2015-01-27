#ifndef VCSN_LABELSET_GENSET_LABELSET_HH
# define VCSN_LABELSET_GENSET_LABELSET_HH

# include <memory>
# include <set>

# include <boost/optional.hpp>

# include <vcsn/core/kind.hh>
# include <vcsn/misc/raise.hh>

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
      using letters_t = std::set<letter_t>;

      genset_labelset(const genset_ptr& gs)
        : gs_{gs}
      {}

      genset_labelset(const genset_t& gs = {})
        : genset_labelset{std::make_shared<const genset_t>(gs)}
      {}

      const genset_t& genset() const
      {
        return *gs_;
      }

      static symbol sname()
      {
        static symbol res(genset_t::sname());
        return res;
      }

      /// Read a range of letters.
      ///
      /// Stream \a i is right on a '['.  Read up to the closing ']',
      /// and return the list of the matching labels.
      ///
      /// For instance "[a-d0-9_]".
      std::set<letter_t>
      convs_(std::istream& i) const
      {
        eat(i, '[');
        // The last letter we read, for intervals.
        boost::optional<letter_t> previous;
        std::set<letter_t> res;
        while (i.peek() != EOF && i.peek() != ']')
          {
            if (i.peek() == '-')
              {
                require(previous != boost::none,
                        "bracket cannot begin with '-'");
                i.ignore();
                // Handle ranges.
                if (i.peek() == ']')
                  // [abc-] does not denote an interval.
                  res.insert(letter_t{'-'});
                else
                  {
                    // [prev - l2].
                    letter_t l2 = genset_t::get_letter(i);
                    require(this->has(l2),
                            "invalid label: unexpected ", str_escape(l2));
                    for (auto i = this->genset().find(previous.get());
                         i != this->genset().end();
                         ++i)
                      {
                        res.insert(*i);
                        if (*i == l2)
                          break;
                      }
                    previous = boost::none;
                  }
              }
            else
              {
                letter_t l = genset_t::get_letter(i);
                require(this->has(l),
                        "invalid label: unexpected ", str_escape(l));
                res.insert(l);
                previous = l;
              }
          }
        eat(i, ']');
        return res;
      }

      /// Use the implementation from genset.
# define DEFINE(Name, Attribute)                                        \
      template <typename... Args>                                       \
      Attribute                                                         \
      auto                                                              \
      Name(Args&&... args) const                                        \
        -> decltype(this->genset().Name(std::forward<Args>(args)...))   \
      {                                                                 \
        return this->genset().Name(std::forward<Args>(args)...);        \
      }

      DEFINE(begin,);
      DEFINE(concat,);
      DEFINE(get_word,);
      DEFINE(delimit,);
      DEFINE(undelimit,);
      DEFINE(end,);
      DEFINE(equal, ATTRIBUTE_PURE);
      DEFINE(has, ATTRIBUTE_PURE);
      DEFINE(is_letter, ATTRIBUTE_PURE);
      DEFINE(transpose, ATTRIBUTE_PURE);

# undef DEFINE

    private:
      genset_ptr gs_;
    };
  }
}

#endif // !VCSN_LABELSET_GENSET_LABELSET_HH
