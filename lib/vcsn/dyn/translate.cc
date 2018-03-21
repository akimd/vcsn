#include <lib/vcsn/dyn/translate.hh>

#include <chrono>
#include <fstream>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <unistd.h> // getpid

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/filesystem.hpp>

#include <lib/vcsn/dyn/context-parser.hh>
#include <lib/vcsn/dyn/context-printer.hh>
#include <lib/vcsn/dyn/type-ast.hh>
#include <lib/vcsn/misc/xltdl.hh>

#include <vcsn/dyn/context.hh>
#include <vcsn/misc/configuration.hh>
#include <vcsn/misc/escape.hh>
#include <vcsn/misc/indent.hh>
#include <vcsn/misc/regex.hh>
#include <vcsn/misc/signature.hh>
#include <vcsn/misc/stream.hh>

namespace
{
  /// Level of verbosity.
  int verbose = []{
    auto res = 0;
    auto cp = getenv("VCSN_VERBOSE");
    std::istringstream is{cp ? cp : "0"};
    is >> res;
    return res;
  }();
}

namespace vcsn
{
  namespace dyn
  {

    jit_error::jit_error(const std::string& a, const std::string& what)
      : std::runtime_error(what)
      , assertions(a)
    {}

    namespace
    {
#define XGETENV(Name) xgetenv(#Name, Name)

      /// Get a var from the config, unless an envvar override it.
      /// For instance `configuration.cxx` and `$VCSN_CXX`.
      std::string
      config(const std::string& var)
      {
        auto envvar = "VCSN_" + boost::algorithm::to_upper_copy(var);
        if (auto cp = getenv(envvar.c_str()))
          return cp;
        else
          return get_config()["configuration"][var].str();
      }

      /// Expand initial "~" in res.
      // http://stackoverflow.com/questions/4891006.
      std::string expand_tilda(std::string res)
      {
        if (!res.empty() && res[0] == '~')
          {
            assert(res.size() == 1 || res[1] == '/');
            auto home = xgetenv("HOME", xgetenv("USERPROFILE"));
            char const *hdrive = getenv("HOMEDRIVE");
            char const *hres = getenv("HOMERES");
            if (!home.empty())
              res.replace(0, 1, home);
            else if (hdrive && hres)
              res.replace(0, 1, std::string(hdrive) + hres);
            else
              res.replace(0, 1, xgetenv("VCSN_TMPDIR", "/tmp"));
          }
        return res;
      }

      /// Ensure that parent directories exist.
      void ensure_parent_directory(const std::string& path)
      {
        boost::filesystem::path p(path);
        boost::filesystem::create_directories(p.parent_path());
      }

      /// Append ".PID".
      std::string tmpname(std::string res)
      {
        res += ".";
        res += std::to_string(getpid());
        return res;
      }

      struct translation
      {
        translation()
          : printer_(os)
        {}

        /// Generate the code to compile in file \a base ".cc".
        ///
        /// Do that atomically.  We could perform the whole generation
        /// step using BASE.PID.cc -> BASE.PID.o -> BASE.PID.so ->
        /// BASE.so.  Rather, we use BASE.PID.cc -> BASE.cc ->
        /// BASE.PID.o, other ccache will always fail to find
        /// "BASE.PID.cc" in its cache.
        ///
        /// Do it only if needed: keep the mtime old if the file
        /// contents is the same anyway.
        void print(const std::string& base)
        {
          ensure_parent_directory(base);
          // For atomicity, generate a file with PID, then mv it
          // (which is atomic on any decent OS/FS).
          auto tmp = tmpname(base);
          {
            auto&& o = std::ofstream{tmp + ".cc"};
            VCSN_REQUIRE(o.good(),
                         "cannot create ", tmp+".cc", ": ", strerror(errno));
            printer_.print(o);
          }
          if (equal_files(tmp + ".cc", base + ".cc"))
            boost::filesystem::remove(tmp + ".cc");
          else
            boost::filesystem::rename(tmp + ".cc", base + ".cc");
        }

        /// Generate C++ syntax for context \a ctx (which might use
        /// our syntax: 'lal_char(ab), z').
        void print_context(const std::string& ctx)
        {
          ast::parse_context(ctx)->accept(printer_);
        }

        /// Generate C++ syntax for type \a type.
        void print_type(const std::string& type)
        {
          ast::parse_type(type)->accept(printer_);
        }

        /// Parse compiler errors and throw.
        /// \param cmd  the command that failed.
        /// \param err  the file name of the file containing the compiler log.
        void throw_compiler_errors(std::string cmd,
                                   const std::string& err)
        {
          // Try to find assertion failures in the error log.
          //
          // $ g++-mp-4.9 -std=c++11 main.cc
          // main.cc: In function 'int main()':
          // main.cc:3:3: error: static assertion failed: foo
          //    static_assert(0, "foo");
          //    ^
          // $ clang++-mp-3.5 -std=c++11 main.cc
          // main.cc:3:3: error: static_assert failed "foo"
          //   static_assert(0, "foo");
          //   ^             ~
          // 1 error generated.
          //
          // We don't try to catch the "error:" part, because
          // vcsn-compile loves to add colors, so "error:" is actually
          // cluttered with ANSI escapes for colors.
          auto is = open_input_file(err);
          static auto r1 = std::regex{"static assertion failed: (.*)$",
                                      std::regex::extended};
          static auto r2 = std::regex{"static_assert failed \"(.*)\"$",
                                      std::regex::extended};
          std::string line;
          std::smatch smatch;
          std::string assertions;
          while (std::getline(*is, line))
            if (std::regex_search(line, smatch, r1)
                || std::regex_search(line, smatch, r2))
              assertions += std::string(smatch[1]) + '\n';
          if (verbose)
            {
              cmd += "\n  compiler error messages:\n";
              cmd += get_file_contents(err);
            }
          throw jit_error(assertions, "  failed command:\n    " + cmd);
        }

        /// Run C++ compiler.
        ///
        /// \param cmd  the compiler arguments
        /// \param tmp  the base name for temporary files
        void cxx(std::string cmd, const std::string& tmp)
        {
          auto err = tmp + ".err";
          if (getenv("VCSN_DEBUG"))
            std::cerr << "run: " << cmd << '\n';
          if (system((cmd + " 2>'" + err + "'").c_str()))
            throw_compiler_errors(cmd, err);
          else
            {
              // At least we should see the warnings.
              auto&& log = std::ifstream{err};
              VCSN_REQUIRE(log,
                           "cannot read ", err, ": ", strerror(errno));
              std::cerr << log.rdbuf();
              // If the file is empty the previous instruction sets the state
              // of cerr to bad. We clear the error state flag to be able to
              // read from cerr afterwards.
              std::cerr.clear();
              boost::filesystem::remove(err);
            }
        }

        /// Run C++ compiler to compile.
        ///
        /// \param base  the file base name
        void cxx_compile(const std::string& base)
        {
          auto tmp = tmpname(base);
          // We try to read the error message via a regexp below.  So
          // avoid translation (we once had "erreur" instead of "error").
          auto cmd = (std::string{"LC_ALL=C"}
                      + " " + config("ccache")
                      + " " + config("cxx")
                      + " " + config("cxxflags")
                      + " " + config("cppflags")
                      + " -fPIC '" + base + ".cc' -c"
                      + " -o '" + tmp + ".o'");
          cxx(cmd, tmp);
        }

        /// Run C++ compiler to link.
        ///
        /// \param base  the file base name
        void cxx_link(const std::string& base)
        {
          auto tmp = tmpname(base);
          auto cmd = (std::string{"LC_ALL=C"}
                      + " " + config("cxx")
                      + " " + config("cxxflags")
                      + " " + config("ldflags")
                      + " -fPIC -lvcsn '" + tmp + ".o' -shared"
                      + " -o '" + tmp + ".so'"
                      + printer_.linkflags());
          cxx(cmd, tmp);
        }

        /// Where the runtime compilation files must be put.
        std::string plugindir() const
        {
          auto res = xgetenv("VCSN_PLUGINDIR",
                             xgetenv("VCSN_HOME", "~/.vcsn") + "/plugins");
          res = expand_tilda(res);
          return res + "/";
        }

        /// Split file names that are too long into something with '/'
        /// inserted to avoid file name limits.
        std::string split(const std::string& s) const
        {
          auto res = std::string{};
          const size_t size = 150;
          for (unsigned i = 0; i < s.length(); i += size)
            {
              if (i)
                res += '/';
              res += s.substr(i, size);
            }
          return res;
        }

        /// Compile and load a C++ file.
        ///
        /// Avoid locks, but avoid races by using temporary files, and
        /// using rename, which is atomic.
        ///
        /// Break the compilation/linking in two steps, in case we
        /// are using ccache, which does not handle
        /// compilation-and-linking in a single run.
        void jit(const std::string& base)
        {
          auto tmp = tmpname(base);
          {
            namespace chr = std::chrono;
            using clock = chr::steady_clock;
            auto start = clock::now();
            static bool no_python = !!getenv("VCSN_NO_PYTHON");
            if (no_python)
              {
                cxx_compile(base);
                cxx_link(base);
                boost::filesystem::rename(tmp + ".so", base + ".so");
                // Upon success, remove the .o file, it is large (10x
                // compared to the *.so on erebus using clang) and not
                // required.  However the debug symbols are in there, so
                // when debugging, leave them!
                if (!getenv("VCSN_DEBUG"))
                  boost::filesystem::remove(tmp + ".o");
              }
            else
              {
                auto cmd
                  = xgetenv("VCSN_COMPILE",
                            xgetenv("VCSN", "vcsn") + " compile");
                auto linkflags = printer_.linkflags();
                if (!linkflags.empty())
                  linkflags = " LDFLAGS+='" + linkflags + "'";
                cxx(cmd + " -shared" + linkflags + " '" + base + ".cc'",
                    tmp);
              }
            auto d
              = chr::duration_cast<chr::milliseconds>(clock::now() - start);
            if (getenv("VCSN_TIME"))
              {
                std::ofstream{"/tmp/vcsn-compile.log",
                    std::ofstream::out | std::ofstream::app}
                << d.count() << ", "
                << (no_python ? "C++, " : "Py,  ")
                << '\'' << base.substr(plugindir().size()) << '\''
                << '\n';
                if (getenv("VCSN_TIME2"))
                  std::cerr << d.count() << "ms: " << base << '\n';
              }
          }
          vcsn::detail::xlt_advise()
            .global(true)
            .ext()
            .verbose(1 < verbose)
            .open(base + ".so");
        }

        /// Compile, and load, a DSO with instantiations for \a ctx.
        void operator()(const std::string& ctx)
        {
          printer_.header("vcsn/ctx/instantiate.hh");
          auto base = plugindir() + "contexts/" + split(ctx);
          os << "using ctx_t =" << incendl;
          print_context(ctx);
          os << ';' << decendl
             <<
            "\n"
            "namespace vcsn\n"
            "{\n"
            "  VCSN_CTX_INSTANTIATE(ctx_t);\n"
            "}\n";
          ;
          print(base);
          jit(base);
        }

        /// Compile, and load, a DSO which instantiates \a algos.
        void
        operator()(const std::set<std::pair<std::string, signature>>& algos)
        {
          printer_.header("vcsn/misc/attributes.hh"); // ATTRIBUTE_USED
          printer_.header("vcsn/dyn/name.hh"); // ssignature
          printer_.header("vcsn/dyn/registries.hh");
          for (const auto& algo: algos)
            printer_.header_algo(algo.first);

          unsigned count = 0;
          for (const auto& algo: algos)
            {
              os << iendl
                 << "// " << algo.first << '.';
              std::string types;
              bool first = true;
              for (const auto& s: algo.second)
                {
                  os << iendl;
                  std::string t = "t" + std::to_string(count) + "_t";
                  os << "using " << t << " =" << incendl;
                  print_type(s);
                  os << ';' << decendl;
                  types += (first ? "" : ", ") + t;
                  ++count;
                  first = false;
                }
              os <<
                "\n"
                "static bool vcsn_" << algo.first << " ATTRIBUTE_USED ="
                 << incendl
                 << "vcsn::dyn::detail::" << algo.first << "_register("
                 << incendl
                 << "vcsn::ssignature<" << types << ">(),"
                 << iendl
                 << "vcsn::dyn::detail::" << algo.first << "<" << types << ">"
                 << decendl
                 << ");" << decendl;
            }

          // The first algo is the once that gives its name to the
          // file to compile.
          auto base = (plugindir()
                       + "algos/"
                       + begin(algos)->first + "/"
                       + split(begin(algos)->second.to_string()));
          print(base);
          jit(base);
        }

        /// The output stream: the corresponding C++ snippet to compile.
        std::ostringstream os;
        ast::context_printer printer_;
      };
    } // namespace detail

    void compile(const std::string& ctx)
    {
      try
        {
          auto translate = translation{};
          translate(ctx);
        }
      catch (const std::runtime_error& e)
        {
          raise(e, "  while compiling context ", ctx);
        }
    }

    void compile(const std::string& algo, const signature& sig)
    {
      auto algos = std::set<std::pair<std::string, signature>>{{algo, sig}};
      if (algo == "delay_automaton"
          || algo == "is_synchronized")
        {
          algos.emplace("delay_automaton", sig);
          algos.emplace("is_synchronized", sig);
        }
      try
        {
          auto translate = translation{};
          translate(algos);
        }
      catch (const std::runtime_error& e)
        {
          raise(e, "  while compiling ", algo, " for ", sig);
        }
    }
  } // namespace dyn
} // namespace vcsn
