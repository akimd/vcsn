#include <vcsn/dyn/translate.hh>

#include <dlfcn.h>
#include <fstream>
#include <memory>
#include <set>
#include <sstream>
#include <string>

#include <boost/filesystem.hpp>

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
    namespace detail
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

        void context()
        {
          header("vcsn/ctx/context.hh");
          os << "vcsn::ctx::context<" << incendl;
          // LabelSet, WeightSet.
          valueset();
          eat(is, '_');
          os << ',' << iendl;
          valueset();
          os << decendl << '>';
        }

        /// Read a labelset in \a is.
        void labelset()
        {
          labelset(word());
        }

        /// Read in \a is a labelset of \a kind.
        void labelset(const std::string& kind)
        {
          if (kind == "lal" || kind == "law")
            {
              if (kind == "lal")
                {
                  header("vcsn/labelset/letterset.hh");
                  // Some instantiation happen here:
                  header("vcsn/ctx/lal_char.hh");
                  os << "vcsn::letterset";
                }
              else if (kind == "law")
                {
                  // Some instantiation happen here:
                  header("vcsn/ctx/law_char.hh");
                  os << "vcsn::wordset";
                }
              eat(is, "_char");
              os << "<vcsn::set_alphabet<vcsn::char_letters>>";
            }
          else if (kind == "lan")
            {
              // Some instantiation happen here:
              header("vcsn/ctx/lan_char.hh");
              os << "vcsn::nullableset<" << incendl;
              eat(is, '<');
              valueset();
              eat(is, '>');
              os << decendl << '>';
            }
          else if (kind == "lao")
            {
              header("vcsn/labelset/oneset.hh");
              os << "vcsn::oneset";
            }
          else if (kind == "lat" || kind == "tupleset")
            {
              // It is very important to load this guy after having
              // loaded the headers that define the one it aggregates.
              // See the comments about the 'headers' member.
              headers_late.insert("vcsn/labelset/tupleset.hh");
              os << "vcsn::tupleset<" << incendl;
              eat(is, '<');
              while (true)
                {
                  valueset();
                  int c = is.peek();
                  if (c == ',')
                    {
                      is.get();
                      os << ',' << iendl;
                    }
                  else
                    break;
                }
              eat(is, '>');
              os << decendl << '>';
            }
          else if (kind == "ratexpset")
            ratexpset(false);
          else
            raise("invalid labelset name: ", str_escape(kind));
        }

        void ratexpset(bool with_ratexpset = true)
        {
          // ratexpset<.
          if (with_ratexpset)
            eat(is, "ratexpset");
          eat(is, '<');
          os << "vcsn::ratexpset<" << incendl;
          // LabelSet, WeightSet.
          context();
          eat(is, '>');
          os << decendl << '>';
          header("vcsn/core/rat/ratexpset.hh");
        }

        /// Read a weightset in \a is.
        void weightset()
        {
          weightset(word());
        }

        /// Parse a weightset of type \a ws.
        void weightset(const std::string& ws)
        {
          if (ws == "b" || ws == "f2"  || ws == "q"
              || ws == "r" || ws == "z" || ws == "zmin")
            {
              header("vcsn/weights/" + ws + ".hh");
              os << "vcsn::" << ws;
            }
          else if (ws == "ratexpset")
            ratexpset(false);
          else
            raise("invalid weightset name: ", str_escape(ws));
        }

        /// Read in \a is a valueset (labelset or weightset).
        void valueset()
        {
          valueset(word());
        }

        void valueset(const std::string& kind)
        {
          if (kind == "lal"
              || kind == "lan"
              || kind == "lao"
              || kind == "lat"
              || kind == "law")
            labelset(kind);
          else
            weightset(kind);
        }

        /// Read in \a is a valueset (labelset or weightset).
        void automaton()
        {
          automaton(word());
        }

        void automaton(const std::string& w)
        {
          if (w == "mutable")
            {
              eat(is, "_automaton<");
              os << "vcsn::mutable_automaton<" << incendl;
              context();
              eat(is, '>');
              os << decendl << '>';
              header("vcsn/core/mutable_automaton.hh");
            }
          else if (w == "transpose")
            {
              eat(is, "_automaton<");
              os << "vcsn::detail::transpose_automaton<" << incendl;
              automaton();
              eat(is, '>');
              os << decendl << '>';
              header("vcsn/algos/transpose.hh");
            }
          else
            raise("invalid automaton name: ", str_escape(w));
        }

        /// Read a thing.
        void type()
        {
          type(word());
        }

        void type(const std::string& w)
        {
          if (w == "const std::string"
              || w == "int"
              || w == "std::ostream")
            os << w;
          else if (w == "mutable" || w == "transpose")
            automaton(w);
          else
            valueset(w);
        }

        /// Generate the code to compile on \a o.
        std::ostream& print(std::ostream& o)
        {
          o << "// " << is.str() << "\n";
          o <<
            "#define BUILD_LIBVCSN 1\n"
            "#define VCSN_INSTANTIATION 1\n"
            "#define MAYBE_EXTERN\n"
            "\n";
          for (const auto& h: headers)
            o << "#include <" << h << ">\n";
          o << '\n';
          for (const auto& h: headers_late)
            o << "#include <" << h << ">\n";
          o << "\n"
            << os.str();
          return o;
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
                             xgetenv("VCSN_HOME", "~/.vcsn/plugins"));
          res = expand_tilda(res);
          ensure_directory(res);
          return res + "/";
        }

        /// Compile and load a C++ file.
        void jit(const std::string& base)
        {
          auto cppflags = xgetenv("VCSN_CPPFLAGS", VCSN_CPPFLAGS);
          cxx("-fPIC " + cppflags + " '" + base + ".cc' -c"
              " -o '" + base + ".o'");
          auto ldflags = xgetenv("VCSN_LDFLAGS", VCSN_LDFLAGS);
          cxx("-fPIC " + ldflags + " -lvcsn '" + base + ".o' -shared"
              " -o '" + base + ".so'");
          void* lib = dlopen((base + ".so").c_str(), RTLD_LAZY);
          require(lib, "cannot load lib: ", base, ".so");
          // Upon success, remove the .o file, it useless and large
          // (10x compared to the *.so on erebus using clang).  Keep
          // the .cc file for inspection.
          boost::filesystem::remove(base + ".o");
        }

        /// Compile, and load, a DSO with instantiations for \a ctx.
        void compile(const std::string& ctx)
        {
          header("vcsn/ctx/instantiate.hh");
          std::string base = plugindir() + context_base::sname(ctx);
          is.clear();
          is.str(ctx);
          os << "using ctx_t =" << incendl;
          context();
          if (is.peek() != -1)
            vcsn::fail_reading(is, "unexpected trailing characters");
          os << ';' << decendl
             <<
            "\n"
            "namespace vcsn\n"
            "{\n"
            "  VCSN_CTX_INSTANTIATE(ctx_t);\n"
            "}\n";
          ;
          {
            std::ofstream o{base + ".cc"};
            print(o);
          }
          jit(base);
        }

        /// Compile, and load, a DSO which instantiates \a algo for \a sig.
        void compile(const std::string& name, const signature& sig)
        {
          header("vcsn/algos/" + name + ".hh");
          std::string base = plugindir() + name + "(" + sig.to_string() + ")";
          int count = 0;
          std::string types;
          for (const auto& s: sig)
            {
              is.clear();
              is.str(s);
              std::string t = "t" + std::to_string(count) + "_t";
              os << "using " << t << " =" << incendl;
              type();
              if (is.peek() != -1)
                vcsn::fail_reading(is, "unexpected trailing characters");
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
          {
            std::ofstream o{base + ".cc"};
            print(o);
          }
          jit(base);
        }

        /// Record that we need an include for this header.
        void header(std::string h)
        {
          // Open code some mismatches between algo name, and header
          // name.  FIXME: algorithms should register this themselves.
          if (false) {}
#define ALGO(In, Out)                           \
          else if (h == "vcsn/algos/" In ".hh") \
            h = "vcsn/algos/" Out ".hh"
          ALGO("chain_ratexp", "concatenation");
          ALGO("concatenation_ratexp", "concatenation");
          ALGO("copy_ratexp", "copy");
          ALGO("infiltration", "product");
          ALGO("info_ratexp", "info");
          ALGO("intersection_ratexp", "product");
          ALGO("is_valid_ratexp", "is_valid");
          ALGO("left_mult_ratexp", "left_mult");
          ALGO("list_ratexp", "print");
          ALGO("make_context", "make-context");
          ALGO("print_ratexp", "print");
          ALGO("print_weight", "print");
          ALGO("right_mult", "left_mult");
          ALGO("right_mult_ratexp", "left_mult");
          ALGO("shuffle", "product");
          ALGO("standard_ratexp", "standard");
          ALGO("sum_ratexp", "sum");
          ALGO("transpose_ratexp", "transpose");
          ALGO("union_a", "union");
          ALGO("union_ratexp", "union");
#undef ALGO
          headers.insert(h);
        }

        /// The input stream: the specification to translate.
        std::istringstream is;
        /// The output stream: the corresponding C++ snippet to compile.
        std::ostringstream os;
        /// Headers to include.
        ///
        /// Sadly enough functions about tupleset must be defined
        /// after the functions that define the behavior of the
        /// components.  The genuine case is that of "print_set",
        /// which fails for the same reasons as the following does not
        /// compile:
        ///
        /// template <typename T>
        /// struct wrapper
        /// {
        ///   T t;
        /// };
        ///
        /// template <typename T>
        /// void print(const wrapper<T>& w)
        /// {
        ///   print(w.t);
        /// }
        ///
        /// void print(int){}
        ///
        /// int main()
        /// {
        ///   wrapper<int> w;
        ///   print(w);
        /// }
        ///
        /// So we use a second set for "late" headers.
        std::set<std::string> headers;
        std::set<std::string> headers_late;
      };
    } // namespace detail

    /// Instantiate a given context.
    void compile(const std::string& ctx)
    {
      detail::translation translate;
      translate.compile(ctx);
    }

    /// Instantiate an algorithm for a given signature.
    void compile(const std::string& algo, const signature& sig)
    {
      detail::translation translate;
      translate.compile(algo, sig);
    }

  } // namespace dyn
} // namespace vcsn
