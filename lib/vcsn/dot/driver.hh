#pragma once

#include <lib/vcsn/dot/fwd.hh>
#include <vcsn/algos/edit-automaton.hh>
#include <vcsn/core/fwd.hh>
#include <vcsn/core/rat/fwd.hh>
#include <vcsn/misc/export.hh>
#include <vcsn/misc/location.hh>

namespace vcsn::detail::dot
{
  using location_t = vcsn::rat::location;

  /// State and public interface for Dot parsing.
  class LIBVCSN_API driver
  {
  public:
    driver();
    ~driver();

    /// Parse this stream.
    /// \throws runtime_error
    ///    if there are problem, including syntax errors.
    dyn::automaton parse(std::istream& is,
                         const location_t& l = location_t{});

    /// Report an error \a m at \a l.
    void error(const location_t& l, const std::string& m);
    /// The string \a s is invalid at \a l.
    ATTRIBUTE_NORETURN
      void invalid(const location_t& l, const std::string& s);

    /// The error messages.
    std::string errors;
    /// The scanner.
    std::unique_ptr<yyFlexLexer> scanner_;

  private:
    /// From context_, build edit_.
    /// \throw std::exception on invalid contexts.
    void setup_(const location_t& l, const std::string& ctx);

    /// Require that vcsn_context was defined.  But do it only
    /// once, no need to spam the user.
    bool has_edit_(const location_t& l);
    bool require_context_done_ = false;

    /// The inital location.
    location_t location_;
    /// An automaton editor that stores the one being built.
    std::shared_ptr<vcsn::automaton_editor> edit_;
    friend class parser;
  };
}
