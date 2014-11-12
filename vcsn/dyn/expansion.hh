#ifndef VCSN_DYN_EXPANSION_HH
# define VCSN_DYN_EXPANSION_HH

# include <memory>
# include <string>

# include <vcsn/misc/symbol.hh>

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
        expansion_wrapper<ExpansionSet>& as()
        {
          return dynamic_cast<expansion_wrapper<ExpansionSet>&>(*this);
        }

        template <typename ExpansionSet>
        const expansion_wrapper<ExpansionSet>& as() const
        {
          return dynamic_cast<const expansion_wrapper<ExpansionSet>&>(*this);
        }
      };

      /// Aggregate a expansion and its expansionset.
      template <typename ExpansionSet>
      class expansion_wrapper: public expansion_base
      {
      public:
        using expansionset_t = ExpansionSet;
        using super_t = expansion_base;
        using expansion_t = typename expansionset_t::value_t;
        expansion_wrapper(const expansion_t& expansion,
                          const expansionset_t& expansionset)
          : expansion_(expansion)
          , expansionset_(expansionset)
        {}
        virtual ~expansion_wrapper() {}

        virtual symbol vname() const override
        {
          return expansionset().sname();
        }

        const expansion_t expansion() const
        {
          return expansion_;
        }

        const expansionset_t& expansionset() const
        {
          return expansionset_;
        }

      protected:
        /// The expansion.
        const expansion_t expansion_;
        /// The expansion set.
        const expansionset_t expansionset_;
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
      return std::make_shared<wrapper_t>(expansion, ps);
    }
  } // namespace dyn
} // namespace vcsn

#endif // !VCSN_DYN_EXPANSION_HH
