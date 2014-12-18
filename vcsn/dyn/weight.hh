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
        auto& as()
        {
          return dyn_cast<weight_wrapper<WeightSet>&>(*this);
        }

        /// Extract wrapped typed weight.
        template <typename WeightSet>
        auto& as() const
        {
          return dyn_cast<const weight_wrapper<WeightSet>&>(*this);
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
        weight_wrapper(const weightset_t& ws, const weight_t& w)
          : weightset_(ws)
          , weight_(w)
        {}

        virtual symbol vname() const override
        {
          return weightset().sname();
        }

        const weightset_t& weightset() const
        {
          return weightset_;
        }

        const weight_t weight() const
        {
          return weight_;
        }

      private:
        /// The weight set.
        const weightset_t weightset_;
        /// The weight.
        const weight_t weight_;
      };

    } // namespace detail

    using weight = std::shared_ptr<const detail::weight_base>;

    template <typename WeightSet>
    inline
    weight
    make_weight(const WeightSet& ws,
                const typename WeightSet::value_t& w)
    {
      return std::make_shared<detail::weight_wrapper<WeightSet>>(ws, w);
    }
  } // namespace dyn
} // namespace vcsn

#endif // !VCSN_DYN_WEIGHT_HH
