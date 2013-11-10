* How we use Flex and Bison together.
** Flex
*** Do not use %option outfile
Because then flex does not obey to -o, which is used by our flex++
wrapper.

*** Do not use %option yyclass
It is really poor: it just defines another YY_DECL macro.  Let's keep
control over this macro (see below).

*** FlexLexer.h
The generated scanner uses this name, and we can't change it.  That's
why we use the flex++ wrapper: to change this include.

There are other clean ups to perform in the origin FlexLexer.h file, they
are all included in the "updated" version in vcsn/misc/flex-lexer.hh with
the following changes.

**** yylex
The definition of yylex in the original FlexLexer.h files is hard-coded to
return an int, and to not take any arguments.  This restriction is lifted:
the base class no longer defines yylex as virtual, and the derived class
uses YY_DECL_ to declare yylex, so it is under user control.  See YY_DECL_
below.

**** YY_FLEX_NAMESPACE_BEGIN, YY_FLEX_NAMESPACE_END
The enclose specialized class declaration.  You may also uses these
macros in the *.ll file: the first one at the end of your %{...%}
block, and the latter in the epilogue.  This way, the scanner in
generated in the right namespace.

**** yyalloc, yyrealloc, yyfree
Flex declares yyalloc, yyrealloc and yyfree in the output file (scan.cc)
right before the user sections, therefore right before our
YY_FLEX_NAMESPACE_BEGIN, but it implements them within this namespace.

We address this in two steps: (i) disable the declarations emitted by Flex
(flex++ does this), (ii) instead declare them within the
YY_FLEX_NAMESPACE_BEGIN in flex-lexer.hh.

*** %option prefix is useless
We use YY_FLEX_NAMESPACE_BEGIN/END to put symbols in different namespaces,
so we don't need to rename yyFlexLexer, yyalloc etc.  Actually, it is even
simpler _not_ to rename them, as then, for instance, we can declare yyalloc
and the like in the FlexLexer header.

Instead of %option prefix, define a short header (scan.hh) which includes
flex-lexer.hh just after having defined the two YY_FLEX_NAMESPACE_BEGIN/END
macros.  Don't use flex-lexer.hh directly.

*** YY_DECL_
We need the signature of yylex twice: once to declare it in its class
definition (hence not class-qualified) in flex-lexer.hh, and another for its
implementation (hence qualified by its class) in the generated scan.cc.
Rather than duplicating, do this:

  %code provides
  {
    #define YY_DECL_(Class)                               \
      int Class lex(parser::semantic_type* yylval,        \
                    parser::location_type* yylloc,        \
                    driver& driver_)
    #define YY_DECL YY_DECL_(yyFlexLexer::)
  }

in the *.yy file.  Note that you don't have to put the namespaces,
since all this will be in a
YY_FLEX_NAMESPACE_BEGIN-YY_FLEX_NAMESPACE_END pair.

** Bison
*** Do not use %name-prefix
Because then it #define yylex <prefix>lex, which is troublesome for us.

*** Provide a yylex wrapper
Use

  %parse-param { driver& driver_ }
  %lex-param   { driver& driver_ }

to make sure that the driver is passed to the scanner, and

  %code top
  {
    /// Use our local scanner object.
    static
    inline
    int
    yylex(parser::semantic_type* yylval,
          parser::location_type* yylloc,
          driver& driver_)
    {
      return driver_.scanner_->lex(yylval, yylloc, driver_);
    }
  }

to provide the parser with an access to its scanner.

--

Local Variables:
mode: outline
coding: utf-8
ispell-dictionary: "american"
fill-column: 76
End:
