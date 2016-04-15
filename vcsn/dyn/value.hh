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
      /// Tag for label/labelset.
      struct label_tag
      {};

      /// Tag for weight/weightset.
      struct weight_tag
      {};

      /// A dyn value/valueset.
      template <typename tag>
      class LIBVCSN_API value
      {
      public:
        value()
          : self_()
        {}

        template <typename ValueSet>
        value(const ValueSet& ls, const typename ValueSet::value_t& l)
          : self_(std::make_shared<model<ValueSet>>(ls, l))
        {}

        /// A description of the value type.
        symbol vname() const
        {
          return self_->vname();
        }

        /// Extract wrapped typed value.
        template <typename ValueSet>
        auto& as()
        {
          return dyn_cast<model<ValueSet>&>(*self_);
        }

        /// Extract wrapped typed value.
        template <typename ValueSet>
        const auto& as() const
        {
          return dyn_cast<const model<ValueSet>&>(*self_);
        }

        value<tag>* operator->()
        {
          return this;
        }

        const value<tag>* operator->() const
        {
          return this;
        }

        bool operator!()
        {
          return !self_;
        }

        auto& operator=(const value& l)
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

        /// The wrapped value/valueset.
        std::shared_ptr<base> self_;
      };

    } // namespace detail

    // A class representing a label/labelset.
    using label = detail::value<detail::label_tag>;
    // A class representing a weight/weightset.
    using weight = detail::value<detail::weigt_tag>;

  } // namespace dyn
} // namespace vcsn