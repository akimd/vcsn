#ifndef VCSN_ALGOS_EDIT_HH
# define VCSN_ALGOS_EDIT_HH

# include <sstream>
# include <string>
# include <type_traits>

# include <vcsn/ctx/traits.hh>
# include <vcsn/dyn/automaton.hh>
# include <vcsn/dyn/fwd.hh>
# include <vcsn/misc/dynamic_bitset.hh>
# include <vcsn/misc/stream.hh>

namespace vcsn
{
  enum class edit_opcode
  {
    add_state =         0,
    remove_state =      1,
    add_transition =    2,
    set_transition =    3,
    get_transition =    4,
    remove_transition = 5,
    set_initialness =   6,
    set_finalness =     7,
    // Probably not worth the trouble of returning lists.
    get_states =        8,
    get_transitions =   9,
  };

  /*-----------------------------.
  | In-place automaton edition.  |
  `-----------------------------*/
  namespace detail
  {
    template <typename Aut>
    class editer // Yep.
    {
      using automaton_t = Aut;
      using label_t = label_t_of<automaton_t>;
      using weight_t = weight_t_of<automaton_t>;
      using state_t = state_t_of<automaton_t>;
      using transition_t = transition_t_of<automaton_t>;
      using labelset_t = labelset_t_of<automaton_t>;
      using weightset_t = weightset_t_of<automaton_t>;

      /// The automaton we're working on, and its context data.
      automaton_t& a_;
      const labelset_t& ls_;
      const weightset_t& ws_;

      /// Parameters.
      edit_opcode opcode_;
      int int1_;
      int int2_;
      const std::string& label_;
      const std::string& weight_;

      static const std::string int_to_string(int n)
      {
        std::ostringstream oss;
        oss << n;
        return oss.str();
      }

      void ensure_state_presence(const state_t s)
      {
        if (s == a_->pre() || s == a_->post() || ! a_->has_state(s))
          throw std::runtime_error("state not present " + int_to_string(s));
      }

      const label_t string_to_label(const std::string& s)
      {
        return conv(ls_, s);
      }

      const weight_t string_to_weight(const std::string& s)
      {
        if (s.length() == 0)
          return ws_.one();
        else if(s == "\\Kz")
          return ws_.zero();
        else if(s == "\\Ke")
          return ws_.one();
        else if(ws_.sname() == "zmin" // Yes, I know.
                && (s == "oo" || s == "inf"))
          return ws_.zero();
        else
          return conv(ws_, s);
      }

    public:
      editer(automaton_t& a,
             const edit_opcode opcode,
             int int1,
             int int2,
             const std::string& label,
             const std::string& weight)
        : a_(a)
        , ls_(* a->labelset())
        , ws_(* a->weightset())
        , opcode_(opcode)
        , int1_(int1)
        , int2_(int2)
        , label_(label)
        , weight_(weight)
      {}

      int operator()()
      {
        switch (opcode_)
          {
          case edit_opcode::add_state:
            return int(a_->new_state());
          case edit_opcode::remove_state:
            {
              state_t s = int1_;
              ensure_state_presence(s);
              a_->del_state(s);
              return -1;
            }
          case edit_opcode::add_transition:
            {
              state_t s1 = int1_, s2 = int2_;
              ensure_state_presence(s1);
              ensure_state_presence(s2);
              a_->add_transition(s1, s2, string_to_label(label_),
                                string_to_weight(weight_));
              return -1;
            }
          case edit_opcode::set_transition:
            {
              state_t s1 = int1_, s2 = int2_;
              ensure_state_presence(s1);
              ensure_state_presence(s2);
              label_t l = string_to_label(label_);
              a_->del_transition(s1, s2, l);
              a_->add_transition(s1, s2, l, string_to_weight(weight_));
              return -1;
            }
          case edit_opcode::get_transition:
            {
              state_t s1 = int1_, s2 = int2_;
              ensure_state_presence(s1);
              ensure_state_presence(s2);
              label_t l = string_to_label(label_);
              if (a_->has_transition(s1, s2, l))
                return a_->get_transition(s1, s2, l);
              else
                throw std::runtime_error("transition not present");
            }
          case edit_opcode::remove_transition:
            {
              transition_t t = int1_;
              if (! a_->has_transition(t))
                throw std::runtime_error("transition not present");
              a_->del_transition(t);
              return -1;
            }
          case edit_opcode::set_initialness:
            {
              state_t s = int1_;
              bool initialness = int2_;
              weight_t w = string_to_weight(weight_);
              ensure_state_presence(s);
              if (initialness)
                a_->set_initial(s, w);
              else
                {
                  if (! ws_.is_one(w))
                    throw std::runtime_error("non-one weight on unset");
                  a_->unset_initial(s);
                }
              return -1;
            }
          case edit_opcode::set_finalness:
            {
              state_t s = int1_;
              bool finalness = int2_;
              weight_t w = string_to_weight(weight_);
              ensure_state_presence(s);
              if (finalness)
                a_->set_final(s, w);
              else
                {
                  if (! ws_.is_one(w))
                    throw std::runtime_error("non-one weight on unset");
                  a_->unset_final(s);
                }
              return -1;
            }
          // Probably it's not worth the trouble of returning lists.
          case edit_opcode::get_states: case edit_opcode::get_transitions:
            throw std::runtime_error("unimplemented");
            break;
          default:
            throw std::runtime_error("unknown edit opcode");
          }
        return -1;
      }
    };
  }

  template <typename Aut>
  inline
  long
  edit(Aut& a,
       edit_opcode opcode,
       int int1 = -1,
       int int2 = -1,
       const std::string& label = "",
       const std::string& weight = "")
  {
    detail::editer<Aut> edit(a, opcode, int1, int2, label, weight);
    return edit();
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut, typename UselessOpcode, typename UselessInt1,
                typename UselessInt2, typename UselessLabelString,
                typename UselessWeightString>
      int
      edit(automaton& aut,
           int opcode,
           int int1, int int2, const std::string& label,
           const std::string& weight)
      {
        auto& a = aut->as<Aut>();
        return edit(a, edit_opcode(opcode), int1, int2, label, weight);
      }

      REGISTER_DECLARE(edit,
                       (automaton& aut,
                        int opcode, int int1, int int2,
                        const std::string& label, const std::string& weight)
                       -> int);
    }
  }

} // namespace vcsn

#endif // !VCSN_ALGOS_EDIT_HH
