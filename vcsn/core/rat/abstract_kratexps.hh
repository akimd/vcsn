#ifndef VCSN_CORE_RAT_FACTORY_HH
# define VCSN_CORE_RAT_FACTORY_HH

# include <string>
# include <list>

# include <vcsn/core/rat/node.fwd.hh>

namespace vcsn
{

  class abstract_kratexps
  {
  public:
    using exp_t = rat::exp;
    using value_t = exp_t*;

    virtual exp_t* zero() const = 0;
    virtual exp_t* unit() const = 0;
    virtual exp_t* atom(const std::string& w) const = 0;
    virtual exp_t* add(exp_t* l, exp_t* r) const = 0;
    virtual exp_t* mul(exp_t* l, exp_t* r) const = 0;
    virtual exp_t* star(exp_t* e) const = 0;
    virtual exp_t* weight(std::string* w, exp_t* e) const = 0;
    virtual exp_t* weight(exp_t* e, std::string* w) const = 0;

    virtual std::ostream& print(std::ostream& o, const value_t v) const = 0;
  };

} // namespace vcsn

#endif // !VCSN_CORE_RAT_FACTORY_HH
