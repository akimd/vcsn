#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/dyn/value.hh>

namespace vcsn 
{
  namespace tafkit
  {
    /// An enum for the type of a parameter.
    /// Values of the members represent the CLI flag
    /// used to qualify an argument.
    /// If you update this enum, please keep parse_arguments()
    /// in sync (in vcsn-tafkit.cc)
    enum class type : char
    {
        automaton = 'A',
        bool_ = 'B',
        expression = 'E',
        float_ = 'F',
        label = 'L',
        number = 'N',
        polynomial = 'P',
        string = 'S',
        unknown = 0,
        weight = 'W',
    };

    /// A structure to represent a parsed argument: its value, its type,
    // and it's input format.
    struct parsed_arg
    {
      std::string arg;
      type t;
      std::string input_format = "default";
    };

    /// A structure to represent a function from dyn algo:
    /// its "signature" (which is its formal parameters types),
    /// and a function to call it.
    struct algo
    {
      std::vector<type> signature;
      std::function<void (const std::vector<parsed_arg>&, dyn::context)> exec;
    };
    
    /// Map from an algo name to its descriptor structure.
    extern const std::unordered_multimap<std::string, algo> algos;

    // Conversion functions.
    template <typename T>
    T convert(const std::string &, const dyn::context&, const std::string& format);

    /*
    template <>
    inline T convert<T>(const std::string& str, const dyn::context& ctx, const std::string& format)
    {
      auto stream = std::istringstream(str);
      return read_T(stream);
    }
    */

#pragma GCC diagnostic ignored "-Wunused-parameter"

    template <>
    inline dyn::automaton convert<dyn::automaton>(const std::string& str, const dyn::context& ctx, const std::string& format)
    {
      std::istringstream stream{str};
      return dyn::read_automaton(stream, format);
    }

    template <>
    inline dyn::label convert<dyn::label>(const std::string& str, const dyn::context& ctx, const std::string& format)
    {
      std::istringstream stream{str};
      return dyn::read_label(ctx, stream);
    }

    template <>
    inline dyn::expression convert<dyn::expression>(const std::string& str, const dyn::context& ctx, const std::string& format)
    {
      std::istringstream stream{str};
      return dyn::read_expression(ctx, dyn::identities::deflt, stream, format);
    }

    template <>
    inline dyn::polynomial convert<dyn::polynomial>(const std::string& str, const dyn::context& ctx, const std::string& format)
    {
      std::istringstream stream{str};
      return dyn::read_polynomial(ctx, stream);
    }

    template <>
    inline dyn::weight convert<dyn::weight>(const std::string& str, const dyn::context& ctx, const std::string& format)
    {
      std::istringstream stream{str};
      return dyn::read_weight(ctx, stream);
    }

    template <>
    inline std::string convert<std::string>(const std::string& str, const dyn::context& ctx, const std::string& format)
    {
      return str;
    }

    template <>
    inline bool convert<bool>(const std::string& str, const dyn::context& ctx, const std::string& format)
    {
      std::istringstream stream{str};
      bool res;
      stream >> res;
      return res;
    }

    template <>
    inline int convert<int>(const std::string& str, const dyn::context& ctx, const std::string& format)
    {
      std::istringstream stream{str};
      int res;
      stream >> res;
      return res;
    }

    template <>
    inline float convert<float>(const std::string& str, const dyn::context& ctx, const std::string& format)
    {
      std::istringstream stream{str};
      float res;
      stream >> res;
      return res;
    }

    template <>
    inline unsigned convert<unsigned>(const std::string& str, const dyn::context& ctx, const std::string& format)
    {
      std::istringstream stream{str};
      unsigned res;
      stream >> res;
      return res;
    }
  }
} // namespace vcsn
