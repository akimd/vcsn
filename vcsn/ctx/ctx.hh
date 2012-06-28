#ifndef VCSN_CTX_CTX_HH
# define VCSN_CTX_CTX_HH

# include <memory>
# include <vcsn/core/rat/fwd.hh>
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
      using genset_ptr = std::shared_ptr<const genset_t>;
      using weightset_ptr = std::shared_ptr<const weightset_t>;
      using kind_t = Kind;

      /// Type of transition labels, and type of RatExp atoms.
      using label_t = typename label_trait<kind_t, genset_t>::label_t;
      /// Type of weights.
      using weight_t = typename weightset_t::value_t;
      /// Type of RatExp kratexps objects.
      using node_t = rat::node<label_t, weight_t>;
      using kratexp_t = std::shared_ptr<const node_t>;
      using kratexpset_t = typename vcsn::kratexpset<context>;
      /// Type of RatExp visitor.
      using const_visitor = vcsn::rat::const_visitor<label_t, weight_t>;

      context(const genset_ptr& gs, const weightset_ptr& ws)
        : gs_(gs)
        , ws_(ws)
      {}

      context(const genset_t& gs = {}, const weightset_t& ws = {})
        : context(std::make_shared<const genset_t>(gs), 
                  std::make_shared<const weightset_t>(ws))
      {}

      const genset_ptr& genset() const
      {
        return gs_;
      }
      const weightset_ptr& weightset() const
      {
        return ws_;
      }

      std::string format(const kratexp_t& e) const
      {
        kratexpset_t ks{*this};
        return ks.format(e);
      }

      }

    private:
      genset_ptr gs_;
      weightset_ptr ws_;
    };
  }
};

#endif // !VCSN_CTX_CTX_HH
