#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/algos/dotty.hh>
#include <map>
#include <boost/type_traits.hpp>

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

    bool set(const std::string& ctx, const Fun& fn)
    {
      if (getenv("YYDEBUG"))
        std::cerr << "Register(" << name_ << ").set(" << ctx << ")\n";
      map_[ctx] = fn;
      return true;
    }

    const Fun& get(const std::string& ctx)
    {
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

  /*---------------.
  | dotty_stream.  |
  `---------------*/

  Registry<dotty_stream_t>&
  dotty_stream_registry()
  {
    static Registry<dotty_stream_t> instance{"dotty_stream"};
    return instance;
  }

  bool dotty_register(const std::string& ctx, const dotty_stream_t& fn)
  {
    return dotty_stream_registry().set(ctx, fn);
  }

  void
  dotty(const abstract_mutable_automaton& aut, std::ostream& out)
  {
    dotty_stream_registry().call(aut.abstract_context().name(),
                                 aut, out);
  }

  /*---------------.
  | dotty_string.  |
  `---------------*/

  Registry<dotty_string_t>&
  dotty_string_registry()
  {
    static Registry<dotty_string_t> instance{"dotty_string"};
    return instance;
  }

  bool dotty_register(const std::string& ctx, const dotty_string_t& fn)
  {
    return dotty_string_registry().set(ctx, fn);
  }

  std::string
  dotty(const abstract_mutable_automaton& aut)
  {
    return dotty_string_registry().call(aut.abstract_context().name(),
                                        aut);
  }
}
