SMS2FR
======

Translate text message abbreviations to actual french.
This program is available with vcsn version v2.1-218-g47f7313.

This program is implemented using the three different levels of usage of the
vcsn library (static, dyn and python) and uses two specific automata.
Both of these automata were created using a machine learning method.
The first one is used to add the different syllable with a similar sound as
those from the text (graphemic model).
The second one treats the probabilities of those changes considering the syntax
of the text (syntactic model).

          b         o
  --> 0 ------> 1 ------> 2 -->

After compose with graphemic model (weights will depend on frequency of usage
in the language):
          <1>b       <1>o, <2>au, <2>eau
  --> 0 --------> 1 ---------------------> 2 -->

After compose with syntactic model (weights will depend on probability of
presence of this grapheme after the letter b):
          <1>b       <2>o, <3>au, <1>eau
  --> 0 --------> 1 ---------------------> 2 -->

Notes:
  - This example does not rely on actual weights and labels from the
algorithm and these values were chosen as theoritical values.
The algorithm will choose the path with the lightest weight (in the tropical
weightset Rmin), here: 'beau'.
  - The characters '#', '[' and ']' are considered special characters by the
trained automata and not accepted in the original text.
