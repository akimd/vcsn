#ifndef LIB_VCSN_LIB_ALGOS_REGISTRY_HH
# define LIB_VCSN_LIB_ALGOS_REGISTRY_HH

# include <iostream>
# include <map>
# include <stdexcept>

# include <vcsn/dyn/context.hh> // sname

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
    //    ~Registry() = delete;

    using map_t = std::map<std::string, Fun*>;

    bool set(std::string ctx, Fun fn)
    {
      ctx = vcsn::dyn::abstract_context::sname(ctx);
      if (getenv("YYDEBUG"))
        std::cerr << "Register(" << name_ << ").set(" << ctx << ")\n";
      map_[ctx] = fn;
      return true;
    }

    const Fun& get(std::string ctx)
    {
      ctx = vcsn::dyn::abstract_context::sname(ctx);
      if (getenv("YYDEBUG"))
        std::cerr << "Register(" << name_ << ").get(" << ctx << ")\n";
      auto i = map_.find(ctx);
      if (i == map_.end())
        throw std::runtime_error(name_
                                 + ": no implementation available for "
                                 + ctx);
      else
        return *i->second;
    }

    template <typename... Args>
    auto
    call(const std::string& ctx, Args&&... args)
      -> decltype (std::declval<Fun>()(args...))
    {
      return (get(ctx))(std::forward<Args>(args)...);
    }

  private:
    std::string name_;
    map_t map_;
  };
}

# define REGISTER_DEFINE(Name)                          \
  namespace details                                     \
  {                                                     \
    Registry<Name ## _t>&                               \
    Name ## _registry()                                 \
    {                                                   \
      static Registry<Name ## _t> instance{#Name};      \
      return instance;                                  \
    }                                                   \
                                                        \
    bool                                                \
    Name ## _register(const std::string& ctx,           \
                      Name ## _t fn)                    \
    {                                                   \
      return Name ## _registry().set(ctx, fn);          \
    }                                                   \
  }


#endif
