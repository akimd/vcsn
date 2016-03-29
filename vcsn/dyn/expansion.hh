#pragma once

#include <memory>
#include <string>

#include <vcsn/dyn/fwd.hh>
#include <vcsn/misc/symbol.hh>

namespace vcsn
{
  namespace dyn
  {
    namespace detail
    {
      /// An abstract expansion.
      class expansion_base
      {
      public:
        /// A description of the expansion.
        virtual symbol vname() const = 0;

        template <typename ExpansionSet>
        auto& as()
        {
          return dyn_cast<expansion_wrapper<ExpansionSet>&>(*this);
        }

        template <typename ExpansionSet>
        auto& as() const
        {
          return dyn_cast<const expansion_wrapper<ExpansionSet>&>(*this);
        }
      };

      /// Aggregate an expansion and its expansionset.
      template <typename ExpansionSet>
      class expansion_wrapper final: public expansion_base
      {
      public:
        using expansionset_t = ExpansionSet;
        using super_t = expansion_base;
        using expansion_t = typename expansionset_t::value_t;
        expansion_wrapper(const expansionset_t& expansionset,
                          const expansion_t& expansion)
          : expansionset_(expansionset)
          , expansion_(expansion)
        {}
        virtual ~expansion_wrapper() {}

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

    } // namespace detail

    using expansion = std::shared_ptr<const detail::expansion_base>;

    template <typename ExpansionSet>
    inline
    expansion
    make_expansion(const ExpansionSet& ps,
                   const typename ExpansionSet::value_t& expansion)
    {
      using wrapper_t = detail::expansion_wrapper<ExpansionSet>;
      return std::make_shared<wrapper_t>(ps, expansion);
    }
  } // namespace dyn
} // namespace vcsn
