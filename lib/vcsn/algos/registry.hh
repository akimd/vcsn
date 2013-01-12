#ifndef LIB_VCSN_LIB_ALGOS_REGISTRY_HH
# define LIB_VCSN_LIB_ALGOS_REGISTRY_HH

# include <iostream>
# include <map>
# include <stdexcept>

# include <boost/type_traits.hpp>

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

    bool set(std::string ctx, const Fun& fn)
    {
      ctx = vcsn::dyn::context::sname(ctx);
      if (getenv("YYDEBUG"))
        std::cerr << "Register(" << name_ << ").set(" << ctx << ")\n";
      map_[ctx] = fn;
      return true;
    }

    const Fun& get(std::string ctx)
    {
      ctx = vcsn::dyn::context::sname(ctx);
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
    // I failed to find a means in plain C++...
      -> typename boost::function_traits<Fun>::result_type
    {
      return (get(ctx))(std::forward<Args>(args)...);
    }

  private:
    std::string name_;
    map_t map_;
  };
}

#endif
