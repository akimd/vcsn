#pragma once

#include <algorithm>
#include <iostream>

#include <boost/range/algorithm/sort.hpp>

#include <lib/vcsn/dyn/translate.hh> // compile
#include <vcsn/dyn/name.hh>
#include <vcsn/misc/raise.hh>
#include <vcsn/misc/signature.hh>
#include <vcsn/misc/unordered_map.hh>

namespace vcsn
{
  namespace dyn
  {
  namespace detail
  {
    template <typename Fun>
    class Registry
    {
    public:
      /// Create a register for an algorithm.
      /// \param name     the name of the algo.
      Registry(const std::string& name)
        : name_(name)
      {}

      Registry() = delete;

      /// Whether log messages should be issued.
      bool debug = getenv("VCSN_DYN");

      /// Register function \a fn for signature \a sig.
      bool set(const signature& sig, Fun* fn)
      {
        if (debug)
          std::cerr << "Register(" << name_ << ").set(" << sig << ")\n";
        map_[sig] = fn;
        return true;
      }

      /// Get function for signature \a sig.
      const Fun* get0(const signature& sig)
      {
        if (debug)
          std::cerr << "Register(" << name_ << ").get(" << sig << ")\n";
        auto i = map_.find(sig);
        if (i == map_.end())
          return nullptr;
        else
          return i->second;
      }

      /// A message about a failed signature compilation.
      std::string signatures(const signature& sig) const
      {
        auto sigs = std::vector<std::string>();
        sigs.reserve(map_.size());
        for (auto p: map_)
          sigs.emplace_back(p.first.to_string());
        boost::sort(sigs);

        std::string res;
        res += "  failed signature:\n";
        res += "    " + sig.to_string() + "\n";
        res += "  available versions:\n";
        for (auto s: sigs)
          res += "    " + s + "\n";
        return res;
      }

      /// Get function for signature \a sig.
      const Fun* get(const signature& sig)
      {
        // Maybe already loaded.
        if (auto res = get0(sig))
          return res;
        else
          // No, try to compile it.
          {
            try
              {
                vcsn::dyn::compile(name_, sig);
              }
            catch (const jit_error& e)
              {
                raise(e.assertions.empty()
                      ? name_ + ": no such implementation\n"
                      : e.assertions,
                      signatures(sig),
                      e.what());
              }
            res = get0(sig);
            VCSN_REQUIRE(res,
                         name_,
                         ": compilation succeeded, "
                         "but function is unavailable\n",
                         signatures(sig));
            return res;
          }
      }

      /// Call function for signature \a sig.
      ///
      /// make_context needs this signature.
      template <typename... Args>
      auto
      call(const signature& sig, Args&&... args)
        -> decltype(std::declval<Fun>()(args...))
      {
        return (get(sig))(std::forward<Args>(args)...);
      }

      template <typename... Args>
      auto
      call(Args&&... args)
        -> decltype(std::declval<Fun>()(args...))
      {
        return call(vsignature(std::forward<Args>(args)...),
                    std::forward<Args>(args)...);
      }

    private:
      /// Function name (e.g., "determinize").
      std::string name_;
      /// Signature -> pointer to implementation.
      using map_t = std::unordered_map<signature, Fun*>;
      map_t map_;
    };
  }
  }
}

/// Implement a registry named Name, with unary dispatch.
#define REGISTRY_DEFINE(Name)                                   \
  namespace detail                                              \
  {                                                             \
    static                                                      \
    Registry<Name ## _t>&                                       \
    Name ## _registry()                                         \
    {                                                           \
      static Registry<Name ## _t> instance{#Name};              \
      return instance;                                          \
    }                                                           \
                                                                \
    bool                                                        \
    Name ## _register(const signature& sig, Name ## _t fn)      \
    {                                                           \
      return Name ## _registry().set(sig, fn);                  \
    }                                                           \
  }
