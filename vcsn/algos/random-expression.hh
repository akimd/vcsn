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
      // FIXME: maybe use something similar to Boost.ProgramOptions or
      // getargs.
      void parse_param_(const std::string& param)
      {
        // Set default value.
        length_ = 6;
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
                  operators_[op] = value;
              }
            else if (op == "length")
              length_ = value;
            else
              raise("random_expression: invalid operator: ", op);
          }
        }
        proba_op_ = transform(operators_,
                              [](const auto& v){ return v.second; });
      }

      /// A string that specifies the current parameters.
      std::string make_param_(unsigned length) const
      {
        auto res = std::string{};
        for (const auto& p: operators_)
          res += p.first + "=" + std::to_string(p.second) + ", ";
        if (!random_weight_params_.empty())
          res += "w=\"" + random_weight_params_ + "\", ";
        res += "length=" + std::to_string(length);
        return res;
      }

      /// Print random weight.
      void print_weight_(std::ostream& out, const format& fmt) const
      {
        out << "<";
        es_.weightset()->print(random_weight_.generate_random_weight(), out,
                               fmt.for_weights());
        out << ">";
      }

      /// Print label.
      void print_label_(std::ostream& out, const format& fmt) const
      {
        const auto& ls = *es_.labelset();
        // Do not generate the empty label, leave it to the expression
        // generator.
        ls.print(random_label(ls, "\\e=0", gen_), out,
                 fmt.for_labels().delimit(true));
      }

      /// Print expression with unary operator.
      void print_unary_exp_(std::ostream& out, unsigned length,
                            const std::string& op, const format& fmt) const
      {
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
      void print_binary_exp_(std::ostream& out, unsigned length,
                             const std::string& op, const format& fmt) const
      {
        if (length < 3)
          print_label_(out, fmt);
        else
        {
          auto dis = std::uniform_int_distribution<>(1, length - 2);
          const auto num_lhs = dis(gen_);
          out << "(";
          print_random_expression_(out, num_lhs, fmt);
          out << op;
          print_random_expression_(out, length - 1 - num_lhs, fmt);
          out << ")";
        }
      }


      /// Print a tuple operator.
      ///
      /// GCC5 crashes on the equivalent code with static_if.
      template <typename ExpSet = expressionset_t>
      auto
      print_tuple_exp_(std::ostream& out, unsigned length,
                       const std::string& op, const format& fmt) const
        -> std::enable_if_t<context_t_of<ExpSet>::is_lat,
                            void>
      {
        assert(op == "|");
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

      /// Print a tuple operator.
      template <typename ExpSet = expressionset_t>
      auto
      print_tuple_exp_(std::ostream& out, unsigned length,
                       const std::string& op, const format& fmt) const
        -> std::enable_if_t<!context_t_of<ExpSet>::is_lat,
                            void>
      {
        assert(op == "|");
        raise(es_, "random_expression: invalid use of '|'");
      }


      std::ostream&
      print_random_expression_(std::ostream& out, unsigned length,
                               const format& fmt) const
      {
        // If there is no operators at all, that's impossible to
        // construct an expression, so just return a label.
        if (operators_.empty())
          print_label_(out, fmt);

        // One symbol left: print a label.
        else if (length == 1)
          print_label_(out, fmt);

        // All operators are possible, choose one randomly (with
        // associated weight probability) and print the associated
        // expression.
        else
        {
          // Choose an operator.
          auto op = choose_(proba_op_, operators_)->first;
          switch (arities_.at(op))
            {
            case 0:
              out << op;
              break;
            case 1:
              print_unary_exp_(out, length, op, fmt);
              break;
            case 2:
              print_binary_exp_(out, length, op, fmt);
              break;
            case 3:
              print_tuple_exp_(out, length, op, fmt);
              break;
            default:
              assert(!"invalid arity");
            }
        }
        return out;
      }

      expressionset_t es_;
      weightset_t ws_;
      unsigned length_;
      /// For each operator, its probability.
      std::map<std::string, float> operators_;
      /// Number of arguments of each operator.
      const std::unordered_map<std::string, int> arities_
      {
        // Nullary.
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

      /// Vector of weights associated with the operators, i.e., the
      /// probabilities to pick each operator.
      std::vector<float> proba_op_;
      /// Random generator.
      RandomGenerator& gen_;
      /// Random weights generator parameters.
      std::string random_weight_params_;
      /// Random weights generator.
      random_weight<weightset_t, RandomGenerator> random_weight_{gen_, ws_};
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
