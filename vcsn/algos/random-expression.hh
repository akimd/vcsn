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
#include <vcsn/misc/unordered_map.hh>

namespace vcsn
{
  namespace detail
  {

    /// Class random expression generator.
    /// \brief Generate a random expression from a context.
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

      random_expression_impl(const expressionset_t& rs,
                             const std::string& param,
                             RandomGenerator& gen)
        : rs_{rs}
        , gen_{gen}
      {
        parse_param_(param);
      }

      /// Print a random expression string (not parsed, so there might
      /// be some syntactic sugar such as `<+`).
      std::ostream& print_random_expression(std::ostream& out) const
      {
        return print_random_expression_(out, length_);
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
        return conv(rs_, random_expression_string());
      }

    private:
      /// FIXME: maybe use something similar to Boost.ProgramOptions
      /// or getargs.
      void parse_param_(const std::string& param)
      {
        // Set default value.
        length_ = 6;
        using tokenizer = boost::tokenizer<boost::escaped_list_separator<char>>;
        auto sep = boost::escaped_list_separator<char>("#####", ",", "\"");
        auto tok = tokenizer(param, sep);
        for (auto it = tok.begin(); it != tok.end(); ++it)
        {
          auto tok_arg = std::string{*it};
          auto eq = tok_arg.find_first_of('=');
          auto op = tok_arg.substr(0, eq);
          boost::algorithm::erase_all(op, " ");
          if (op == "w")
            random_weight_.parse_param(tok_arg.substr(eq + 1));
          else
          {
            float value = 1;
            if (eq != std::string::npos)
              value = lexical_cast<float>(tok_arg.substr(eq + 1));
            if (has(arities_, op))
              operators_[op] = value;
            else if (op == "length")
              length_ = value;
            else
              raise("random_expression: invalid operator: ", op);
          }
        }
        proba_op_ = transform(operators_,
                              [](const auto& v){ return v.second; });
      }

      /// Print random weight.
      void print_weight_(std::ostream& out) const
      {
        out << "<";
        rs_.weightset()->print(random_weight_.generate_random_weight(), out);
        out << ">";
      }

      /// Print label.
      void print_label_(std::ostream& out) const
      {
        rs_.labelset()->print(random_label(*rs_.labelset(), gen_), out);
      }

      /// Print expression with unary operator.
      void print_unary_exp_(std::ostream& out, unsigned length,
                            const std::string& op) const
      {
        // Prefix.
        if (op == "!" || op == "w.")
        {
          out << '(';
          if (op == "w.")
            print_weight_(out);
          else
            out << op;
          print_random_expression_(out, length - 1);
          out << ')';
        }
        // Postfix.
        else
        {
          out << '(';
          print_random_expression_(out, length - 1);
          if (op == ".w")
            print_weight_(out);
          else
            out << op;
          out << ")";
        }
      }

      /// Print binary expression with binary operator.
      /// It is composed of the left and right side, and the operator.
      /// The number of symbols is randomly distribued between both side.
      void print_binary_exp_(std::ostream& out, unsigned length,
                             const std::string& op) const
      {
        if (length < 3)
          print_label_(out);
        else
        {
          auto dis = std::uniform_int_distribution<>(1, length - 2);
          auto num_lhs = dis(gen_);
          out << "(";
          print_random_expression_(out, num_lhs);
          out << op;
          print_random_expression_(out, length - 1 - num_lhs);
          out << ")";
        }
      }

      std::ostream&
      print_random_expression_(std::ostream& out, unsigned length) const
      {
        // If there is no operators at all, that's impossible to
        // construct an expression, so just return a label.
        if (operators_.empty())
          print_label_(out);

        // One symbol left: print a label.
        else if (length == 1)
          print_label_(out);

        // All operators are possible, choose one randomly (with
        // associated weight probability) and print the associated
        // expression.
        else
        {
          // Choose an operator.
          auto op = chooser_it_(proba_op_, operators_)->first;
          switch (arities_.at(op))
            {
            case 0:
              out << op;
              break;
            case 1:
              print_unary_exp_(out, length, op);
              break;
            case 2:
              print_binary_exp_(out, length, op);
              break;
            default:
              assert(!"invalid arity");
            }
        }
        return out;
      }

      expressionset_t rs_;
      weightset_t ws_;
      unsigned length_;
      /// For each operator, its probability.
      std::map<std::string, float> operators_;
      /// Number of arguments of each operator.
      std::unordered_map<std::string, int> arities_
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
      };

      /// Vector of weights associated with the operators, i.e., the
      /// probabilities to pick each operator.
      std::vector<float> proba_op_;
      /// Random generator.
      RandomGenerator& gen_;
      /// Random weights generator.
      random_weight<weightset_t, RandomGenerator> random_weight_{gen_, ws_};
      discrete_chooser<RandomGenerator> chooser_it_{gen_};
    };

    /// Convenience constructor.
    template <typename ExpressionSet, typename RandomGenerator = std::mt19937>
    random_expression_impl<ExpressionSet, RandomGenerator>
    make_random_expression_impl(const ExpressionSet& rs,
                                const std::string& param,
                                RandomGenerator& gen = make_random_engine())
    {
      return {rs, param, gen};
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
  random_expression_string(const ExpressionSet& rs, const std::string& param)
  {
    auto random_exp = detail::make_random_expression_impl(rs, param);
    return random_exp.random_expression_string();
  }


  /// Generate a random expression.
  template <typename ExpressionSet>
  typename ExpressionSet::value_t
  random_expression(const ExpressionSet& rs, const std::string& param)
  {
    auto random_exp = detail::make_random_expression_impl(rs, param);
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
        auto rs = make_expressionset(c, ids);
        return {rs, random_expression(rs, param)};
      }
    }
  }
} // end namespace vcsn
