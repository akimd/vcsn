#ifndef LIB_VCSN_ALGOS_REGISTRY_HH
# define LIB_VCSN_ALGOS_REGISTRY_HH

# include <iostream>
# include <map>
# include <stdexcept>

# include <vcsn/dyn/translate.hh> // compile
# include <vcsn/misc/name.hh>
# include <vcsn/misc/signature.hh>
# include <vcsn/misc/raise.hh>

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
            try
              {
                vcsn::dyn::compile(name_, sig);
                auto fun = get0(sig);
                require(fun,
                        "compilation succeeded, but function is unavailable");
                return *fun;
              }
            catch (const std::runtime_error& e)
              {
                std::string err = name_ + ": no such implementation\n";
                err += "  failed signature:\n";
                  err += "    " + sig.to_string() + "\n";
                err += "  available versions:\n";
                for (auto p: map_)
                  err += "    " + p.first.to_string() + "\n";
                err += e.what();
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
        return call(vsignature(std::forward<Args>(args)...),
                    std::forward<Args>(args)...);
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
  }
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
