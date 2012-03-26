#ifndef VCSN_CORE_RAT_FACTORY_HH
# define VCSN_CORE_RAT_FACTORY_HH

# include <string>
# include <list>

# include <vcsn/core/rat/node.fwd.hh>

namespace vcsn
{

  class factory
  {
  public:
    typedef std::string weight_str;
    typedef std::list<weight_str*> weight_str_container;
    typedef rat::exp exp_t;
    typedef exp_t* value_t;

    virtual exp_t* mul(exp_t* l, exp_t* r) const = 0;
    virtual exp_t* add(exp_t* l, exp_t* r) const = 0;
    virtual exp_t* star(exp_t* e) const = 0;
    virtual exp_t* unit() const = 0;
    virtual exp_t* zero() const = 0;
    virtual exp_t* atom(std::string* w) const = 0;
    virtual exp_t* weight(weight_str_container* w, exp_t* e) const = 0;
    virtual exp_t* weight(exp_t* e, weight_str_container* w) const = 0;

    weight_str_container* weight(weight_str* w) const;
    weight_str_container* weight(weight_str* w, weight_str_container* l) const;
    weight_str_container* weight(weight_str_container* l, weight_str* w) const;

    virtual std::ostream& print(std::ostream& o, const value_t v) const = 0;
  };

} // namespace vcsn

# include <vcsn/core/rat/factory.hxx>

#endif // !VCSN_CORE_RAT_FACTORY_HH
