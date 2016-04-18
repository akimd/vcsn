#pragma once

#include <memory>
#include <string>

#include <vcsn/misc/symbol.hh>
#include <vcsn/dyn/cast.hh>

namespace vcsn
{
  namespace dyn
  {
    namespace detail
    {
      /// A dyn expansion/expansionset.
      class LIBVCSN_API expansion
      {
      public:
        expansion()
          : self_()
        {}

        template <typename ExpansionSet>
        expansion(const ExpansionSet& es, const typename ExpansionSet::value_t& e)
          : self_(std::make_shared<model<ExpansionSet>>(es, e))
        {}

        /// A description of the expansion type.
        symbol vname() const
        {
          return self_->vname();
        }

        /// Extract wrapped typed expansion.
        template <typename ExpansionSet>
        auto& as()
        {
          return dyn_cast<model<ExpansionSet>&>(*self_);
        }

        /// Extract wrapped typed expansion.
        template <typename ExpansionSet>
        const auto& as() const
        {
          return dyn_cast<const model<ExpansionSet>&>(*self_);
        }

        auto* operator->()
        {
          return this;
        }

        const auto* operator->() const
        {
          return this;
        }

        bool operator!()
        {
          return !self_;
        }

        auto& operator=(const expansion& e)
        {
          self_ = std::move(e.self_);
          return *this;
        }

      private:
        /// Abstract wrapped typed expansion/expansionset.
        struct base
        {
          virtual ~base() = default;
          virtual symbol vname() const = 0;
        };

        /// A wrapped typed label/labelset.
        template <typename ValueSet>
        struct model final : base
        {
          using valueset_t = ValueSet;
          using value_t = typename valueset_t::value_t;

          model(const valueset_t& es, const value_t& e)
            : valueset_(es)
            , value_(e)
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
          /// The expansion set.
          const valueset_t valueset_;
          /// The expansion.
          const value_t value_;
        };

        /// The wrapped expansion/expansionset.
        std::shared_ptr<base> self_;
      };
    } // namespace detail

    using expansion = detail::expansion;
  } // namespace dyn
} // namespace vcsn
