#pragma once

#include <iostream>
#include <queue>
#include <set>
#include <type_traits>

#include <boost/bimap.hpp>
#include <boost/bimap/set_of.hpp>
#include <boost/bimap/unordered_set_of.hpp>

#include <vcsn/core/automaton-decorator.hh>
#include <vcsn/labelset/stateset.hh>
#include <vcsn/misc/bimap.hh> // vcsn::has
#include <vcsn/misc/unordered_map.hh> // vcsn::has

namespace vcsn
{
  namespace detail
  {
    template <typename Then, typename Else>
    auto static_if(std::true_type, Then then, Else)
    {
      return then;
    }

    template <typename Then, typename Else>
    auto static_if(std::false_type, Then, Else else_)
    {
      return else_;
    }


    /// A map.
    template <typename Stateset, typename StateNameset,
              bool Bidirectional>
    class state_bimap;

    template <typename Stateset, typename StateNameset>
    class state_bimap<Stateset, StateNameset, true>
    {
      using stateset_t = Stateset;
      using state_t = typename stateset_t::value_t;

      using state_nameset_t = StateNameset;
      using state_name_t = typename state_nameset_t::value_t;

      /// Storage for state names.
      using left_t
        = boost::bimaps::unordered_set_of<state_name_t,
                                          vcsn::hash<state_nameset_t>,
                                          vcsn::equal_to<state_nameset_t>>;
      /// Storage for state index.
      using right_t = boost::bimaps::set_of<state_t>;
      /// Bidirectional map state_name_t -> state_t;
      using bimap_t = boost::bimap<left_t, right_t>;

      bimap_t map_;

    public:
      using const_iterator = typename bimap_t::const_iterator;

      template <typename... Args>
      auto emplace(Args&&... args)
      {
        return map_.insert({ std::forward<Args>(args)... });
      }

      auto find_key(const state_name_t& s) const
      {
        return map_.left.find(s);
      }

      auto end_key() const
      {
        return map_.left.end();
      }

      /// Get the state name from a const_iterator.
      static const state_name_t& state_name(const const_iterator& i)
      {
        return i->left;
      }

      /// Get the state from a const_iterator.
      static state_t state(const const_iterator& i)
      {
        return i->right;
      }

      /// A map from state indexes to state names.
      using origins_t = typename bimap_t::right_map;
      const origins_t& origins() const
      {
        return map_.right;
      }
    };


    template <typename Stateset, typename StateNameset>
    class state_bimap<Stateset, StateNameset, false>
    {
      using stateset_t = Stateset;
      using state_t = typename stateset_t::value_t;

      using state_nameset_t = StateNameset;
      using state_name_t = typename state_nameset_t::value_t;

      using map_t = std::unordered_map<state_name_t, state_t,
                                       vcsn::hash<state_nameset_t>,
                                       vcsn::equal_to<state_nameset_t>>;
      map_t map_;

    public:
      using const_iterator = typename map_t::const_iterator;

      template <typename... Args>
      auto emplace(Args&&... args)
      {
        return map_.emplace(std::forward<Args>(args)...);
      }

      auto find_key(const state_name_t& sn) const
      {
        return map_.find(sn);
      }

      auto end_key() const
      {
        return map_.end();
      }

      /// Get the state name from a const_iterator.
      static const state_name_t& state_name(const const_iterator& i)
      {
        return i->first;
      }

      /// Get the state from a const_iterator.
      static state_t state(const const_iterator& i)
      {
        return i->second;
      }

      /// A map from state indexes to state names.
      using origins_t = std::map<state_t, state_name_t>;
      mutable origins_t origins_;
      const origins_t& origins() const
      {
        if (origins_.empty())
          for (const auto& p: map_)
            origins_.emplace(p.second, p.first);
        return origins_;
      }
    };

    template <Automaton Aut,
              wet_kind_t Kind = detail::wet_kind<labelset_t_of<Aut>,
                                                 weightset_t_of<Aut>>(),
              bool Lazy = false>
    class polystate_automaton_impl
      : public automaton_decorator<fresh_automaton_t_of<Aut>>
      , public state_bimap<stateset<Aut>,
                           polynomialset<context<stateset<Aut>,
                                                 weightset_t_of<Aut>>, Kind>,
                           Lazy>
    {
    public:
      using automaton_t = Aut;
      using context_t = context_t_of<automaton_t>;
      template <typename Ctx = context_t>
      using fresh_automaton_t = fresh_automaton_t_of<Aut, Ctx>;
      using super_t = automaton_decorator<fresh_automaton_t<>>;

      /// Labels and weights.
      using label_t = label_t_of<automaton_t>;
      using labelset_t = labelset_t_of<automaton_t>;
      using weightset_t = weightset_t_of<automaton_t>;

      /// State index.
      using state_t = state_t_of<automaton_t>;
      using stateset_t = stateset<automaton_t>;

      /// The state name: set of (input) states.
      using state_nameset_t
        = polynomialset<context<stateset_t, weightset_t>, Kind>;
      using state_name_t = typename state_nameset_t::value_t;

      using state_bimap_t
        = state_bimap<stateset<Aut>,
                      polynomialset<context<stateset<Aut>,
                                            weightset_t_of<Aut>>, Kind>,
                      Lazy>;

      /// Build the determinizer.
      /// \param a         the automaton to determinize
      polystate_automaton_impl(const automaton_t& a)
        : super_t(a->context())
        , input_(a)
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
        return static_if
          (bool_constant<Kind == wet_kind_t::bitset>{},
           [] (const auto& self) { return state_name_t(self.state_size_); },
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

      /// The state for set of states \a ss.
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

      /// The sets of (input) states waiting to be processed.
      using queue_t = std::queue<typename state_bimap_t::const_iterator>;
      queue_t todo_;

      /// We use state numbers as indexes, so we need to know the last
      /// state number.  If states were removed, it is not the same as
      /// the number of states.
      size_t state_size_ = input_->all_states().back() + 1;
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
