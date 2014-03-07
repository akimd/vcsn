#ifndef VCSN_LABELSET_NULLABLESET_HH
# define VCSN_LABELSET_NULLABLESET_HH

# include <cstring> //strncmp
# include <memory>
# include <set>
# include <sstream>

# include <vcsn/alphabets/setalpha.hh> // intersect
# include <vcsn/core/kind.hh>
# include <vcsn/labelset/genset-labelset.hh>
# include <vcsn/labelset/fwd.hh>
# include <vcsn/misc/escape.hh>
# include <vcsn/misc/hash.hh>
# include <vcsn/misc/raise.hh>

namespace vcsn
{

  namespace detail {

    template <typename LabelSet>
    struct nullable_helper
    {
      using null = nullableset<LabelSet>;
      using kind_t = typename LabelSet::kind_t;

      static null make(std::istream& is)
      {
        // name: lal_char(abc)_ratexpset<law_char(xyz)_b>.
        //       ^^^ ^^^^ ^^^  ^^^^^^^^^^^^^^^^^^^^^^^^^
        //        |   |    |        weightset
        //        |   |    +-- gens
        //        |   +-- letter_type
        //        +-- kind
        null::make_nullableset_kind(is);
        eat(is, '<');
        auto ls = null::labelset_t::make(is);
        eat(is, '>');
        return null{ls};
      }

      ATTRIBUTE_PURE
      static constexpr typename null::value_t
      one()
      {
        return null::labelset_t::one();
      }
    };

    template<typename GenSet>
    struct nullable_helper<letterset<GenSet>>
    {
      using null = nullableset<letterset<GenSet>>;
      using kind_t = labels_are_nullable;

      static null
      make(std::istream& is)
      {
        using genset_t = typename null::labelset_t::genset_t;
        // name: lal_char(abc)_ratexpset<law_char(xyz)_b>.
        //       ^^^ ^^^^ ^^^  ^^^^^^^^^^^^^^^^^^^^^^^^^
        //        |   |    |        weightset
        //        |   |    +-- gens
        //        |   +-- letter_type
        //        +-- kind
        null::make_nullableset_kind(is);
        if (is.peek() == '_')
        {
          eat(is, '_');
          auto gs = genset_t::make(is);
          auto ls = typename null::labelset_t{gs};
          return null{ls};
        }
        eat(is, '<');
        auto ls = null::labelset_t::make(is);
        eat(is, '>');
        return null{ls};
      }

      ATTRIBUTE_PURE
      static constexpr typename null::value_t
      one()
      {
        return GenSet::one_letter();
      }
    };
  }

    /// Implementation of labels are nullables (letter or empty).
    template <typename LabelSet>
  class nullableset : public LabelSet
    {
    public:
      using labelset_t = LabelSet;
      using labelset_ptr = std::shared_ptr<const labelset_t>;
      using self_type = nullableset;
      using kind_t = typename detail::nullable_helper<labelset_t>::kind_t;

      using value_t = typename LabelSet::value_t;

      nullableset(const labelset_t& ls)
        : labelset_t{ls}, ls_{std::make_shared<const labelset_t>(ls)}
      {}

      nullableset(const std::shared_ptr<const labelset_t>& ls)
        : labelset_t{*ls}, ls_{ls}
      {}

      static std::string sname()
      {
        return "lan<" + labelset_t::sname() + ">";
      }

      std::string vname(bool full = true) const
      {
        return "lan<" + ls_->vname(full) + ">";
      }

      /// Build from the description in \a is.
      static nullableset make(std::istream& i)
      {
        return detail::nullable_helper<labelset_t>::make(i);
      }

      static constexpr bool
      has_one()
      {
        return true;
      }

      ATTRIBUTE_PURE
      static constexpr value_t
      one()
      {
        return detail::nullable_helper<labelset_t>::one();
      }

      ATTRIBUTE_PURE
      bool
      is_one(value_t l) const
      {
        return l == one();
      }

      bool
      is_valid(value_t v) const
      {
        return ls_->is_valid(v) || is_one(v);
      }

      static value_t
      conv(self_type, value_t v)
      {
        return v;
      }

      const labelset_ptr labelset() const
      {
        return ls_;
      }

      /// \throws std::domain_error if there is no label here.
      value_t
      conv(std::istream& i) const
      {
        value_t res;
        int c = i.peek();
        if (c == '\\')
          {
            i.ignore();
            c = i.peek();
            require(c == 'e', "invalid label: unexpected \\", str_escape(c));
            i.ignore();
            res = one();
          }
        else
          res = ls_->conv(i);
        return res;
      }

      std::set<value_t>
      convs(std::istream& i) const
      {
        return ls_->convs(i);
      }

      std::ostream&
      print(std::ostream& o, value_t l,
            const std::string& format = "text") const
      {
        if (is_one(l))
          o << (format == "latex" ? "\\varepsilon" : "\\e");
        else
          ls_->print(o, l, format);
        return o;
      }

      static void
      make_nullableset_kind(std::istream& is)
      {
        char kind[4];
        is.get(kind, sizeof kind);
        if (strncmp("lan", kind, 4))
          throw std::runtime_error("kind::make: unexpected: "
                                   + str_escape(kind)
                                   + ", expected: " + "lan");
      }

    private:
      labelset_ptr ls_;
    };


#define DEFINE(Func, Operation, Lhs, Rhs, Res)                  \
    template <typename GenSet>                                  \
    Res                                                         \
    Func(const Lhs& lhs, const Rhs& rhs)                        \
    {                                                           \
      return {Operation(*lhs.genset(), *rhs.genset())};     \
    }

    /// Compute the meet with another labelset.
    DEFINE(meet, intersection, nullableset<letterset<GenSet>>,
           nullableset<letterset<GenSet>>, nullableset<letterset<GenSet>>);

    DEFINE(meet, intersection, letterset<GenSet>,
           nullableset<letterset<GenSet>>, letterset<GenSet>);

    DEFINE(meet, intersection, nullableset<letterset<GenSet>>,
           letterset<GenSet>, letterset<GenSet>);

    template <typename Lls, typename Rls>
    nullableset<meet_t<Lls, Rls>>
    meet(const nullableset<Lls>& lhs, const nullableset<Rls>& rhs)
    {
      return nullableset<meet_t<Lls, Rls>>{meet(*lhs.labelset(), *rhs.labelset())};
    }

    /// compute the join with another labelset.
    DEFINE(join, get_union, nullableset<letterset<GenSet>>,
           nullableset<letterset<GenSet>>, nullableset<letterset<GenSet>>);

    DEFINE(join, get_union, letterset<GenSet>,
           nullableset<letterset<GenSet>>, nullableset<letterset<GenSet>>);

    DEFINE(join, get_union, nullableset<letterset<GenSet>>,
           letterset<GenSet>, nullableset<letterset<GenSet>>);

    template <typename Lls, typename Rls>
    nullableset<join_t<Lls, Rls>>
    join(const nullableset<Lls>& lhs, const nullableset<Rls>& rhs)
    {
      return nullableset<join_t<Lls, Rls>>{join(*lhs.labelset(), *rhs.labelset())};
    }


#undef DEFINE

  template <typename GenSet>
  inline
  std::ostream&
  print_set(const nullableset<GenSet>& ls,
            std::ostream& o, const std::string& format)
  {
    if (format == "latex")
      {
        o << "(";
        print_set(*ls.labelset(), o, format);
        o << ")^?";
      }
    else if (format == "text")
      o << ls.vname(true);
    else
      raise("invalid format: ", format);
    return o;
  }

}

#endif // !VCSN_LABELSET_NULLABLESET_HH
