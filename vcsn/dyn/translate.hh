#ifndef VCSN_DYN_TRANSLATE_HH
# define VCSN_DYN_TRANSLATE_HH

# include <memory>
# include <set>
# include <sstream>
# include <string>

#include <dlfcn.h>
#include <fstream>

# include <vcsn/misc/escape.hh>
# include <vcsn/misc/escape.hh>
# include <vcsn/misc/stream.hh>
# include <vcsn/config.hh>
# include <vcsn/dyn/context.hh>

namespace vcsn
{
  namespace dyn
  {
    namespace detail
    {
      struct translation
      {
        void context()
        {
          headers.insert("vcsn/ctx/context.hh");
          os << "vcsn::ctx::context<\n  ";
          // LabelSet, WeightSet.
          labelset();
          eat(is, '_');
          os << ",\n  ";
          weightset();
          os << "\n>";
        }

        void labelset()
        {
          std::string kind;
          {
            char k[4];
            is.get(k, sizeof k);
            kind = k;
          }
          if (kind == "lal" || kind == "law")
            {
              if (kind == "lal")
                {
                  headers.insert("vcsn/labelset/letterset.hh");
                  os << "vcsn::ctx::letterset";
                }
              else if (kind == "law")
                {
                  headers.insert("vcsn/labelset/wordset.hh");
                  os << "vcsn::ctx::wordset";
                }
              eat(is, "_char");
              if (is.peek() == '(')
                {
                  // Skip until we have the closing paren.
                  int c;
                  while ((c = is.get()) != EOF && c != ')')
                    continue;
                }
              headers.insert("vcsn/ctx/lal_char.hh");
              os << "<vcsn::set_alphabet<vcsn::char_letters>>";
            }
          else if (kind == "lao")
            {
              headers.insert("vcsn/labelset/oneset.hh");
              os << "vcsn::ctx::oneset";
            }
          else
            throw std::domain_error("invalid kind name: "
                                    + str_escape(kind));
        }

        void ratexpset(bool with_ratexpset = true)
        {
          // ratexpset<.
          if (with_ratexpset)
            eat(is, "ratexpset");
          eat(is, '<');
          os << "vcsn::ratexpset<\n  ";
          // LabelSet, WeightSet.
          context();
          eat(is, '>');
          os << "\n>";
          headers.insert("vcsn/core/rat/ratexpset.hh");
        }

        void weightset()
        {
          std::string ws;
          char c;
          while (is >> c)
            if (c == '<' || c == ',' || c == '>')
              {
                is.unget();
                break;
              }
            else
              ws.append(1, c);
          if (ws == "b" || ws == "f2"  || ws == "q"
              || ws == "r" || ws == "z" || ws == "zmin")
            {
              headers.insert("vcsn/weights/" + ws + ".hh");
              os << "vcsn::" << ws;
            }
          else if (ws == "ratexpset")
            ratexpset(false);
          else
            throw std::domain_error("invalid weightset name: "
                                    + str_escape(ws));
        }

        std::ostream& print(std::ostream& o)
        {
          o << "// " << is.str() << "\n";
          o << "# define MAYBE_EXTERN\n"
            "#define BUILD_LIBVCSN 1\n"
            "#define VCSN_INSTANTIATION 1\n"
            "#define MAYBE_EXTERN\n"
            "\n";
          for (const auto& h: headers)
            o << "#include <" << h << ">\n";
          o << "\n";
          o << "using ctx_t =\n";
          o << os.str() << ";\n";
          o <<
            "# include <vcsn/ctx/instantiate.hh>\n"
            "\n"
            "namespace vcsn\n"
            "{\n"
            "  VCSN_CTX_INSTANTIATE(ctx_t);\n"
            "};\n"
            "\n";
          return o;
        }

        std::string xgetenv(const char* var, const char* val)
        {
          const char* cp = getenv(var);
          return cp ? cp : val;
        }

        void
        cxx(std::string s)
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
            throw std::runtime_error("cannot run: " + s);
        }

        void compile(const std::string& ctx)
        {
          headers.insert("vcsn/ctx/instantiate.hh");
          std::string base = "/tmp/" + context_base::sname(ctx);
          is.str(ctx);
          {
            std::ofstream o{base + ".cc"};
            context();
            print(o);
          }
          auto cppflags = xgetenv("VCSN_CPPFLAGS", VCSN_CPPFLAGS);
          cxx("-fPIC " + cppflags + " '" + base + ".cc' -c"
              " -o '" + base + ".o'");
          auto ldflags = xgetenv("VCSN_LDFLAGS", VCSN_LDFLAGS);
          cxx("-fPIC " + ldflags + " -lvcsn '" + base + ".o' -shared"
              " -o '" + base + ".so'");
          void* lib = dlopen((base + ".so").c_str(), RTLD_LAZY);
          if (!lib)
            throw std::domain_error("cannot load lib: " + base + ".so");
        }

        std::istringstream is;
        std::ostringstream os;
        std::set<std::string> headers;
      };
    } // namespace detail
  } // namespace dyn
} // namespace vcsn

#endif // !VCSN_DYN_TRANSLATE_HH
