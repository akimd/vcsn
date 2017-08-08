#pragma once

#include <functional>
#include <map>
#include <string>
#include <vector>

#include <boost/optional.hpp>

#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/dyn/value.hh>

namespace vcsn
{
  namespace tools
  {
    /// An enum for the type of a parameter.
    /// Values of the members represent the CLI flag
    /// used to qualify an argument.
    /// If you update this enum, please keep parse_arguments()
    /// in sync (in vcsn-tools.cc)
    enum class type : char
    {
      automaton = 'A',
      bool_ = 'B',
      expression = 'E',
      float_ = 'F',
      identities = 'D',
      label = 'L',
      number = 'N',
      polynomial = 'P',
      string = 'S',
      unknown = 0,
      weight = 'W',
    };

    /// A parsed argument: its value, its type, and its input format.
    struct parsed_arg
    {
      parsed_arg(std::string arg_, type t_, std::string input_format_)
        : arg{std::move(arg_)}
        , t{t_}
        , input_format{std::move(input_format_)}
      {}
      parsed_arg() = default;

      std::string arg;
      type t;
      // Can't use a default initializer here as GCC 4.9 doesn't
      // support them in aggregate types.
      std::string input_format;
    };

    /// A function from dyn algo: its "signature" (its formal
    /// parameters types), and a function to call it.
    struct algo
    {
      std::vector<type> signature;
      std::string declaration;
      std::function<void (const std::vector<parsed_arg>&, dyn::context)> exec;
    };

    /// The documentation of an algorithm from dyn algos:
    /// Separate from struct algo because it is not duplicated when optional
    /// parameters are involved.
    struct algo_doc
    {
      std::string declaration;
      std::string doc;
    };

    /// Map from an algo name to its descriptor structure.
    extern const std::multimap<std::string, algo> algos;
    extern const std::multimap<std::string, algo_doc> algos_doc;

    /// Conversion functions.
    template <typename T>
    T convert(const std::string &, const dyn::context&,
              const std::string& format);

    /*
    template <>
    inline T convert<T>(const std::string& str, const dyn::context& ctx,
                        const std::string& format)
    {
      auto stream = std::istringstream(str);
      return read_T(stream);
    }
    */

    template <>
    inline dyn::automaton
    convert<dyn::automaton>(const std::string& str, const dyn::context&,
                            const std::string& format)
    {
      return dyn::make_automaton(str, format);
    }

    template <>
    inline dyn::expression
    convert<dyn::expression>(const std::string& str, const dyn::context& ctx,
                             const std::string& format)
    {
      return dyn::make_expression(ctx, str, dyn::identities::deflt, format);
    }

    template <>
    inline dyn::label
    convert<dyn::label>(const std::string& str, const dyn::context& ctx,
                        const std::string& format)
    {
      return dyn::make_label(ctx, str, format);
    }

    template <>
    inline dyn::polynomial
    convert<dyn::polynomial>(const std::string& str, const dyn::context& ctx,
                             const std::string&)
    {
      return dyn::make_polynomial(ctx, str);
    }

    template <>
    inline dyn::weight
    convert<dyn::weight>(const std::string& str, const dyn::context& ctx,
                         const std::string&)
    {
      return dyn::make_weight(ctx, str);
    }

    template <>
    inline std::string
    convert<std::string>(const std::string& str, const dyn::context&,
                         const std::string&)
    {
      return str;
    }

    template <>
    inline bool
    convert<bool>(const std::string& str, const dyn::context&,
                  const std::string&)
    {
      std::istringstream stream{str};
      bool res;
      stream >> res;
      return res;
    }

    template <>
    inline int
    convert<int>(const std::string& str, const dyn::context&,
                 const std::string&)
    {
      std::istringstream stream{str};
      int res;
      stream >> res;
      return res;
    }

    template <>
    inline float
    convert<float>(const std::string& str, const dyn::context&,
                   const std::string&)
    {
      std::istringstream stream{str};
      float res;
      stream >> res;
      return res;
    }

    template <>
    inline unsigned
    convert<unsigned>(const std::string& str, const dyn::context&,
                      const std::string&)
    {
      std::istringstream stream{str};
      unsigned res;
      stream >> res;
      return res;
    }

    template <>
    inline boost::optional<unsigned>
    convert<boost::optional<unsigned>>(const std::string& str,
                                       const dyn::context&,
                                       const std::string&)
    {
      std::istringstream stream{str};
      unsigned res;
      stream >> res;
      return res;
    }

    template <>
    inline dyn::identities
    convert<dyn::identities>(const std::string& str, const dyn::context&,
                             const std::string&)
    {
      return str;
    }

    template <typename T>
    T
    convert(const parsed_arg& arg, const dyn::context& ctx)
    {
      return vcsn::tools::convert<T>(arg.arg, ctx, arg.input_format);
    }
  }

  namespace dyn
  {
    template <typename T>
    const T& cat(const T& arg)
    {
      return arg;
    }
  }
} // namespace vcsn
