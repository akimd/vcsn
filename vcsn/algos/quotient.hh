#ifndef VCSN_ALGOS_QUOTIENT_HH
# define VCSN_ALGOS_QUOTIENT_HH

# include <algorithm> // min_element.
# include <unordered_map>
# include <unordered_set>

# include <vcsn/algos/accessible.hh>
# include <vcsn/algos/is-deterministic.hh>
# include <vcsn/dyn/automaton.hh>
# include <vcsn/misc/dynamic_bitset.hh>

namespace vcsn
{

  namespace detail
  {
    /// Apply a quotient onto an automaton: fuse equivalent states.
    template <typename Aut>
    class quotienter
    {
    public:
      using automaton_t = Aut;

      using class_t = unsigned;
      using state_t = typename automaton_t::state_t;
      using set_t = std::vector<state_t>;
      using state_to_class_t = std::unordered_map<state_t, class_t>;
      using class_to_set_t = std::vector<set_t>;
      using class_to_state_t = std::vector<state_t>;

      /// \param class_to_set  The equivalence classes.
      ///             Might be modified to put the states with the
      ///             smallest ID first in their class.
      quotienter(class_to_set_t& class_to_set)
        : class_to_set_(class_to_set)
        , num_classes_(class_to_set_.size())
      {
        sort_classes_();
      }

      /// Sort the classes.
      ///
      /// This step, which is "useless" in that the result would be
      /// correct anyway, just ensures that the classes are numbered
      /// after their states: classes are sorted by the smallest of
      /// their state ids.
      void sort_classes_()
      {
        /* For each class, put its smallest numbered state first.  We
           don't need to fully sort.  */
        for (unsigned c = 0; c < num_classes_; ++c)
            std::swap(class_to_set_[c][0],
                      *std::min_element(begin(class_to_set_[c]),
                                        end(class_to_set_[c])));

        /* Sort class numbers by smallest state number.  */
        std::sort(begin(class_to_set_), end(class_to_set_),
                  [](const set_t& lhs, const set_t& rhs) -> bool
                  {
                    return lhs[0] < rhs[0];
                  });
      }

      /// Build the resulting automaton.
      automaton_t build_result_(const automaton_t& aut)
      {
        state_to_class_t state_to_class;
        for (unsigned c = 0; c < num_classes_; ++c)
          for (auto s: class_to_set_[c])
            state_to_class[s] = c;

        automaton_t res{aut.context()};
        class_to_res_state_.resize(num_classes_);
        for (unsigned c = 0; c < num_classes_; ++c)
          {
            state_t s = class_to_set_[c][0];
            class_to_res_state_[c]
              = s == aut.pre()  ? res.pre()
              : s == aut.post() ? res.post()
              : res.new_state();
          }
        for (unsigned c = 0; c < num_classes_; ++c)
          {
            // Copy the transitions of the first state of the class in
            // the result.
            state_t s = class_to_set_[c][0];
            state_t src = class_to_res_state_[c];
            for (auto t : aut.all_out(s))
              {
                state_t d = aut.dst_of(t);
                state_t dst = class_to_res_state_[state_to_class[d]];
                res.add_transition(src, dst, aut.label_of(t), aut.weight_of(t));
              }
          }
        return std::move(res);
      }

      /// The minimized automaton.
      automaton_t operator()(const automaton_t& aut)
      {
        return build_result_(aut);
      }

      /// A map from quotient states to sets of original states.
      using origins_t = std::map<state_t, std::set<state_t>>;
      origins_t
      origins()
      {
        origins_t res;
        for (unsigned c = 0; c < num_classes_; ++c)
          res[class_to_res_state_[c]]
              .insert(begin(class_to_set_[c]), end(class_to_set_[c]));
        return res;
      }

      /// Print the origins.
      static
      std::ostream&
      print(std::ostream& o, const origins_t& orig)
      {
        o << "/* Origins." << std::endl
          << "    node [shape = box, style = rounded]" << std::endl;
        for (auto p : orig)
          if (2 <= p.first)
            {
              o << "    " << p.first - 2
                << " [label = \"";
              const char* sep = "";
              for (auto s: p.second)
                {
                  o << sep << s - 2;
                  sep = ",";
                }
              o << "\"]" << std::endl;
            }
        o << "*/" << std::endl;
        return o;
      }

    private:
      std::ostream& print_(std::ostream& o, const set_t& ss) const
      {
        const char* sep = "{";
        for (auto s : ss)
          {
            o << sep << s;
            sep = ", ";
          }
        return o << "}";
      }
      std::ostream& print_(std::ostream& o, const class_to_set_t& c2ss) const
      {
        const char* sep = "";
        for (unsigned i = 0; i < c2ss.size(); ++i)
          {
            o << sep << '[' << i << "] = ";
            print_(o, c2ss[i]);
            sep = "\n";
          }
        return o;
      }

      class_to_set_t& class_to_set_;
      unsigned num_classes_;
      class_to_state_t class_to_res_state_;
    };
  } // detail::

  template <typename Aut>
  inline
  Aut
  quotient(const Aut& a,
           typename detail::quotienter<Aut>::class_to_set_t& classes)
  {
    detail::quotienter<Aut> quotient(classes);
    auto res = quotient(a);
    // FIXME: Not absolutely elegant.  But currently no means to
    // associate meta-data to states.
    if (getenv("VCSN_ORIGINS"))
      quotient.print(std::cout, quotient.origins());
    return res;
  }

} // namespace vcsn

#endif // !VCSN_ALGOS_QUOTIENT_HH
