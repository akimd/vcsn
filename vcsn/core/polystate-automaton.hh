#pragma once

#include <iostream>
#include <queue>
#include <set>
#include <type_traits>

#include <vcsn/core/automaton-decorator.hh>
#include <vcsn/core/state-bimap.hh>
#include <vcsn/misc/wet.hh>

namespace vcsn
{
  namespace detail
  {
    /// An automaton whose state names are polynomials of states.
    ///
    /// \tparam Aut   the input automaton type, whose states
    ///               will form the polynomials of states.
    /// \tparam Kind  the desired implemenation of the polynomials.
    template <Automaton Aut,
              wet_kind_t Kind = detail::wet_kind<labelset_t_of<Aut>,
                                                 weightset_t_of<Aut>>(),
              bool Lazy = false>
    class polystate_automaton_impl
      : public automaton_decorator<fresh_automaton_t_of<Aut>>
      , public state_bimap<polynomialset<context<stateset<Aut>,
                                                 weightset_t_of<Aut>>, Kind>,
                           stateset<Aut>,
                           Lazy>
    {
    public:
      using automaton_t = Aut;
      using context_t = context_t_of<automaton_t>;
      template <typename Ctx = context_t>
      using fresh_automaton_t = fresh_automaton_t_of<Aut, Ctx>;
      using super_t = automaton_decorator<fresh_automaton_t<>>;
      using state_bimap_t
        = state_bimap<polynomialset<context<stateset<Aut>,
                                            weightset_t_of<Aut>>, Kind>,
                      stateset<Aut>,
                      Lazy>;

      /// Labels and weights.
      using label_t = label_t_of<automaton_t>;
      using labelset_t = labelset_t_of<automaton_t>;
      using weightset_t = weightset_t_of<automaton_t>;

      /// State index.
      using state_t = state_t_of<automaton_t>;
      using stateset_t = stateset<automaton_t>;

      /// The state name: set of (input) states.
      using state_nameset_t = typename state_bimap_t::state_nameset_t;
      using state_name_t = typename state_bimap_t::state_name_t;

      /// Build the determinizer.
      /// \param a         the automaton to determinize
      polystate_automaton_impl(const automaton_t& a)
        : super_t{a->context()}
        , input_{a}
      {
        // Pre.
        {
          auto pre = zero();
          ns_.new_weight(pre, input_->pre(), ws_.one());
          todo_.push(this->emplace(std::move(pre), this->pre()).first);
          if (Lazy)
            this->set_lazy(this->pre());
        }

        // Post.
        {
          auto post = zero();
          ns_.new_weight(post, input_->post(), ws_.one());
          this->emplace(std::move(post), this->post());
        }
      }

      bool state_has_name(state_t s) const
      {
        return has(this->origins(), s);
      }

      /// The empty polynomial of states.
      state_name_t zero() const
      {
        return static_if<Kind == wet_kind_t::bitset>
          ([] (const auto& self) { return state_name_t(self.state_size_); },
           [] (const auto& self) { return self.ns_.zero(); })
        (*this);
      }

      std::ostream&
      print_state_name(state_t s, std::ostream& o,
                       format fmt = {}, bool delimit = false) const
      {
        const auto& origs = this->origins();
        auto i = origs.find(s);
        if (i == std::end(origs))
          this->print_state(s, o);
        else
          {
            if (delimit)
              o << '{';
            ns_.print(i->second, o, fmt, ", ");
            if (delimit)
              o << '}';
          }
        return o;
      }

      /// The state for set of states \a n.
      /// If this is a new state, schedule it for visit.
      state_t state_(state_name_t n)
      {
        state_t res;
        auto i = this->find_key(n);
        if (i == this->end_key())
          {
            res = this->new_state();
            if (Lazy)
              this->set_lazy(res);
            todo_.push(this->emplace(std::move(n), res).first);
          }
        else
          res = i->second;
        return res;
      }

      /// Input automaton.
      automaton_t input_;

      /// Its weightset.
      weightset_t ws_ = *input_->weightset();

      /// (Nameset) The polynomialset that stores weighted states.
      state_nameset_t ns_ = {{stateset_t(input_), ws_}};

      /// States waiting to be processed.
      using queue_t = std::queue<typename state_bimap_t::const_iterator>;
      queue_t todo_;

      /// We use state numbers as indexes, so we need to know the last
      /// state number.  If states were removed, it is not the same as
      /// the number of states.
      size_t state_size_ = states_size(input_);
    };

    /// A polystate automaton as a shared pointer.
    template <Automaton Aut,
              wet_kind_t Kind = wet_kind<labelset_t_of<Aut>,
                                         weightset_t_of<Aut>>(),
              bool Lazy = false>
    using polystate_automaton
      = std::shared_ptr<polystate_automaton_impl<Aut, Kind, Lazy>>;

    template <Automaton Aut,
              wet_kind_t Kind = wet_kind<labelset_t_of<Aut>,
                                         weightset_t_of<Aut>>(),
              bool Lazy = false>
    auto
    make_polystate_automaton(const Aut& aut)
    {
      using res_t = polystate_automaton<Aut, Kind, Lazy>;
      return make_shared_ptr<res_t>(aut);
    }
  }
} // namespace vcsn
