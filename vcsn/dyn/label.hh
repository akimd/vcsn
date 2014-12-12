#ifndef VCSN_DYN_LABEL_HH
# define VCSN_DYN_LABEL_HH

# include <memory>
# include <string>

# include <vcsn/misc/export.hh>

namespace vcsn
{
  namespace dyn
  {
    namespace detail
    {

      /// An abstract label.
      class LIBVCSN_API label_base
      {
      public:
        /// A description of the label type.
        virtual symbol vname() const = 0;

        /// Extract wrapped typed label.
        template <typename LabelSet>
        auto& as()
        {
          return dyn_cast<label_wrapper<LabelSet>&>(*this);
        }

        /// Extract wrapped typed label.
        template <typename LabelSet>
        auto& as() const
        {
          return dyn_cast<const label_wrapper<LabelSet>&>(*this);
        }
      };

      /// Aggregate a label and its labelset.
      template <typename LabelSet>
      class label_wrapper: public label_base
      {
      public:
        using labelset_t = LabelSet;
        using super_t = label_base;
        using label_t = typename labelset_t::value_t;

        label_wrapper(const label_t& l, const labelset_t& ls)
          : label_(l)
          , labelset_(ls)
        {}

        virtual symbol vname() const override
        {
          return labelset().sname();
        }

        const label_t label() const
        {
          return label_;
        }

        const labelset_t& labelset() const
        {
          return labelset_;
        }

      private:
        /// The label.
        const label_t label_;
        /// The label set.
        const labelset_t labelset_;
      };

    } // namespace detail

    using label = std::shared_ptr<const detail::label_base>;

    template <typename LabelSet>
    inline
    label
    make_label(const LabelSet& ls,
               const typename LabelSet::value_t& l)
    {
      return std::make_shared<detail::label_wrapper<LabelSet>>(l, ls);
    }

    template <typename LabelSet>
    inline
    label
    make_word(const LabelSet& ls,
              const typename LabelSet::value_t& l)
    {
      return make_label(make_wordset(ls), l);
    }
  } // namespace dyn
} // namespace vcsn

#endif // !VCSN_DYN_LABEL_HH
