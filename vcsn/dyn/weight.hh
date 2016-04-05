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
        /// Abstract wrapped typed weight/weightset.
        struct base
        {
          virtual ~base() = default;
          virtual symbol vname() const = 0;
        };

        /// A wrapped typed weight/weightset.
        template <typename ValueSet>
        struct model final: base
        {
        public:
          using valueset_t = ValueSet;
          using value_t = typename valueset_t::value_t;

          model(const valueset_t& ws, const value_t& w)
            : valueset_(ws)
            , value_(w)
          {}

          virtual symbol vname() const override
          {
            return valueset().sname();
          }

          const valueset_t& valueset() const
          {
            return valueset_;
          }

          const value_t value() const
          {
            return value_;
          }

        private:
          /// The value set.
          const valueset_t valueset_;
          /// The value.
          const value_t value_;
        };

        std::shared_ptr<base> self_;
      };
    } // namespace detail

    using weight =  detail::weight;

  } // namespace dyn
} // namespace vcsn
