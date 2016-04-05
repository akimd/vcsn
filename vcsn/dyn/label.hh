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
      // A dyn label/labelset.
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

        /// Extract wrapped typed label/labelset.
        template <typename LabelSet>
        auto& as()
        {
          return dyn_cast<model<LabelSet>&>(*self_);
        }

        /// Extract wrapped typed label/labelset.
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
        /// Abstract wrapped typed label/labelset.
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

          model(const valueset_t& ls, const value_t& l)
            : valueset_(std::move(ls))
            , value_(std::move(l))
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

        /// The wrapped label/labelset.
        std::shared_ptr<base> self_;
      };
    } // namespace detail

    using label = detail::label;

  } // namespace dyn
} // namespace vcsn
