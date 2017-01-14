#pragma once

#include <iostream>
#include <map>
#include <memory> // std::make_shared
#include <string>
#include <type_traits>
#include <vector>

#include <boost/algorithm/string/erase.hpp>
#include <boost/tokenizer.hpp>

#include <vcsn/misc/algorithm.hh>
#include <vcsn/misc/cast.hh>
#include <vcsn/misc/functional.hh>
#include <vcsn/misc/random.hh>
#include <vcsn/misc/type_traits.hh> // detect

// It is much simpler and saner in C++ to put types and functions on
// these types in the same namespace.  Since "using q =
// detail::weightset_mixin<q_impl>" would just create an alias of
// q, its original namespace, detail::, would still be the namespace
// used in ADL.
//
// This is really troublesome to implement free-functions such as join.
//
// Therefore, although this wrapper should be hidden as a detail::, it
// will remain in vcsn::, where join and the like will find it.

namespace vcsn
{
  /*-------------------.
  | weightset_mixin.   |
  `-------------------*/
  namespace detail
  {
    /// The signature of power.
    template <typename T>
    using power_mem_fn_t
      = decltype(std::declval<T>()
                 .power(std::declval<typename T::value_t>(), 0));

    /// Whether T features a power member function.
    template <typename T>
    using has_power_mem_fn = detail::detect<T, power_mem_fn_t>;
  }

  /// Provide a variadic mul on top of a binary mul(), and one().
  template <typename WeightSet>
  struct weightset_mixin : WeightSet
  {
    using super_t = WeightSet;
    using typename super_t::value_t;

    /// Inherit the constructors.
    using super_t::super_t;

    /// Import mul overloads.
    using super_t::mul;

    /// A variadic multiplication.
    template <typename... Ts>
    value_t mul(const Ts&... ts) const
    {
      value_t res = this->one();
      // FIXME: Remove once GCC is fixed.
      using swallow = int[];
      (void) swallow
        {
          ((res = super_t::mul(res, ts)), 0)...
        };
      return res;
    }

  private:
    /// Case where the weightset T features a power(value_t, unsigned)
    /// member function.
    template <typename WS = super_t>
    auto power_(value_t e, unsigned n) const
      -> std::enable_if_t<detail::has_power_mem_fn<WS>{}, value_t>
    {
      return super_t::power(e, n);
    }

    /// Case where the weightset T does not feature a
    /// power(value_t, unsigned) member function.
    template <typename WS = super_t>
    auto power_(value_t e, unsigned n) const
      -> std::enable_if_t<!detail::has_power_mem_fn<WS>{}, value_t>
    {
      value_t res = super_t::one();
      if (!super_t::is_one(e))
        while (n--)
          res = mul(res, e);
      return res;
    }

  public:

    /// Repeated multiplication.
    value_t power(value_t e, unsigned n) const
    {
      return power_<WeightSet>(e, n);
    }
  };


  /*-----------------.
  | random_weight.   |
  `-----------------*/
  namespace detail
  {
    /// Abstract class for random weight generation.
    /// It contains the parsing of the arguments (which can be overridden),
    /// the map of probabilities, the min and the max.
    template <typename WeightSet,
              typename RandomGenerator = std::default_random_engine>
    class random_weight_base
    {
    public:
      using weightset_t = WeightSet;
      using weight_t = typename weightset_t::value_t;
      using random_generator_t = RandomGenerator;

      random_weight_base(random_generator_t& gen, const weightset_t& ws)
        : gen_{gen}
        , ws_{ws}
        , min_{ws.min()}
        , max_{ws.max()}
      {}

      void parse_param(const std::string& param)
      {
        if (!param.empty())
          parse_param_(param);
      }

      weight_t generate_random_weight() const
      {
        return print_random_weight_();
      }

    protected:
      virtual void parse_param_(const std::string& weights)
      {
        using tokenizer = boost::tokenizer<boost::char_separator<char>>;
        using boost::algorithm::erase_all_copy;
        auto sep = boost::char_separator<char>{","};
        for (const auto& arg: tokenizer(weights, sep))
        {
          auto eq = arg.find('=');
          auto weight = erase_all_copy(arg.substr(0, eq), " ");
          if (weight == "min")
            min_ = conv(ws_, arg.substr(eq + 1));
          else if (weight == "max")
            max_ = conv(ws_, arg.substr(eq + 1));
          else
          {
            float value = (eq != std::string::npos)
              ? detail::lexical_cast<float>(arg.substr(eq + 1))
              : 1;
            auto w = conv(ws_, weight);
            weight_[w] = value;
          }
        }

        weight_weight_
          = detail::transform(weight_, [](const auto& v){ return v.second; });
      }

      virtual weight_t pick_value_() const = 0;

      /// A random weight.
      weight_t print_random_weight_() const
      {
        if (choose_map(weight_weight_, gen_))
        {
          /// There is a biased probability to choose a value
          /// specified on the parameters.
          auto it = chooser_it_(weight_weight_, weight_);
          return it->first;
        }
        /// Otherwise, choose a random value from the WeightSet.
        else
        {
          return pick_value_();
        }
      }

      random_generator_t& gen_;
      weightset_t ws_;
      /// The min and the max given by the user.  If unspecified, then
      /// it takes the min and max of the weighset by default.
      weight_t min_;
      weight_t max_;
      /// Elements given by the user and their associated
      /// probabilities (weight_weight_).
      using weight_map_t = std::map<weight_t, float, vcsn::less<weightset_t>>;
      weight_map_t weight_;
      using weight_weight_t = std::vector<float>;
      weight_weight_t weight_weight_;
      discrete_chooser<random_generator_t> chooser_it_{gen_};
    };

    /// Generic declaration of the class which is specialized
    /// in each weightset.
    template <typename WeightSet, typename RandomGenerator>
    class random_weight;
  }

  /*----------.
  | traits.   |
  `----------*/

  // FIXME: find generic implementation for min-plus.
  template <typename T>
  struct is_tropical : std::false_type
  {};

  /*----------.
  | Errors.   |
  `----------*/

  /// Cannot make a value from this.
  template <typename ValueSet, typename... Args>
  ATTRIBUTE_NORETURN
  void raise_invalid_value(const ValueSet& vs, Args&&... args)
  {
    raise(vs, ": invalid value: ", std::forward<Args>(args)...);
  }

  /// This value is not starrable.
  template <typename WeightSet>
  ATTRIBUTE_NORETURN
  void raise_not_starrable(const WeightSet& ws,
                           const typename WeightSet::value_t& w)
  {
    raise(ws, ": value is not starrable: ", to_string(ws, w));
  }
}
