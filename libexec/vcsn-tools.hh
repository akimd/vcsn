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
#include <vcsn/misc/stream.hh>

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
      parsed_arg(std::string arg_, bool file_,
                 type t_, std::string input_format_)
        : arg{std::move(arg_)}
        , file{file_}
        , t{t_}
        , input_format{std::move(input_format_)}
      {}
      parsed_arg() = default;

      std::string arg;
      /// Whether arg is a file name (-f) instead of actual contents (-e).
      bool file;
      type t;
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
    struct convert_impl
    {
      static
      auto conv(std::istream& in,
                const dyn::context&, const std::string&)
        -> T
      {
        auto res = T{};
        in >> res;
        return res;
      }
    };

    template <typename T>
    struct convert_impl<boost::optional<T>>
    {
      static
      boost::optional<T>
      conv(std::istream& in,
           const dyn::context& ctx, const std::string& fmt)
      {
        return convert_impl<T>::conv(in, ctx, fmt);
      }
    };

    template <>
    struct convert_impl<std::string>
    {
      static
      std::string
      conv(std::istream& in,
           const dyn::context&, const std::string&)
      {
        return std::string{std::istreambuf_iterator<char>{in}, {}};
      }
    };

    template <>
    struct convert_impl<dyn::automaton>
    {
      static
      dyn::automaton
      conv(std::istream& in,
           const dyn::context&, const std::string& fmt)
      {
        return dyn::read_automaton(in, fmt);
      }
    };

    template <>
    struct convert_impl<dyn::expression>
    {
      static
      dyn::expression
      conv(std::istream& in,
           const dyn::context& ctx, const std::string& fmt)
      {
        return dyn::read_expression(ctx, dyn::identities::deflt, in, fmt);
      }
    };

    template <>
    struct convert_impl<dyn::label>
    {
      static
      dyn::label
      conv(std::istream& in,
           const dyn::context& ctx, const std::string& fmt)
      {
        return dyn::read_label(ctx, in, fmt);
      }
    };


    template <>
    struct convert_impl<dyn::polynomial>
    {
      static
      dyn::polynomial
      conv(std::istream& in,
           const dyn::context& ctx, const std::string&)
      {
        return dyn::read_polynomial(ctx, in);
      }
    };


    template <>
    struct convert_impl<dyn::weight>
    {
      static
      dyn::weight
      conv(std::istream& in,
           const dyn::context& ctx, const std::string&)
      {
        return dyn::read_weight(ctx, in);
      }
    };

    template <typename T>
    T
    convert(const parsed_arg& arg, const dyn::context& ctx)
    {
      auto in = [&]() -> std::shared_ptr<std::istream>
        {
          if (arg.file)
            return open_input_file(arg.arg);
          else
            return std::make_shared<std::istringstream>(arg.arg);
        }();
      return convert_impl<T>::conv(*in, ctx, arg.input_format);
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
