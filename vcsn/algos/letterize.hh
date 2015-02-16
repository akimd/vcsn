#pragma once

#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/ctx/context.hh>
#include <vcsn/ctx/traits.hh> // context_t_of
#include <vcsn/dyn/automaton.hh> // dyn::make_automaton
#include <vcsn/labelset/letterset.hh>
#include <vcsn/labelset/nullableset.hh>
#include <vcsn/labelset/wordset.hh>
#include <vcsn/misc/algorithm.hh> // detail::back

namespace vcsn
{

  namespace detail
  {
    template <typename In_Aut, typename Out_Aut>
    class letterizer
    {
    public:
      using in_automaton_t = In_Aut;
      using in_ctx_t = context_t_of<in_automaton_t>;
      using in_state_t = state_t_of<in_automaton_t>;
      using in_labelset_t = labelset_t_of<in_automaton_t>;
      using in_label_t = typename in_labelset_t::value_t;
      using in_transition_t = transition_t_of<in_automaton_t>;

      using out_automaton_t = Out_Aut;
      using out_ctx_t = context_t_of<out_automaton_t>;
      using out_state_t = state_t_of<out_automaton_t>;
      using out_labelset_t = labelset_t_of<out_automaton_t>;
      using out_labelset_ptr = std::shared_ptr<out_labelset_t>;
      using out_label_t = typename out_labelset_t::value_t;
      using out_transition_t = transition_t_of<out_automaton_t>;

      // map in_state_t -> out_state_t
      using map_t = std::vector<out_state_t>;

      letterizer(const in_automaton_t& in_aut, out_labelset_ptr ls)
        : _in_aut(in_aut)
        , _out_aut(make_mutable_automaton(out_ctx_t{ls, in_aut->weightset()}))
        , _state_map(detail::back(in_aut->all_states()) + 1)
      {}

      out_automaton_t letterize()
      {
        auto in_ls = _in_aut->labelset();
        auto out_ws = _out_aut->weightset();
        // Copy the states, and setup the map
        _state_map[_in_aut->pre()] = _out_aut->pre();
        _state_map[_in_aut->post()] = _out_aut->post();
        for (auto st : _in_aut->states())
          _state_map[st] = _out_aut->new_state();

        for (auto st : _in_aut->all_states())
          for (auto tr : _in_aut->all_out(st))
            {
              auto letters = in_ls->letters_of(_in_aut->label_of(tr));
              if (! letters.empty())
              {
                size_t i = 0;
                auto src = _state_map[st];
                auto dst = letters.size() > 1 ? _out_aut->new_state()
                                              : _state_map[_in_aut->dst_of(tr)];
                _out_aut->new_transition(src, dst,
                                        letters[0], _in_aut->weight_of(tr));
                src = dst;
                std::cerr << letters.size() << std::endl;
                for (i = 1; i < letters.size(); ++i)
                {
                  dst = i == letters.size() - 1 ? _state_map[_in_aut->dst_of(tr)]
                                                : _out_aut->new_state();
                  _out_aut->new_transition(src, dst, letters[i], out_ws->one());
                  src = dst;
                }
              }
              else
                _out_aut->new_transition(_state_map[st],
                                        _state_map[_in_aut->dst_of(tr)],
                                        out_labelset_t::one(),
                                        _in_aut->weight_of(tr));
            }

        return std::move(_out_aut);
      }

    protected:
      const in_automaton_t& _in_aut;
      out_automaton_t _out_aut;
      map_t _state_map;
    };

    /// From an automaton, the corresponding automaton with a non-word labelset.
    template <typename Aut>
    struct letterized_automaton
    {
      using automaton_t = Aut;
      using result_t = const automaton_t;

      static result_t
      letterize(const automaton_t& aut)
      {
        return aut;
      }
    };

    template <typename GenSet, typename WeightSet>
    struct letterized_automaton<mutable_automaton<context<wordset<GenSet>, WeightSet>>>
    {
      using automaton_t = mutable_automaton<context<wordset<GenSet>, WeightSet>>;
      using result_t = mutable_automaton<context<nullableset<letterset<GenSet>>, WeightSet>>;

      using type = nullableset<letterset<GenSet>>;
      static std::shared_ptr<type>
      labelset(const std::shared_ptr<const wordset<GenSet>>& ls)
      {
        return std::make_shared<type>(type{ls->genset()});
      }

      static result_t
      letterize(const automaton_t& aut)
      {
        letterizer<automaton_t, result_t> lt(aut, labelset(aut->labelset()));
        return lt.letterize();
      }
    };

  }

  /*------------.
  | letterize.  |
  `------------*/

  /// Split the word transitions in the input automaton into letter ones.
  ///
  /// \param[in] aut        the automaton
  /// \returns              the letterized automaton
  template <typename Aut>
  typename detail::letterized_automaton<Aut>::result_t
  letterize(const Aut& aut)
  {
    return detail::letterized_automaton<Aut>::letterize(aut);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut>
      automaton letterize(const automaton& aut)
      {
        return make_automaton(::vcsn::letterize(aut->as<Aut>()));
      }
    }
  }

} // namespace vcsn
