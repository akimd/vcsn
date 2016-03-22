#pragma once

#include <memory>
#include <string>

#include <vcsn/dyn/cast.hh>
#include <vcsn/misc/export.hh>

namespace vcsn
{
  namespace dyn
  {
    namespace detail
    {
      /// An abstract weight.
      class LIBVCSN_API weight
      {
      public:
        weight()
          : self_()
        {}

        template <typename WeightSet>
        weight(const WeightSet& ws, const typename WeightSet::value_t& w)
          : self_(std::make_shared<model<WeightSet>>(ws, w))
        {}

        symbol vname() const
        {
          return self_->vname();
        }

        /// Extract wrapped typed weight.
        template <typename WeightSet>
        auto& as()
        {
          return dyn_cast<model<WeightSet>&>(*self_);
        }

        /// Extract wrapped typed weight.
        template <typename WeightSet>
        auto& as() const
        {
          return dyn_cast<const model<WeightSet>&>(*self_);
        }

        auto* operator->()
        {
          return this;
        }

        const auto* operator->() const
        {
          return this;
        }

      private:
        /// Abstract wrapped typed label.
        struct base
        {
          virtual ~base() = default;
          virtual symbol vname() const = 0;
        };

        /// Aggregate a weight and its weightset.
        template <typename WeightSet>
        struct model final: base
        {
        public:
          using weightset_t = WeightSet;
          using weight_t = typename weightset_t::value_t;

          model(const weightset_t& ws, const weight_t& w)
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

        std::shared_ptr<base> self_;
      };
    } // namespace detail

    using weight =  detail::weight;

  } // namespace dyn
} // namespace vcsn
