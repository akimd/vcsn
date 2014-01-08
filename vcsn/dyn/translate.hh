#ifndef VCSN_DYN_TRANSLATE_HH
# define VCSN_DYN_TRANSLATE_HH

# include <memory>
# include <set>
# include <sstream>
# include <string>

#include <dlfcn.h>
#include <fstream>

# include <vcsn/misc/escape.hh>
# include <vcsn/misc/indent.hh>
# include <vcsn/misc/raise.hh>
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
        translation()
        {
          // We use os.str() with one level of indentation.
          // Alternatively, it would be useful that misc::indent
          // provided a means to output a std::string and intercept
          // its \n to indent them properly.
          os << incindent;
        }

        /// Return the next word from \a is.
        /// Stop at any of "<,_>".
        std::string word()
        {
          std::string res;
          char c;
          while (is >> c)
            if (c == '<' || c == ',' || c == '_' || c == '>')
              {
                is.unget();
                break;
              }
            else
              res.append(1, c);
          return res;
        }

        void context()
        {
          headers.insert("vcsn/ctx/context.hh");
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

        void labelset(const std::string& kind)
        {
          if (kind == "lal" || kind == "lan" || kind == "law")
            {
              if (kind == "lal")
                {
                  headers.insert("vcsn/labelset/letterset.hh");
                  // Some instantiation happen here:
                  headers.insert("vcsn/ctx/lal_char.hh");
                  os << "vcsn::ctx::letterset";
                }
              else if (kind == "lan")
                {
                  headers.insert("vcsn/labelset/nullableset.hh");
                  os << "vcsn::ctx::nullableset";
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
              os << "<vcsn::set_alphabet<vcsn::char_letters>>";
            }
          else if (kind == "lao")
            {
              headers.insert("vcsn/labelset/oneset.hh");
              os << "vcsn::ctx::oneset";
            }
          else if (kind == "lat")
            {
              eat(is, '<');
              headers.insert("vcsn/labelset/tupleset.hh");
              os << "vcsn::ctx::tupleset<";
              while (true)
                {
                  valueset();
                  int c = is.peek();
                  if (c == ',')
                    {
                      is.get();
                      os << ",\n";
                    }
                  else
                    break;
                }
              eat(is, '>');
              os << ">\n";
            }
          else
            raise("invalid kind name: ", str_escape(kind));
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
          headers.insert("vcsn/core/rat/ratexpset.hh");
        }

        /// Read a weightset in \a is.
        void weightset()
        {
          weightset(word());
        }

        void weightset(const std::string& ws)
        {
          if (ws == "b" || ws == "f2"  || ws == "q"
              || ws == "r" || ws == "z" || ws == "zmin")
            {
              headers.insert("vcsn/weights/" + ws + ".hh");
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
          auto kind = word();
          if (kind == "lal"
              || kind == "lan"
              || kind == "lao"
              || kind == "lat"
              || kind == "law")
            labelset(kind);
          else
            weightset(kind);
        }

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
          o << "\n";
          o << "using ctx_t =" << incendl;
          o << os.str() << ';' << decendl;
          o <<
            "\n"
            "#include <vcsn/ctx/instantiate.hh>\n"
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
            raise("cannot run: ", s);
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
          require (lib, "cannot load lib: ", base, ".so");
        }

        /// The input stream: the specification to translate.
        std::istringstream is;
        /// The output stream: the corresponding C++ snippet to compile.
        std::ostringstream os;
        /// Headers to include.
        std::set<std::string> headers;
      };
    } // namespace detail
  } // namespace dyn
} // namespace vcsn

#endif // !VCSN_DYN_TRANSLATE_HH
