#include <vcsn/dyn/translate.hh>

#include <fstream>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <unistd.h> // getpid

#include <boost/filesystem.hpp>

#include <ltdl.h>

#include <vcsn/dyn/context-parser.hh>
#include <vcsn/dyn/context-printer.hh>

#include <vcsn/config.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/misc/escape.hh>
#include <vcsn/misc/indent.hh>
#include <vcsn/misc/raise.hh>
#include <vcsn/misc/regex.hh>
#include <vcsn/misc/signature.hh>
#include <vcsn/misc/stream.hh>

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
      /// \a getenv(var) if defined, otherwise \a val.
      std::string
      xgetenv(const std::string& var, const std::string& val = "")
      {
        const char* cp = getenv(var.c_str());
        return cp ? cp : val;
      }

#define XGETENV(Name) xgetenv(#Name, Name)

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
        /// Return the next word from \a is.
        /// Stop at any of "<,_>", and leave this separator in the stream.
        std::string word()
        {
          std::string res;
          int c;
          while ((c = is.get()) != EOF)
            if (c == '<' || c == ',' || c == '_' || c == '>')
              {
                is.unget();
                break;
              }
            else
              res += c;
          return res;
        }

        translation()
          : parser_(is), printer_(os)
        {}

        /// Generate the code to compile in file \a base ".cc".
        ///
        /// Do that atomically.  We could perform the whole generation
        /// step using BASE.PID.cc -> BASE.PID.o -> BASE.PID.so ->
        /// BASE.so.  Rather, we use BASE.PID.cc -> BASE.cc ->
        /// BASE.PID.o, other ccache will always fail to find
        /// "BASE.PID.cc" in its cache.
        void print(const std::string& base)
        {
          ensure_parent_directory(base);
          // For atomicity, generate a file with PID, then mv it
          // (which is atomic on any decent OS/FS).
          auto tmp = tmpname(base);
          {
            std::ofstream o{tmp + ".cc"};
            printer_.print(o);
          }
          boost::filesystem::rename(tmp + ".cc", base + ".cc");
        }

        /// Generate C++ syntax for context \a ctx (which might use
        /// our syntax: 'lal_char(ab), z').
        void print_context(const std::string& ctx)
        {
          is.clear();
          is.str(ctx);
          auto ast = parser_.parse_context();
          ast->accept(printer_);
        }

        /// Generate C++ syntax for type \a type.
        void print_type(const std::string& type)
        {
          is.clear();
          is.str(type);
          auto ast = parser_.parse();
          ast->accept(printer_);
        }

        /// Run C++ compiler.
        ///
        /// \param cmd  the compiler arguments
        /// \param tmp  the base name for temporary files
        void cxx(std::string cmd, const std::string& tmp)
        {
          auto err = tmp + ".err";
          // We try to read the error message via a regexp below.  So
          // avoid translation (we once had "erreur" instead of "error").
          cmd = "LC_ALL=C " + cmd;

          if (getenv("VCSN_DEBUG"))
            std::cerr << "run: " << cmd << std::endl;
          std::string assertions;
          if (system((cmd + " 2>'" + err + "'").c_str()))
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
              auto is = open_input_file(err);
              static std::regex r1("error: static assertion failed: (.*)$",
                                   std::regex::extended);
              static std::regex r2("error: static_assert failed \"(.*)\"$",
                                   std::regex::extended);
              std::string line;
              std::smatch smatch;
              while (std::getline(*is, line))
                if (std::regex_search(line, smatch, r1)
                    || std::regex_search(line, smatch, r2))
                  assertions += std::string(smatch[1]) + '\n';
              if (getenv("VCSN_VERBOSE"))
                {
                  cmd += "\n  compiler error messages:\n";
                  cmd += get_file_contents(err);
                }
              throw jit_error(assertions, "  failed command:\n    " + cmd);
            }
          else if (!getenv("VCSN_DEBUG"))
            boost::filesystem::remove(err);
        }

        /// Run C++ compiler to compile.
        ///
        /// \param base  the file base name
        void cxx_compile(const std::string& base)
        {
          auto tmp = tmpname(base);
          auto cmd = (XGETENV(VCSN_CCACHE)
                      + " " + XGETENV(VCSN_CXX)
                      + " " + XGETENV(VCSN_CXXFLAGS)
                      + " " + XGETENV(VCSN_CPPFLAGS)
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
          auto cmd = (XGETENV(VCSN_CXX)
                      + " " + XGETENV(VCSN_CXXFLAGS)
                      + " " + XGETENV(VCSN_LDFLAGS)
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
          std::string res;
          const size_t size = 200;
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
          cxx_compile(base);
          cxx_link(base);
          auto tmp = tmpname(base);
          boost::filesystem::rename(tmp + ".so", base + ".so");
          static bool first = true;
          if (first)
            {
              lt_dlinit();
              first = false;
            }
          lt_dlhandle lib = lt_dlopen((base + ".so").c_str());
          require(lib, "cannot load lib: ", base, ".so: ", lt_dlerror());
          // Upon success, remove the .o file, it is large (10x
          // compared to the *.so on erebus using clang) and not
          // required.  However the debug symbols are in there, so
          // when debugging, leave them!
          if (!getenv("VCSN_DEBUG"))
            {
              boost::filesystem::remove(tmp + ".cc");
              boost::filesystem::remove(tmp + ".o");
            }
        }

        /// Compile, and load, a DSO with instantiations for \a ctx.
        void compile(const std::string& ctx)
        {
          printer_.header("vcsn/ctx/instantiate.hh");
          std::string base =
            plugindir() + "contexts/" + split(ctx);
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
        void compile(const std::set<std::pair<std::string, signature>>& algos)
        {
          printer_.header("vcsn/misc/name.hh"); // ssignature
          printer_.header("vcsn/dyn/registers.hh");
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
                "static bool " << algo.first << " =" << incendl
                 << "vcsn::dyn::detail::" << algo.first << "_register(" << incendl
                 << "vcsn::ssignature<" << types << ">()," << iendl
                 << "vcsn::dyn::detail::" << algo.first << "<" << types << ">" << decendl
                 << ");" << decendl;
            }

          // The first algo is the once that gives its name to the
          // file to compile.
          std::string base = (plugindir()
                              + "algos/"
                              + begin(algos)->first + "/"
                              + split(begin(algos)->second.to_string()));
          print(base);
          jit(base);
        }

        /// The input stream: the specification to translate.
        std::istringstream is;
        /// The output stream: the corresponding C++ snippet to compile.
        std::ostringstream os;
        ast::context_parser parser_;
        ast::context_printer printer_;
      };
    } // namespace detail

    void compile(const std::string& ctx)
    {
      translation translate;
      translate.compile(ctx);
    }

    void compile(const std::string& algo, const signature& sig)
    {
      translation translate;
      std::set<std::pair<std::string, signature>> algos{{algo, sig}};
      if (algo == "dot"
          // || algo == "efsm" Need to add a bool argument.
          || algo == "info" && sig.size() == 3)
        {
          // Aut, Ostrean, Bool.
          algos.emplace("dot", sig);
          algos.emplace("info", sig);
          // Aut, Ostream.
          signature sig2{sig[0], sig[1]};
          algos.emplace("efsm", sig2);
        }
      else if (algo == "synchronize" || algo == "is_synchronized")
        {
          algos.emplace("synchronize", sig);
          algos.emplace("is_synchronized", sig);
        }
      translate.compile(algos);
    }
  } // namespace dyn
} // namespace vcsn
