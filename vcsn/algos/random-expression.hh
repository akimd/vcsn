#include <ostream>
#include <regex>
#include <string>

#include <boost/algorithm/string/erase.hpp>
#include <boost/tokenizer.hpp>

#include <vcsn/algos/random.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/misc/algorithm.hh>
#include <vcsn/misc/cast.hh>
#include <vcsn/misc/raise.hh>
#include <vcsn/misc/random.hh>

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
      using operator_t = std::map<std::string, float>;
      using weight_t = std::vector<float>;

      random_expression_impl(const std::string& param, RandomGenerator& gen)
        : gen_{gen}
      {
        parse_param_(param);
      }

      std::ostream& print_random_expression(std::ostream& out,
                                            const ExpressionSet& ls)
      {
        return print_random_expression_(out, ls, num_symbols_);
      }

    private:
      void parse_param_(const std::string& param)
      {
        // Set default value.
        num_symbols_ = 6;
        using tokenizer = boost::tokenizer<boost::char_separator<char>>;
        auto sep = boost::char_separator<char>{","};
        auto tok = tokenizer(param, sep);
        for (auto it = tok.begin(); it != tok.end(); ++it)
        {
          auto tok_arg = std::string{*it};
          auto eq = tok_arg.find_first_of('=');
          if (eq == std::string::npos)
            raise("random_expression: operator '", tok_arg, "' need a value");
          auto op = tok_arg.substr(0, eq);
          auto value = tok_arg.substr(eq + 1);
          // trim whitespaces.
          boost::algorithm::erase_all(op, " ");
          boost::algorithm::erase_all(value, " ");
          if (op == "!" || op == "{c}" || op == "*" || op == "{T}")
            unary_op_[op] = lexical_cast<float>(value);
          else if (op == "symbols")
            num_symbols_ = lexical_cast<float>(value);
          else if (op == "\\e" || op == "\\z")
            nullary_op_[op] = lexical_cast<float>(value);
          else if (op == "&" || op == "&:" || op == ":"
                   || op == "." || op == "<+" || op == "%"
                   || op == "+" || op == "{/}" || op == "{\\}")
            binary_op_[op] = lexical_cast<float>(value);
          else
            raise("random_expression: invalid operator: ", op);
        }

        weight_null_ = transform(nullary_op_, [](const auto& v){ return v.second; });
        weight_un_   = transform(unary_op_,   [](const auto& v){ return v.second; });
        weight_bin_  = transform(binary_op_,  [](const auto& v){ return v.second; });
      }

      /// Print nullary expression (\z or \e).
      /// If there is only one expression available, then apply Bernoulli
      /// distribution to choose if a nullary expresssion should be printed
      /// or not (if not then continue but decrement the number of symbols).
      void print_nullary_exp_(std::ostream& out,
                              const ExpressionSet& ls,
                              unsigned num_symbols)
      {
        // FIXME: the proportion of \e should be controllable (see random_label).
        if (nullary_op_.size() == 1
            && !std::bernoulli_distribution(nullary_op_.begin()->second)(gen_))
          print_random_expression_(out, ls, num_symbols - 1);
        else
        {
          auto it =
            discrete_chooser<RandomGenerator>{gen_}(weight_null_.begin(),
                                                    weight_null_.end(),
                                                    nullary_op_.begin());
          out << it->first;
        }
      }

      /// Print expression with unary operator
      /// If there is no unary operator available and there is only 2 symbols
      /// left, then just print a random label.
      /// If there is only one operator available, then apply bernoulli
      /// distribution to choose if a unary operator should be print
      /// or not (if not then continue but decrement the number of symbols).
      void print_unary_exp_(std::ostream& out,
                            const ExpressionSet& ls,
                            unsigned num_symbols)
      {
        if (unary_op_.empty())
          ls.print(random_label(ls, gen_), out);
        else if (unary_op_.size() == 1
                 && !(std::bernoulli_distribution(unary_op_.begin()->second)(gen_)))
          print_random_expression_(out, ls, num_symbols - 1);
        else
        {
          auto it =
            discrete_chooser<RandomGenerator>{gen_}(weight_un_.begin(),
                                                    weight_un_.end(),
                                                    unary_op_.begin());
          auto op = it->first;
          // prefix
          if (op == "!")
          {
            out << "(" << op;
            print_random_expression_(out, ls, num_symbols - 1);
            out << ")";
          }
          // postfix
          else
          {
            out << "(";
            print_random_expression_(out, ls, num_symbols - 1);
            out << op << ")";
          }
        }
      }

      /// Print binary expression with binary operator.
      /// It is composed of the left and right side, and the operator.
      /// The number of symbols is randomly distribued between both side.
      void print_binary_exp_(std::ostream& out,
                             const ExpressionSet& ls,
                             unsigned num_symbols)
      {
        auto it =
          discrete_chooser<RandomGenerator>{gen_}(weight_bin_.begin(),
                                                  weight_bin_.end(),
                                                  binary_op_.begin());
        auto dis = std::uniform_int_distribution<>(1, num_symbols - 1);
        auto num_lhs = dis(gen_);
        out << "(";
        print_random_expression_(out, ls, num_lhs);
        out << it->first;
        print_random_expression_(out, ls, num_symbols - num_lhs);
        out << ")";
      }

      std::ostream& print_random_expression_(std::ostream& out,
                                             const ExpressionSet& ls,
                                             unsigned num_symbols)
      {
        // If there is no operators at all, that's impossible to
        // construct an expression, so just return a label.
        if (binary_op_.empty() && unary_op_.empty())
        {
          ls.print(random_label(ls, gen_), out);
          return out;
        }

        switch (num_symbols)
        {
          // 2 symbols left: take unary operator.
        case 2:
          print_unary_exp_(out, ls, num_symbols);
          break;

          // 1 symbol left: print a label.
        case 1:
          ls.print(random_label(ls, gen_), out);
          break;

          // binary, unary or nullary operators are possible
          // just choose randomly one (between those that are not empty)
          // and print the associated expression.
        default:
        {
          auto choose =
            std::uniform_int_distribution<>(0, unary_op_.empty() ?  1 : 2)(gen_);
          if (unary_op_.empty() && nullary_op_.empty())
            choose = 1;
          else if (binary_op_.empty() && choose == 1)
            choose = 2;
          else if (nullary_op_.empty())
            choose = std::uniform_int_distribution<>(0, 1)(gen_) + 1;

          switch (choose)
          {
          case 0:
            print_nullary_exp_(out, ls, num_symbols);
            break;

          case 1:
            print_binary_exp_(out, ls, num_symbols);
            break;

          default:
            print_unary_exp_(out, ls, num_symbols);
            break;
          }
        }
        break;
        }
        return out;
      }

      unsigned num_symbols_;
      operator_t nullary_op_;
      operator_t unary_op_;
      operator_t binary_op_;
      weight_t weight_null_;
      weight_t weight_un_;
      weight_t weight_bin_;
      RandomGenerator gen_;
    };
  } // end namespace vcsn::detail

  /// Generate a random expression from a context. This returns a string as
  /// it is easier to generate that way without creating an AST in place, and
  /// the cost of creating the expression from the string representation is
  /// negligible. This also allows doing easily constructs that are not
  /// possible in the AST in case we want them (i.e [a-z] ranges).
  /// Furthermore, the user can then choose the identities they want to apply
  /// to the resulting expression.
  template <typename ExpressionSet>
  std::string random_expression(const ExpressionSet& ls, const std::string& param)
  {
    using namespace detail;
    auto& gen = make_random_engine();

    std::ostringstream out;
    auto random_exp = random_expression_impl<ExpressionSet, decltype(gen)>(param, gen);
    random_exp.print_random_expression(out, ls);
    return out.str();
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (random_expression).
      template <typename Context, typename String, typename Identities>
      expression
      random_expression(const context& ctx, const std::string& param,
                        rat::identities ids)
      {
        const auto& c = ctx->as<Context>();
        std::istringstream in(random_expression(*c.labelset(), param));
        return read_expression(ctx, ids, in);
      }
    }
  }
} // end namespace vcsn
