#include <ostream>
#include <regex>
#include <string>

#include <boost/algorithm/string/erase.hpp>
#include <boost/tokenizer.hpp>

#include <vcsn/core/rat/expressionset.hh> // make_expressionset
#include <vcsn/dyn/algos.hh>
#include <vcsn/misc/algorithm.hh>
#include <vcsn/misc/cast.hh>
#include <vcsn/misc/raise.hh>
#include <vcsn/misc/random.hh>
#include <vcsn/misc/tuple.hh>
#include <vcsn/misc/unordered_map.hh>

namespace vcsn
{
  template <typename ExpressionSet>
  std::string
  random_expression_string(const ExpressionSet& es, const std::string& param);

  namespace detail
  {
    /// Random expression generator.
    ///
    /// \tparam ExpressionSet the expression set type.
    /// \tparam RandomGenerator the random number engine type.
    template <typename ExpressionSet,
              typename RandomGenerator = std::default_random_engine>
    class random_expression_impl
    {
    public:
      using expressionset_t = ExpressionSet;
      using expression_t    = typename expressionset_t::value_t;
      using weight_t        = typename expressionset_t::weight_t;
      using weightset_t     = typename expressionset_t::weightset_t;

      random_expression_impl(const expressionset_t& es,
                             const std::string& param,
                             RandomGenerator& gen)
        : es_{es}
        , gen_{gen}
      {
        parse_param_(param);
      }

      /// Print a random expression string (not parsed, so there might
      /// be some syntactic sugar such as `<+`).
      std::ostream& print_random_expression(std::ostream& out,
                                            const format& fmt = {}) const
      {
        return print_random_expression_(out, length_, fmt);
      }

      /// A random expression string (not parsed, so there might be
      /// some syntactic sugar such as `<+`).
      std::string random_expression_string() const
      {
        std::ostringstream out;
        print_random_expression(out);
        return out.str();
      }

      /// A random expression (parsed, so there cannot be syntactic
      /// sugar such as `<+`).
      expression_t random_expression() const
      {
        return conv(es_, random_expression_string());
      }

    private:
      // FIXME: use something similar to Boost.ProgramOptions or
      // getargs.
      void parse_param_(std::string param)
      {
        using namespace std::literals;
        if (param.empty() || param == "default")
          param = "\\e=.2, +, ., *=.2";
        param = "length=10, l"s + (param.empty() ? ""s : ", "s) + param;
        using tokenizer = boost::tokenizer<boost::escaped_list_separator<char>>;
        using boost::algorithm::erase_all_copy;
        const auto sep
          = boost::escaped_list_separator<char>("#####", ",", "\"");
        for (const auto& arg: tokenizer(param, sep))
        {
          const auto eq = arg.find('=');
          const auto op = erase_all_copy(arg.substr(0, eq), " ");
          if (op == "w")
            {
              random_weight_params_ = arg.substr(eq + 1);
              random_weight_.parse_param(random_weight_params_);
            }
          else
          {
            const float value = (eq != std::string::npos)
              ? detail::lexical_cast<float>(arg.substr(eq + 1))
              : 1;
            if (has(arities_, op))
              {
                // Ignore operators that don't make sense.
                if ((op != "|" && op != "@")
                    || context_t_of<expressionset_t>::is_lat)
                  {
                    if (arities_.at(op) == 0)
                      nullary_ops_[op] = value;
                    else if (arities_.at(op) == 1)
                      {
                        unary_ops_[op] = value;
                        binary_or_unary_ops_[op] = value;
                      }
                    else
                      binary_or_unary_ops_[op] = value;
                  }
              }
            else if (op == "length")
              length_ = value;
            else
              raise("random_expression: invalid operator: ", str_quote(op));
          }
        }
        VCSN_REQUIRE(!nullary_ops_.empty(),
                     "at least one leaf expression (\"l\", \"\\e\", \"\\z\")",
                     " is needed: ",
                     str_quote(param));
        VCSN_REQUIRE(!binary_or_unary_ops_.empty(),
                     "at least one operator is needed: ",
                     str_quote(param));
      }

      /// A string that specifies the current parameters.
      std::string make_param_(unsigned length) const
      {
        auto res = std::string{};
        for (const auto& p: nullary_ops_)
          res += p.first + "=" + std::to_string(p.second) + ", ";
        for (const auto& p: binary_or_unary_ops_)
          res += p.first + "=" + std::to_string(p.second) + ", ";
        if (!random_weight_params_.empty())
          res += "w=\"" + random_weight_params_ + "\", ";
        res += "length=" + std::to_string(length);
        return res;
      }

      /// Print random weight.
      void print_weight_(std::ostream& out, const format& fmt) const
      {
        out << '<';
        es_.weightset()->print(random_weight_(), out,
                               fmt.for_weights());
        out << '>';
      }

      /// Print expression leaf.
      void print_nullary_(std::ostream& out, unsigned length,
                          const std::string& op, const format& fmt) const
      {
        assert(1 == length);
        if (op == "l")
          {
            const auto& ls = *es_.labelset();
            // Do not generate the empty label, leave it to the expression
            // generator.
            ls.print(random_label(ls, "\\e=0", gen_), out,
                     fmt.for_labels().delimit(true));
          }
        else
          out << op;
      }

      /// Print expression with unary operator.
      void print_unary_(std::ostream& out, unsigned length,
                        const std::string& op, const format& fmt) const
      {
        assert(2 <= length);
        // Prefix.
        if (op == "!" || op == "w.")
        {
          out << '(';
          if (op == "w.")
            print_weight_(out, fmt);
          else
            out << op;
          print_random_expression_(out, length - 1, fmt);
          out << ')';
        }
        // Postfix.
        else
        {
          out << '(';
          print_random_expression_(out, length - 1, fmt);
          if (op == ".w")
            print_weight_(out, fmt);
          else
            out << op;
          out << ")";
        }
      }

      /// Print binary expression with binary operator.
      /// It is composed of the left and right side, and the operator.
      /// The number of symbols is randomly distribued between both side.
      void print_binary_(std::ostream& out, unsigned length,
                         const std::string& op, const format& fmt) const
      {
        assert(3 <= length);
        auto dis = std::uniform_int_distribution<>(1, length - 2);
        const auto num_lhs = dis(gen_);
        out << "(";
        print_random_expression_(out, num_lhs, fmt);
        out << op;
        print_random_expression_(out, length - 1 - num_lhs, fmt);
        out << ")";
      }


      /// Print a tuple operator.
      template <typename ExpSet = expressionset_t>
      auto
      print_tuple_(std::ostream& out, unsigned length,
                   const std::string& op, const format& fmt) const
        -> void
      {
        assert(op == "|");
        if constexpr (context_t_of<ExpSet>::is_lat)
          {
            const auto len = length / es_.labelset()->size();
            const auto param = make_param_(std::max(size_t{1}, len));
            out << '(';
            const auto* sep = "";
            es_.as_tupleset().map([&out, &param, &sep](const auto& subes)
               {
                 out << sep
                     << '('
                     << vcsn::random_expression_string(subes, param)
                     << ')';
                 sep = "|";
                 // Please make_tuple.
                 return 0;
               });
            out << ')';
          }
        else
          raise(es_, "random_expression: invalid use of '|'");
      }

      std::ostream&
      print_random_expression_(std::ostream& out, unsigned length,
                               const format& fmt) const
      {
        // Choose an operator.
        const auto& ops = [&]
          {
            if (length == 1)
              return nullary_ops_;
            else if (length == 2)
              // If need a unary but there is none, go for a nullary.
              return unary_ops_.empty() ? nullary_ops_ : unary_ops_;
            else // 3 <= length
              return binary_or_unary_ops_;
          }();
        auto op = choose_(ops)->first;
        switch (arities_.at(op))
          {
          case 0:
            print_nullary_(out, length, op, fmt);
            break;
          case 1:
            print_unary_(out, length, op, fmt);
            break;
          case 2:
            print_binary_(out, length, op, fmt);
            break;
          case 3:
            print_tuple_(out, length, op, fmt);
            break;
          default:
            assert(!"invalid arity");
          }
        return out;
        }


      expressionset_t es_;
      weightset_t ws_;
      unsigned length_;
      /// For each operator, its probability.
      std::map<std::string, float> nullary_ops_;
      std::map<std::string, float> unary_ops_;
      std::map<std::string, float> binary_or_unary_ops_;
      /// Number of arguments of each operator.
      const std::unordered_map<std::string, int> arities_
      {
        // Nullary.
        {"l", 0},
        {"\\e", 0},
        {"\\z", 0},
        // Unary.
        {"!", 1},
        {"{c}", 1},
        {"*", 1},
        {"{T}", 1},
        {"w.", 1},
        {".w", 1},
        // Binary.
        {"&", 2},
        {"&:", 2},
        {":", 2},
        {".", 2},
        {"<+", 2},
        {"%", 2},
        {"+", 2},
        {"{/}", 2},
        {"{\\}",2},
        {"@", 2},
        // Special.
        {"|", 3},
      };

      /// Random generator.
      RandomGenerator& gen_;
      /// Random weights generator parameters.
      std::string random_weight_params_;
      /// Random weights generator.
      random_weight<weightset_t, RandomGenerator> random_weight_{ws_, gen_};
      /// Random selection in containers.
      discrete_chooser<RandomGenerator> choose_{gen_};
    };

    /// Convenience constructor.
    template <typename ExpressionSet, typename RandomGenerator = std::mt19937>
    random_expression_impl<ExpressionSet, RandomGenerator>
    make_random_expression_impl(const ExpressionSet& es,
                                const std::string& param,
                                RandomGenerator& gen = make_random_engine())
    {
      return {es, param, gen};
    }
  } // end namespace vcsn::detail


  /// Generate a random expression string.
  ///
  /// Return a string. This allows doing easily constructs that are
  /// not possible in the AST in case we want them (i.e [a-z] ranges).
  /// Furthermore, the user can then choose the identities they want
  /// to apply to the resulting expression.
  template <typename ExpressionSet>
  std::string
  random_expression_string(const ExpressionSet& es, const std::string& param)
  {
    auto random_exp = detail::make_random_expression_impl(es, param);
    return random_exp.random_expression_string();
  }


  /// Generate a random expression.
  template <typename ExpressionSet>
  typename ExpressionSet::value_t
  random_expression(const ExpressionSet& es, const std::string& param)
  {
    auto random_exp = detail::make_random_expression_impl(es, param);
    return random_exp.random_expression();
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Context, typename String, typename Identities>
      expression
      random_expression(const context& ctx, const std::string& param,
                        identities ids)
      {
        const auto& c = ctx->as<Context>();
        auto es = make_expressionset(c, ids);
        return {es, random_expression(es, param)};
      }
    }
  }
} // end namespace vcsn
