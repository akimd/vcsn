This parser is in charge of reading a weighted rational expression,
and to build the corresponding AST (see expressions.hh).

* Directives, or LL-style?
The precedence between the different operators are very tricky to
determine.  A first attempt, with %precedence directives, failed, and it was
then consider that better insight was given by grammars without precedence
directives (at least for the tricky parts).

But we reached a dead-end, and I (Akim Demaille) failed to see how to
address the remaining issues without the assistance of precedence directives
to simplify the grammar.

I could not see a clean solution to add support for "a<2>*", i.e.,
supporting stars after a weight.  And, I was afraid of the endless
complications that we would face when introducing more operators (say "+"
for one-or-more, "?" etc.).  So in commit
a738fb3c7a72e1292f9d2f386aa6906e5cee797e I went back to using precedence
directives.

* Dealing with the left- and right-weights
The original motivation for not using precedence directives was to parse
correctly:

        <b><b>aa<c><d>aa<b><b>

as (warning: this is no longer how we parse this, see "lmul and mul" below):

        (<bb>(aa).<cc>(aa))<bb>

The problem is when reaching this point:

        <b><b>aa<c> . <d>aa<b><b>

where it is unknown whether the '<c>' should be attached to the previous
'aa' (which would be the case if there were no "aa" afterward:
"<b><b>aa<c><d>"); or if <c> and <d> should be kept for the forthcoming "aa"
(which is the case in the above examples).

The real insight here was to understand that weights should be parsed
together, so that the previous example would actually be parsed as

          <weights1> aa <weights2> aa <weights3>

In which case we can wait to see (as a look-ahead) the forthcoming "aa" to
decide whether to attach <weights2> to the first "aa", or to (shift) keep it
for the next one.

Note that this requires that we are able to store *parsed* weights.
Actually, we could also have made them list of strings to be converted only
when the list of strings is needed (this was actually the first
implementation), but as a result the parsing of weight is not
"synchronized", which causes several issues if you want to have correct
locations in error messages, first errors in the text reported first, etc.

Yet weights cannot be stored abstractly (they can be bool, int, whatever,
with no common root).  Bison does not allow to parametrize the parser, and
anyway, let's avoid more compile-time and code-bloat.

The trick is simple: store the weights W as a rational-expression: "<W>\e",
and simply use the product to apply a weight.

* Dealing with "labels are words"
Everything was fine with "labels are letters", but "labels are words" adds
its own complexity.  We want "abcd" to be a single label, which means that
we want "exp: exp exp" to concatenate labels (using concat()) rather that
multiply them (mul()).

But then, since (of course) parens are syntax-only, "(ab)(ab)" also results
in "abab".  Wrong.

One idea is to have a new non-terminal, say "label", that would collect all
the letters together, so "exp: exp exp" would always use mul(), never
concat().  Wrong again: "ab*" will be parsed as "(ab)*".

So we need to remember whether the expression was in parens, which is done in
braced_expression.  So to handle the "exp" part, we write:

  exp: exp "." exp   { $$.exp = MAKE(mul, $1.exp, $3.exp); }

Victory!

No, not yet.  Consider "(ab)cd".  At this point, "(ab)c . d", the parser
will already have converted this concat by an explicit product since the
first word is in parens, so the stack contains "mul(atom(ab), atom(c))",
and when "d" arrives, again, this is a product, so it results in
"((ab).c).d)".  The trick then is twofold: make the "parens" field of
"(ab).c" be "false" to enable the call to "concat()" rather than "mul()",
and teach "concat(mul(<exp>, <word1>), <word2>)" to return "mul(<exp>,
<word1+2>)".

* How about glr?
Most probably several of these complications can be avoided thanks to GLR.
Unfortunately it is far from being usable in C++, especially because our
semantic values are objects (shared_ptr).

* lmul and mul
At the origin, we were trying to read "<2>abc" as "<2>(abc)".  We stopped
that, because it was actually complex to parse and pretty-print the same
way.  Actually, at some point, I also realized that "<2>abc" when converted
into an automaton becomes clearly "(<2>a)(b)(c)".  So, to improve
consistency between expressions and automaton, I moved to "<2>abc" is
actually "(<2>a)(b)(c)" in LAL.  So in LAW, it's read as "(<2>a)(bc)".

It is somewhat surprising, agreed, but since anyway words should be grouped
in parens, "<2>(abc)" is actually what you should write.

--

 LocalWords:  AST hh Demaille fb aa cee bb bool abcd mul parens abab struct cd
 LocalWords:  dereferenced concat glr ptr utf ispell american

Local Variables:
mode: outline
coding: utf-8
ispell-dictionary: "american"
fill-column: 76
End:
