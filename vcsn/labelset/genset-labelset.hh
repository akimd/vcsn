#ifndef VCSN_LABELSET_GENSET_LABELSET_HH
# define VCSN_LABELSET_GENSET_LABELSET_HH

# include <memory>
# include <set>

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

      static std::string sname()
      {
        return genset_t::sname();
      }

      std::string vname(bool full = true) const
      {
        return this->genset().vname(full);
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
        int c = i.get();
        require(c == '[',
                "expected '[', found ", str_escape(c));
        int previous = -1;
        std::set<letter_t> res;
        while (!i.eof() && i.peek() != ']')
          {
            c = i.get();
            if (c == '-')
              {
                require(previous != -1, "bracket cannot begin with '-'.");
                // Handle ranges
                int endrange = i.peek();
                if (endrange == ']')
                  res.insert('-');
                else
                  {
                    auto it = this->genset().find(previous);
                    for (; *it != endrange && it != this->genset().end();
                         it++)
                      res.insert(*it);
                    require(it != this->genset().end(),
                            "unexpected ", str_escape(endrange));
                  }
              }
            else
              {
                if (c == '\\')
                  c = i.get();
                if (this->has(c))
                  res.insert(c);
                else
                  throw std::domain_error("invalid label: unexpected "
                                          + str_escape(c));
                previous = c;
              }
          }
        require(!i.eof(), "EOF, expected ']'");
        i.ignore(); // Eat ]

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
      DEFINE(conv,);
      DEFINE(delimit,);
      DEFINE(undelimit,);
      DEFINE(end,);
      DEFINE(equals, ATTRIBUTE_PURE);
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
