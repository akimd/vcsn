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
        template <typename ExpansionSet>
        struct model final : base
        {
          using expansionset_t = ExpansionSet;
          using expansion_t = typename expansionset_t::value_t;

          model(const expansionset_t& es, const expansion_t& e)
            : expansionset_(es)
            , expansion_(e)
          {}

          virtual symbol vname() const override
          {
            return expansionset().sname();
          }

          const expansionset_t& expansionset() const
          {
            return expansionset_;
          }

          const expansion_t expansion() const
          {
            return expansion_;
          }

        private:
          /// The expansion set.
          const expansionset_t expansionset_;
          /// The expansion.
          const expansion_t expansion_;
        };

        /// The wrapped expansion/expansionset.
        std::shared_ptr<base> self_;
      };
    } // namespace detail

    using expansion = detail::expansion;
  } // namespace dyn
} // namespace vcsn
