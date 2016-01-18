#include <ostream>

#include <vcsn/algos/random.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/misc/random.hh>

namespace vcsn
{
  namespace detail
  {
    template <typename LabelSet,
              typename RandomGenerator = std::default_random_engine>
    std::ostream& print_random_expr(std::ostream& out,
                                    const LabelSet& ls,
                                    unsigned num_terminals,
                                    float star_chance = 0.20,
                                    RandomGenerator gen = RandomGenerator(),
                                    bool previous_star = false)
    {
      std::bernoulli_distribution bern(star_chance);
      if (!previous_star && bern(gen)) // Kleene closure
        {
          out << '(';
          print_random_expr(out, ls, num_terminals, star_chance, gen, true);
          out << ")*";
        }
      else if (num_terminals == 1) // Terminal symbol
          out << random_label(ls, gen);
      else
        {
          std::bernoulli_distribution bern(0.5);
          bool add = bern(gen); // Whether it is an union or a concatenation

          std::uniform_int_distribution<> dis(1, num_terminals - 1);
          auto num_lhs = dis(gen);

          if (add) // Union requires parenthesis for precedence
            out << '(';
          print_random_expr(out, ls, num_lhs, star_chance, gen);
          if (add)
            out << '+';
          print_random_expr(out, ls, num_terminals - num_lhs, star_chance, gen);
          if (add)
            out << ')';
        }

      return out;
    }


  } // end namespace vcsn::detail

  /// Generate a random expression from a context. This returns a string as
  /// it is easier to generate that way without creating an AST in place, and
  /// the cost of creating the expression from the string representation is
  /// negligible. This also allows doing easily constructs that are not
  /// possible in the AST in case we want them (i.e [a-z] ranges).
  /// Furthermore, the user can then choose the identities they want to apply
  /// to the resulting expression.
  template <typename LabelSet>
  std::string random_expr(const LabelSet& ls, unsigned num_terminals,
                          float star_chance)
  {
    std::random_device rd;
    auto seed = rd();
    if (getenv("VCSN_SEED"))
      seed = std::mt19937::default_seed;
    std::mt19937 gen(seed);

    std::ostringstream out;
    print_random_expr(out, ls, num_terminals, star_chance, gen);
    return out.str();
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (random_expression).
      template <typename Context, typename Float, typename Integer,
                typename Identities>
      expression
      random_expression(const context& ctx, unsigned num_terminals,
                        float star_chance, rat::identities ids)
      {
        const auto& c = ctx->as<Context>();
        std::istringstream in(random_expr(*c.labelset(), num_terminals,
                                          star_chance));
        return read_expression(ctx, ids, in);
      }
    }
  }
} // end namespace vcsn
