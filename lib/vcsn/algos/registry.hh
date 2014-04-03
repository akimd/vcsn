#ifndef LIB_VCSN_ALGOS_REGISTRY_HH
# define LIB_VCSN_ALGOS_REGISTRY_HH

# include <iostream>
# include <map>
# include <stdexcept>

# include <vcsn/dyn/context.hh> // sname
# include <vcsn/dyn/translate.hh> // compile
# include <vcsn/misc/name.hh>
# include <vcsn/misc/signature.hh>
# include <vcsn/misc/raise.hh>

namespace vcsn
{
  template <typename Fun>
  class Registry
  {
  public:
    /// Create a register for algo named \a n.
    Registry(const std::string& n)
      : name_(n)
    {}

    Registry() = delete;

    /// Whether log messages should be issued.
    bool debug = getenv("VCSN_DYN");

    /// Register function \a fn for signature \a sig.
    bool set(const signature& sig, Fun fn)
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

    /// Get function for signature \a sig.
    const Fun& get(const signature& sig)
    {
      // Maybe already loaded.
      if (auto fun = get0(sig))
        return *fun;
      else
        {
          // No, try to compile it.
          vcsn::dyn::compile(name_, sig);
          if (auto fun = get0(sig))
            return *fun;
          else
            {
              std::string err = (name_ + ": no implementation available for "
                                 + sig.to_string());
              err += "\n  available versions:";
              for (auto p: map_)
                err += "\n    " + p.first.to_string();
              raise(err);
            }
        }
    }

    /// Call function for signature \a sig.
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
      const auto& sig = vsignature(std::forward<Args>(args)...);
      return (get(sig))(std::forward<Args>(args)...);
    }

    template <typename T>
    auto
    call_variadic(const std::vector<T>& ts)
      -> decltype(std::declval<Fun>()(ts))
    {
      signature sig;
      for (const auto& t: ts)
        sig.sig.emplace_back(vname(t));
      return (get(sig))(ts);
    }

  private:
    /// Function name (e.g., "determinize").
    std::string name_;
    /// Context name -> pointer to implementation.
    using map_t = std::map<signature, Fun*>;
    map_t map_;
  };

}

/// Implement a registry named Name, with unary dispatch.
# define REGISTER_DEFINE(Name)                                  \
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

#endif // !LIB_VCSN_ALGOS_REGISTRY_HH
