#ifndef VCSN_CTX_CTX_HH
# define VCSN_CTX_CTX_HH

# include <vcsn/core/rat/node.fwd.hh>
# include <vcsn/core/kind.hh>

namespace vcsn
{
  namespace ctx
  {
    template <typename GenSet,
              typename WeightSet,
              typename Kind = labels_are_letters>
    class context
    {
    public:
      using genset_t = GenSet;
      using weightset_t = WeightSet;
      using kind_t = Kind;

      /// Type of transition labels, and type of RatExp atoms.
      using label_t = typename label_trait<kind_t, genset_t>::label_t;
      /// Type of weights.
      using weight_t = typename weightset_t::value_t;
      /// Type of RatExp nodes.
      using node_t = rat::node<label_t, weight_t>;
      /// Type of RatExp visitor.
      using const_visitor = vcsn::rat::const_visitor<label_t, weight_t>;

      context(const genset_t& gs = {}, const weightset_t& ws = {})
        : gs_(gs)
        , ws_(ws)
      {}

      const genset_t& genset() const
      {
        return gs_;
      }
      const weightset_t& weightset() const
      {
        return ws_;
      }

    private:
      genset_t gs_;
      weightset_t ws_;
    };
  }
};

#endif // !VCSN_CTX_CTX_HH
