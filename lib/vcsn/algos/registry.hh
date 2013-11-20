#ifndef LIB_VCSN_ALGOS_REGISTRY_HH
# define LIB_VCSN_ALGOS_REGISTRY_HH

# include <iostream>
# include <map>
# include <stdexcept>

# include <vcsn/dyn/context.hh> // sname
# include <vcsn/misc/name.hh>

namespace vcsn
{
  template <typename Fun>
  class Registry
  {
  public:
    Registry(const std::string& n)
      : name_(n)
    {}

    Registry() = delete;

    /// Whether log messages should be issued.
    bool debug = getenv("VCSN_DYN");

    /// Register function \a fn for signature \a sig.
    bool set(const std::string& sig, Fun fn)
    {
      if (debug)
        std::cerr << "Register(" << name_ << ").set(" << sig << ")\n";
      map_[sig] = fn;
      return true;
    }

    /// Get function for signature \a sig.
    const Fun& get(const std::string& sig)
    {
      if (debug)
        std::cerr << "Register(" << name_ << ").get(" << sig << ")\n";
      auto i = map_.find(sig);
      if (i == map_.end())
        {
          std::string err = name_ + ": no implementation available for " + sig;
          err += "\n  available versions:";
          for (auto p: map_)
            err += "\n    " + p.first;
          throw std::runtime_error(err);
        }
      else
        return *i->second;
    }

    /// Call function for context \a ctx.
    template <typename... Args>
    auto
    call(const std::string& ctx, Args&&... args)
      -> decltype (std::declval<Fun>()(args...))
    {
      return (get(ctx))(std::forward<Args>(args)...);
    }

    /// Call function for context \a ctx.
    template <typename... Args>
    auto
    call(Args&&... args)
      -> decltype (std::declval<Fun>()(args...))
    {
      const auto& sig = vname(std::forward<Args>(args)...);
      return (get(sig))(std::forward<Args>(args)...);
    }

  private:
    /// Function name (e.g., "determinize").
    std::string name_;
    /// Context name -> pointer to implementation.
    using map_t = std::map<std::string, Fun*>;
    map_t map_;
  };

}

/// Implement a registry named Name, with unary dispatch.
# define REGISTER_DEFINE(Name)                          \
  namespace detail                                      \
  {                                                     \
    static                                              \
    Registry<Name ## _t>&                               \
    Name ## _registry()                                 \
    {                                                   \
      static Registry<Name ## _t> instance{#Name};      \
      return instance;                                  \
    }                                                   \
                                                        \
    bool                                                \
    Name ## _register(const std::string& type,          \
                      Name ## _t fn)                    \
    {                                                   \
      return Name ## _registry().set(type, fn);         \
    }                                                   \
  }

#endif // !LIB_VCSN_ALGOS_REGISTRY_HH
