#include <sstream>

#include <vcsn/dyn/ratexp.hh> // dyn::make_ratexp
#include <vcsn/labelset/oneset.hh>
#include <vcsn/misc/cast.hh> // down_pointer_cast
#include <vcsn/misc/stream.hh> // conv

namespace vcsn
{
namespace dyn
{
  namespace detail
  {
    /*--------------------.
    | ratexpset_wrapper.  |
    `--------------------*/

    template <typename RatExpSet>
    inline
    ratexpset_wrapper<RatExpSet>::ratexpset_wrapper
      (const ratexpset_t& rs)
      : super_type()
      , rs_(rs)
    {}

#define DEFINE                                  \
    template <typename RatExpSet>               \
    inline                                      \
    auto                                        \
    ratexpset_wrapper<RatExpSet>

    DEFINE::down(const value_t& v) const
      -> std::shared_ptr<const node_t>
    {
      return down_pointer_cast<const node_t>(v);
    }

    DEFINE::down(const std::string& w) const -> weight_t
    {
      return ::vcsn::conv(*rs_.weightset(), w);
    }

    DEFINE::make_ratexp(const value_t& v) const
      -> dyn::ratexp
    {
      return dyn::make_ratexp(rs_, down(v));
    }

    DEFINE::zero() const -> value_t
    {
      return rs_.zero();
    }

    DEFINE::one() const -> value_t
    {
      return rs_.one();
    }

    DEFINE::atom(const std::string& w) const -> value_t
    {
      return rs_.atom(::vcsn::conv(*rs_.labelset(), w));
    }

    DEFINE::add(value_t l, value_t r) const -> value_t
    {
      return rs_.add(down(l), down(r));
    }

    DEFINE::mul(value_t l, value_t r) const -> value_t
    {
      return rs_.mul(down(l), down(r));
    }

    DEFINE::conjunction(value_t l, value_t r) const -> value_t
    {
      return rs_.conjunction(down(l), down(r));
    }

    DEFINE::shuffle(value_t l, value_t r) const -> value_t
    {
      return rs_.shuffle(down(l), down(r));
    }

    DEFINE::ldiv(value_t l, value_t r) const -> value_t
    {
      return rs_.ldiv(down(l), down(r));
    }

    DEFINE::rdiv(value_t l, value_t r) const -> value_t
    {
      return rs_.rdiv(down(l), down(r));
    }

    DEFINE::concat(value_t l, value_t r) const -> value_t
    {
      return rs_.concat(down(l), down(r));
    }

    DEFINE::star(value_t v) const -> value_t
    {
      return rs_.star(down(v));
    }

    DEFINE::complement(value_t v) const -> value_t
    {
      return rs_.complement(down(v));
    }

    DEFINE::transposition(value_t v) const -> value_t
    {
      return rs_.transposition(down(v));
    }

    DEFINE::lmul(const std::string& w, value_t v) const -> value_t
    {
      return rs_.lmul(down(w), down(v));
    }

    DEFINE::rmul(value_t v, const std::string& w) const -> value_t
    {
      return rs_.rmul(down(v), down(w));
    }

    DEFINE::letter_class(const letter_class_t& cs, bool accept) const -> value_t
    {
      using labelset_t = typename ratexpset_t::labelset_t;
      return letter_class_<labelset_t>(cs, accept,
                                     std::is_same<labelset_t, vcsn::oneset>{});
    }

    template <typename RatExpSet>
    template <typename LabelSet_>
    inline
    auto
    ratexpset_wrapper<RatExpSet>::letter_class_(const letter_class_t& chars,
                                              bool accept,
                                              std::false_type) const -> value_t
    {
      auto ls = *rs_.labelset();

      using labelset_t = decltype(ls);
      using word_t = typename labelset_t::word_t;
      using label_t = typename labelset_t::value_t;
      using letter_t = typename labelset_t::letter_t;

      std::set<std::pair<letter_t, letter_t>> ccs;
      for (auto cc: chars)
        {
          // Yes, this is ugly: to convert an std::string into a
          // letter_t, we (i) parse the string into a label, (ii)
          // convert it into a word_t, (iii) from which we extract its
          // first letter.
          label_t lbl1 = ::vcsn::conv(ls, cc.first);
          label_t lbl2 = ::vcsn::conv(ls, cc.second);
          word_t w1 = ls.word(lbl1);
          word_t w2 = ls.word(lbl2);
          letter_t l1 = *std::begin(ls.letters_of(w1));
          letter_t l2 = *std::begin(ls.letters_of(w2));
          ccs.emplace(l1, l2);
        }
      return rs_.letter_class(ccs, accept);
    }

    template <typename RatExpSet>
    template <typename LabelSet_>
    inline
    auto
    ratexpset_wrapper<RatExpSet>::letter_class_(const letter_class_t&,
                                              bool,
                                              std::true_type) const
      -> value_t
    {
      return one();
    }

    DEFINE::conv(std::istream& is) const -> value_t
    {
      return rs_.conv(is);
    }

    DEFINE::print(std::ostream& o, value_t v) const -> std::ostream&
    {
      return rs_.print(o, down(v));
    }

#undef DEFINE

  } // namespace detail

  template <typename RatExpSet>
  inline
  ratexpset
  make_ratexpset(const RatExpSet& rs)
  {
    using wrapper_t = detail::ratexpset_wrapper<RatExpSet>;
    return std::make_shared<wrapper_t>(rs);
  }

} // namespace dyn
} // namespace vcsn
