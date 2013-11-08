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

There are other clean ups to perform in the origin FlexLexer.h file,
they are all included in the "updated" version in
vcsn/misc/flex-lexer.hh.  In particular, we support three additional macros:

**** YY_FLEX_NAMESPACE_BEGIN, YY_FLEX_NAMESPACE_END
The enclose specialized class declaration.  You may also uses these
macros in the *.ll file: the first one at the end of your %{...%}
block, and the latter in the epilogue.  This way, the scanner in
generated in the right namespace.

*** YY_DECL_
We need the signature of yylex twice: once to declare it in its class
(hence not qualified), and another for its implementation (hence
qualified by its class).  Rather than duplicating, do this:

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

--

Local Variables:
mode: outline
coding: utf-8
ispell-dictionary: "american"
fill-column: 76
End:
