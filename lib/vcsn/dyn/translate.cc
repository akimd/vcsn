#include <vcsn/dyn/translate.hh>

#include <dlfcn.h>
#include <fstream>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <unistd.h> // getpid

#include <boost/filesystem.hpp>

#include <vcsn/dyn/context-parser.hh>
#include <vcsn/dyn/context-printer.hh>

#include <vcsn/config.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/misc/escape.hh>
#include <vcsn/misc/indent.hh>
#include <vcsn/misc/raise.hh>
#include <vcsn/misc/signature.hh>
#include <vcsn/misc/stream.hh>

namespace vcsn
{
  namespace dyn
  {
    namespace
    {
      /// \a getenv(var) if defined, otherwise \a val.
      std::string
      xgetenv(const std::string& var, const std::string& val = "")
      {
        const char* cp = getenv(var.c_str());
        return cp ? cp : val;
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

      /// Ensure a directory exists.
      void ensure_directory(const std::string& dir)
      {
        boost::filesystem::create_directories(dir);
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
          // For atomicity, generate a file with PID, then mv it
          // (which is atomic on any decent OS/FS).
          auto tmp = tmpname(base);
          {
            std::ofstream o{tmp + ".cc"};
            printer_.print(o);
          }
          boost::filesystem::rename(tmp + ".cc", base + ".cc");
        }

        void print_ctx(const std::string& ctx)
        {
          is.clear();
          is.str(ctx);
          auto ast = parser_.parse();
          ast->accept(printer_);
          if (is.peek() != -1)
            vcsn::fail_reading(is, "unexpected trailing characters");
        }

        /// Run C++ compiler with arguments \a s.
        void cxx(std::string s)
        {
          // Break the compilation/linking in two steps, in case we
          // are using ccache, which does not handle
          // compilation-and-linking in a single run.
          s = xgetenv("VCSN_CXX", VCSN_CXX)
            + " " + xgetenv("VCSN_CXXFLAGS", VCSN_CXXFLAGS)
            + " " + s;
          if (getenv("VCSN_DEBUG"))
            std::cerr << "run: " << s << std::endl;
          if (system(s.c_str()))
            raise("cannot run: ", s);
        }

        /// Where the runtime compilation files must be put.
        std::string plugindir() const
        {
          auto res = xgetenv("VCSN_PLUGINDIR",
                             xgetenv("VCSN_HOME", "~/.vcsn") + "/plugins");
          res = expand_tilda(res);
          ensure_directory(res);
          return res + "/";
        }

        /// Compile and load a C++ file.
        ///
        /// Avoid locks, but avoid races by using temporary files, and
        /// using rename, which is atomic.
        void jit(const std::string& base)
        {
          /// Use a temporary base name for object file.
          std::string tmp = tmpname(base);
          auto cppflags = xgetenv("VCSN_CPPFLAGS", VCSN_CPPFLAGS);
          cxx("-fPIC " + cppflags + " '" + base + ".cc' -c"
              " -o '" + tmp + ".o'");
          auto ldflags = xgetenv("VCSN_LDFLAGS", VCSN_LDFLAGS);
          cxx("-fPIC " + ldflags + " -lvcsn '" + tmp + ".o' -shared"
              " -o '" + tmp + ".so'");
          boost::filesystem::rename(tmp + ".so", base + ".so");
          void* lib = dlopen((base + ".so").c_str(), RTLD_LAZY);
          require(lib, "cannot load lib: ", base, ".so");
          // Upon success, remove the .o file, it useless and large
          // (10x compared to the *.so on erebus using clang).  Keep
          // the .cc file for inspection.
          boost::filesystem::remove(tmp + ".o");
        }

        /// Compile, and load, a DSO with instantiations for \a ctx.
        void compile(const std::string& ctx)
        {
          printer_.header("vcsn/ctx/instantiate.hh");
          std::string base = plugindir() + detail::context_base::sname(ctx);
          os << "using ctx_t =" << incendl;
          print_ctx(ctx);
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

        /// Compile, and load, a DSO which instantiates \a algo for \a sig.
        void compile(const std::string& name, const signature& sig)
        {
          printer_.header("vcsn/misc/name.hh"); // ssignature
          printer_.header("vcsn/algos/" + name + ".hh");
          std::string base = plugindir() + name + "(" + sig.to_string() + ")";
          int count = 0;
          std::string types;
          for (const auto& s: sig)
            {
              std::string t = "t" + std::to_string(count) + "_t";
              os << "using " << t << " =" << incendl;
              print_ctx(s);
              os << ';' << decendl;
              types += (count ? ", " : "") + t;
              ++count;
            }
          os <<
            "\n"
            "namespace vcsn\n"
            "{\n"
            "  namespace dyn\n"
            "  {\n"
            "    namespace detail\n"
            "    {\n"
            "      static bool f = " << name << "_register(ssignature<" << types << ">(), "
             << name << "<" << types << ">);\n"
            "    }\n"
            "  }\n"
            "}\n";
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

    /// Instantiate a given context.
    void compile(const std::string& ctx)
    {
      translation translate;
      translate.compile(ctx);
    }

    /// Instantiate an algorithm for a given signature.
    void compile(const std::string& algo, const signature& sig)
    {
      translation translate;
      translate.compile(algo, sig);
    }

  } // namespace dyn
} // namespace vcsn
