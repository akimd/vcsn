#ifndef VCSN_DYN_TRANSLATE_HH
# define VCSN_DYN_TRANSLATE_HH

# include <dlfcn.h>
# include <fstream>
# include <memory>
# include <set>
# include <sstream>
# include <string>

# include <vcsn/config.hh>
# include <vcsn/dyn/context.hh>
# include <vcsn/misc/escape.hh>
# include <vcsn/misc/indent.hh>
# include <vcsn/misc/raise.hh>
# include <vcsn/misc/signature.hh>
# include <vcsn/misc/stream.hh>

namespace vcsn
{
  namespace dyn
  {
    namespace detail
    {
      struct translation
      {
        /// Return the next word from \a is.
        /// Stop at any of "<,_>", and leave this separator in the stream.
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
          else if (kind == "lao")
            {
              header("vcsn/labelset/oneset.hh");
              os << "vcsn::ctx::oneset";
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

        /// \a getenv(var) if defined, otherwise \a val.
        std::string
        xgetenv(const std::string& var, const std::string& val) const
        {
          const char* cp = getenv(var.c_str());
          return cp ? cp : val;
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
        std::string ctxlibdir() const
        {
          auto tmp = xgetenv("VCSN_TMPDIR", "/tmp");
          auto ctxlibdir = xgetenv("VCSN_CTXLIBDIR", tmp);
          return ctxlibdir + "/";
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
          require (lib, "cannot load lib: ", base, ".so");
        }

        /// Compile, and load, a DSO with instantiations for \a ctx.
        void compile(const std::string& ctx)
        {
          header("vcsn/ctx/instantiate.hh");
          std::string base = ctxlibdir() + context_base::sname(ctx);
          is.str(ctx);
          os << "using ctx_t =" << incendl;
          context();
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
        /// Record that we need an include for this header.
        void header(const std::string& h)
        {
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
  } // namespace dyn
} // namespace vcsn

#endif // !VCSN_DYN_TRANSLATE_HH
