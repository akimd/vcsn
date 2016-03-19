#pragma once

#include <memory>
#include <string>

#include <vcsn/misc/export.hh>

namespace vcsn
{
  namespace dyn
  {
    namespace detail
    {
      class LIBVCSN_API label
      {
      public:
        label()
          : self_()
        {}

        template <typename LabelSet>
        label(const LabelSet& ls, const typename LabelSet::value_t& l)
          : self_(std::make_shared<model<LabelSet>>(ls, l))
        {}

        /// A description of the label type.
        symbol vname() const
        {
          return self_->vname();
        }

        /// Extract wrapped typed label.
        template <typename LabelSet>
        auto& as()
        {
          return dyn_cast<model<LabelSet>&>(*self_);
        }

        /// Extract wrapped typed label.
        template <typename LabelSet>
        const auto& as() const
        {
          return dyn_cast<const model<LabelSet>&>(*self_);
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

        auto& operator=(const label& l)
        {
          self_ = std::move(l.self_);
          return *this;
        }

      private:
        /// Abstract wrapped typed automaton.
        struct base
        {
          virtual ~base() = default;
          virtual symbol vname() const = 0;
        };

        /// A wrapped typed automaton.
        template <typename LabelSet>
        struct model final : base
        {
          using labelset_t = LabelSet;
          using label_t = typename labelset_t::value_t;

          model(const labelset_t& ls, const label_t& l)
            : labelset_(std::move(ls))
            , label_(std::move(l))
          {}

          virtual symbol vname() const override
          {
            return labelset().sname();
          }

          const labelset_t& labelset() const
          {
            return labelset_;
          }

          const label_t label() const
          {
            return label_;
          }

        private:
          /// The label set.
          const labelset_t labelset_;
          /// The label.
          const label_t label_;
        };

        /// The wrapped LabelSet.
        std::shared_ptr<base> self_;
      };
    } // namespace detail

    using label = detail::label;

  } // namespace dyn
} // namespace vcsn
