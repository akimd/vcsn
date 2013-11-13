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

    /// Register function \a fn for context \a ctx.
    bool set(const std::string& ctx, Fun fn)
    {
      if (debug)
        std::cerr << "Register(" << name_ << ").set(" << ctx << ")\n";
      map_[ctx] = fn;
      return true;
    }

    /// Get function for context \a ctx.
    const Fun& get(const std::string& ctx)
    {
      if (debug)
        std::cerr << "Register(" << name_ << ").get(" << ctx << ")\n";
      auto i = map_.find(ctx);
      if (i == map_.end())
        throw std::runtime_error(name_
                                 + ": no implementation available for "
                                 + ctx);
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

  private:
    /// Function name (e.g., "determinize").
    std::string name_;
    /// Context name -> pointer to implementation.
    using map_t = std::map<std::string, Fun*>;
    map_t map_;
  };

}

# define REGISTER_DEFINE_(Name)                         \
  static                                                \
  Registry<Name ## _t>&                                 \
  Name ## _registry()                                   \
  {                                                     \
    static Registry<Name ## _t> instance{#Name};        \
    return instance;                                    \
  }                                                     \

/// Implement a registry named Name, with unary dispatch.
# define REGISTER_DEFINE(Name)                          \
  namespace detail                                      \
  {                                                     \
    REGISTER_DEFINE_(Name)                              \
                                                        \
    bool                                                \
    Name ## _register(const std::string& ctx,           \
                      Name ## _t fn)                    \
    {                                                   \
      return Name ## _registry().set(ctx, fn);          \
    }                                                   \
  }

/// Implement a registry named Name, with binary dispatch.
# define REGISTER_DEFINE2(Name)                                 \
  namespace detail                                              \
  {                                                             \
    REGISTER_DEFINE_(Name)                                      \
                                                                \
    bool                                                        \
    Name ## _register(const std::string& ctx1,                  \
                      const std::string& ctx2,                  \
                      Name ## _t fn)                            \
    {                                                           \
      return Name ## _registry().set(vname(ctx1, ctx2), fn);    \
    }                                                           \
  }

#endif // !LIB_VCSN_ALGOS_REGISTRY_HH
