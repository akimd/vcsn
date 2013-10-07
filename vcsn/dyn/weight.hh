#ifndef VCSN_DYN_WEIGHT_HH
# define VCSN_DYN_WEIGHT_HH

# include <memory>
# include <vcsn/ctx/fwd.hh>
# include <vcsn/dyn/weightset.hh>

namespace vcsn
{
  namespace dyn
  {
    namespace detail
    {

      /// Aggregate a weight and its weightset.
      ///
      /// FIXME: Improperly named, it is not a base class for
      /// static weights.
      class abstract_weight
      {
      public:
        /// A description of the weight.
        /// \param full  whether to include the genset.
        ///              if false, same as sname.
        std::string vname(bool full = true) const
        {
          return get_weightset().vname(full);
        }

        virtual const abstract_weightset& get_weightset() const = 0;

        template <typename WeightSet>
        concrete_abstract_weight<WeightSet>& as()
        {
          return dynamic_cast<concrete_abstract_weight<WeightSet>&>(*this);
        };

        template <typename WeightSet>
        const concrete_abstract_weight<WeightSet>& as() const
        {
          return dynamic_cast<const concrete_abstract_weight<WeightSet>&>(*this);
        };
      };

      /// Aggregate a weight and its weightset.
      ///
      /// FIXME: Improperly named, it is not a base class for
      /// static weights.
      template <typename WeightSet>
      class concrete_abstract_weight: public abstract_weight
      {
      public:
        using weightset_t = WeightSet;
        using super_type = abstract_weight;
        using weight_t = typename weightset_t::value_t;
        concrete_abstract_weight(const weight_t& weight,
                                 const weightset_t& weightset)
          : weight_(weight)
          , weightset_(weightset)
        {}
        virtual ~concrete_abstract_weight() {}

        const weight_t weight() const
        {
          return weight_;
        }

        virtual const weightset_t& get_weightset() const override
        {
          return weightset_;
        }
      protected:
        /// The weight.
        const weight_t weight_;
        /// The weight set.
        const weightset_t weightset_;
      };

    } // namespace detail

    using weight = std::shared_ptr<const detail::abstract_weight>;

    template <typename WeightSet>
    inline
    weight
    make_weight(const WeightSet& ws,
                const typename WeightSet::value_t& weight)
    {
      return std::make_shared<detail::concrete_abstract_weight<WeightSet>>
        (weight, ws);
    }
  } // namespace dyn
} // namespace vcsn

#endif // !VCSN_DYN_WEIGHT_HH
