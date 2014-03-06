#ifndef LIB_VCSN_ALGOS_REGISTRY_HH
# define LIB_VCSN_ALGOS_REGISTRY_HH

# include <iostream>
# include <map>
# include <stdexcept>

# include <vcsn/dyn/context.hh> // sname
# include <vcsn/misc/name.hh>
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

    /// A signature: the name of the input types.
    using signature_t = std::vector<std::string>;

    std::string
    to_string(const signature_t& ss) const
    {
      std::string res;
      const char* sep = "";
      for (auto s: ss)
        {
          res += sep;
          res += s;
          sep = ", ";
        }
      return res;
    }

    /// Register function \a fn for signature \a sig.
    bool set(const signature_t& sig, Fun fn)
    {
      if (debug)
        std::cerr << "Register(" << name_ << ").set(" << to_string(sig) << ")\n";
      map_[sig] = fn;
      return true;
    }

    /// Get function for signature \a sig.
    const Fun* get0(const signature_t& sig)
    {
      if (debug)
        std::cerr << "Register(" << name_ << ").get(" << to_string(sig) << ")\n";
      auto i = map_.find(sig);
      if (i == map_.end())
        return nullptr;
      else
        return i->second;
    }

    /// Get function for signature \a sig.
    const Fun& get(const signature_t& sig)
    {
      if (auto fun = get0(sig))
        return *fun;
      else
        {
          std::string err = (name_ + ": no implementation available for "
                             + to_string(sig));
          err += "\n  available versions:";
          for (auto p: map_)
            err += "\n    " + to_string(p.first);
          raise(err);
        }
    }

    /// Call function for signature \a sig.
    template <typename... Args>
    auto
    call(const signature_t& sig, Args&&... args)
      -> decltype(std::declval<Fun>()(args...))
    {
      return (get(sig))(std::forward<Args>(args)...);
    }

    template <typename... Args>
    auto
    call(Args&&... args)
      -> decltype (std::declval<Fun>()(args...))
    {
      const auto& sig = vsignature(std::forward<Args>(args)...);
      return (get(sig))(std::forward<Args>(args)...);
    }

  private:
    /// Function name (e.g., "determinize").
    std::string name_;
    /// Context name -> pointer to implementation.
    using map_t = std::map<signature_t, Fun*>;
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
    Name ## _register(const std::vector<std::string>& sig,      \
                      Name ## _t fn)                            \
    {                                                           \
      return Name ## _registry().set(sig, fn);                  \
    }                                                           \
  }

#endif // !LIB_VCSN_ALGOS_REGISTRY_HH
