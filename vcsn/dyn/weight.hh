#ifndef VCSN_DYN_WEIGHT_HH
# define VCSN_DYN_WEIGHT_HH

# include <memory>
# include <string>

# include <vcsn/misc/export.hh>

namespace vcsn
{
  namespace dyn
  {
    namespace detail
    {

      /// An abstract weight.
      class LIBVCSN_API weight_base
      {
      public:
        /// A description of the weight type.
        virtual symbol vname() const = 0;

        /// Extract wrapped typed weight.
        template <typename WeightSet>
        weight_wrapper<WeightSet>& as()
        {
          return dynamic_cast<weight_wrapper<WeightSet>&>(*this);
        }

        /// Extract wrapped typed weight.
        template <typename WeightSet>
        const weight_wrapper<WeightSet>& as() const
        {
          return dynamic_cast<const weight_wrapper<WeightSet>&>(*this);
        }
      };

      /// Aggregate a weight and its weightset.
      template <typename WeightSet>
      class weight_wrapper: public weight_base
      {
      public:
        using weightset_t = WeightSet;
        using super_t = weight_base;
        using weight_t = typename weightset_t::value_t;
        weight_wrapper(const weight_t& w, const weightset_t& ws)
          : weight_(w)
          , weightset_(ws)
        {}

        virtual symbol vname() const override
        {
          return weightset().sname();
        }

        const weight_t weight() const
        {
          return weight_;
        }

        const weightset_t& weightset() const
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

    using weight = std::shared_ptr<const detail::weight_base>;

    template <typename WeightSet>
    inline
    weight
    make_weight(const WeightSet& ws,
                const typename WeightSet::value_t& w)
    {
      return std::make_shared<detail::weight_wrapper<WeightSet>>(w, ws);
    }
  } // namespace dyn
} // namespace vcsn

#endif // !VCSN_DYN_WEIGHT_HH
