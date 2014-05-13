#! /usr/bin/env python

import vcsn
from test import *

b = vcsn.context('lal_char(abcd)_b')

## ---------------------- ##
## Existing transitions.  ##
## ---------------------- ##

# See the actual code of product to understand the point of this test
# (which is new_transition vs. add_transition).
a1 = b.ratexp('a*a').derived_term()
a2 = a1 & a1
CHECK_EQ('a*a', str(a2.ratexp()))

## ---------------------- ##
## (a+b)* & (b+c)* = b*.  ##
## ---------------------- ##

lhs = vcsn.context('lal_char(ab)_b').ratexp('(a+b)*').standard()
rhs = vcsn.context('lal_char(bc)_b').ratexp('(b+c)*').standard()
CHECK_EQ(vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(b)_b"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    F0
    F1
  }
  {
    node [shape = circle]
    0
    1
  }
  I0 -> 0
  0 -> F0
  0 -> 1 [label = "b"]
  1 -> F1
  1 -> 1 [label = "b"]
}
'''),
         lhs & rhs)

## ------------- ##
## ab & cd = 0.  ##
## ------------- ##

lhs = vcsn.context('lal_char(ab)_b').ratexp('ab').standard()
rhs = vcsn.context('lal_char(cd)_b').ratexp('cd').standard()
CHECK_EQ(vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char()_b"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
  }
  {
    node [shape = circle]
    0 [color = DimGray]
  }
  I0 -> 0 [color = DimGray]
}
'''), lhs & rhs)



## ---------------------- ##
## (a+b)* & (c+d)* = \e.  ##
## ---------------------- ##

lhs = vcsn.context('lal_char(ab)_b').ratexp('(a+b)*').standard()
rhs = vcsn.context('lal_char(cd)_b').ratexp('(c+d)*').standard()
CHECK_EQ(vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char()_b"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    F0
  }
  {
    node [shape = circle]
    0
  }
  I0 -> 0
  0 -> F0
}
'''),
         lhs & rhs)



## ------------ ##
## lal_char_z.  ##
## ------------ ##

# <2>(a*b*a*)
lhs = vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(abc)_z"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    F0
    F1
    F2
  }
  {
    node [shape = circle]
    0
    1
    2
  }
  I0 -> 0 [label = "<2>"]
  0 -> F0
  0 -> 0 [label = "a"]
  0 -> 1 [label = "b"]
  0 -> 2 [label = "a"]
  1 -> F1
  1 -> 1 [label = "b"]
  1 -> 2 [label = "a"]
  2 -> F2
  2 -> 2 [label = "a"]
}
''')

# (<3>(ab))*
rhs = vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(abc)_z"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    F0
  }
  {
    node [shape = circle]
    0
    1
  }
  I0 -> 0 [label = "<3>"]
  0 -> F0
  0 -> 1 [label = "a"]
  1 -> 0 [label = "<3>b"]
}
''')

exp = vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(abc)_z"
  rankdir = LR
  {
    node [shape = point, width = 0]
    I0
    F0
    F3
  }
  {
    node [shape = circle]
    0
    1
    2 [color = DimGray]
    3
  }
  I0 -> 0 [label = "<6>"]
  0 -> F0
  0 -> 1 [label = "a"]
  0 -> 2 [label = "a", color = DimGray]
  1 -> 3 [label = "<3>b"]
  3 -> F3
  3 -> 2 [label = "a", color = DimGray]
}
''')

CHECK_EQ(exp, lhs & rhs)

## ------------------------------------ ##
## Heterogeneous (and variadic) input.  ##
## ------------------------------------ ##

# check OPERATION RES AUT...
# --------------------------
def check(operation, exp, *args):
    CHECK_EQ(exp, str(vcsn.automaton._product(list(args)).ratexp()))

# RatE and B, in both directions.
a1 = vcsn.context('lal_char(ab)_ratexpset<lal_char(uv)_b>') \
         .ratexp('(<u>a+<v>b)*').standard()
a2 = vcsn.context('lal_char(ab)_b').ratexp('a{+}').standard()
check('product', '<u>a+<u>a<u>a(<u>a)*', a1, a2)
check('product', '<u>a+<u>a<u>a(<u>a)*', a2, a1)

# Z, Q, R.
z = vcsn.context('lal_char(ab)_z').ratexp('(<2>a+<3>b)*')    .derived_term()
q = vcsn.context('lal_char(ab)_q').ratexp('(<1/2>a+<1/3>b)*').derived_term()
r = vcsn.context('lal_char(ab)_r').ratexp('(<.2>a+<.3>b)*')  .derived_term()

check('product', '(a+b)*', z, q)
check('product', '(a+b)*', q, z)
check('product', '(<2>a+<3>b)*', z, q, z)
check('product', '(<1/2>a+<1/3>b)*', z, q, q)

check('product', '(<0.4>a+<0.9>b)*', z, r)
check('product', '(<0.4>a+<0.9>b)*', r, z)

check('product', '(<0.1>a+<0.1>b)*', q, r)
check('product', '(<0.1>a+<0.1>b)*', r, q)


## ----------------- ##
## Non-commutative.  ##
## ----------------- ##

a1 = vcsn.context('lal_char(ab)_ratexpset<lal_char(uv)_b>') \
         .ratexp('<u>a<v>b').standard()
a2 = vcsn.context('lal_char(ab)_ratexpset<lal_char(xy)_b>') \
         .ratexp('<x>a<y>b').standard()

def check_enumerate(exp, aut):
    CHECK_EQ(exp, str(aut.enumerate(4)))

check_enumerate('<uxvy>ab', a1 & a2)
check_enumerate('\z', a1.transpose() & a2)
check_enumerate('\z', a1 & a2.transpose())
check_enumerate('<vyux>ba', a1.transpose() & a2.transpose())


## ---------- ##
## variadic.  ##
## ---------- ##

# unary case: return only the accessible part.
CHECK_EQ(vcsn.automaton('''
digraph {
  vcsn_context = "lal_char(ab)_b"
  I -> 0
  0 -> 1 [label="a"]
  1 -> 2 [label="a"]
}
'''), vcsn.automaton._product([vcsn.automaton('''
digraph {
  vcsn_context = "lal_char(ab)_b"
  I -> 0
  0 -> 1 [label="a"]
  1 -> 2 [label="a"]
}
''')]))

# four arguments.
ctx = vcsn.context('lal_char(x)_ratexpset<lal_char(abcd)_b>')
a = dict()
for l in ['a', 'b', 'c', 'd']:
    a[l] = ctx.ratexp("<{}>x".format(l)).standard()
check_enumerate('<abcd>x', a['a'] & a['b'] & a['c'] & a['d'])


## ----------------- ##
## ratexp & ratexp.  ##
## ----------------- ##

# Add stars (<u>a*, not <u>a) to avoid that the trivial identities
# (a&b -> \z) fire and yield a global \z.
br = vcsn.context('lal_char(a)_ratexpset<lal_char(uv)_b>') \
         .ratexp('<u>a*')
z = vcsn.context('lal_char(b)_z').ratexp('<2>b*')
q = vcsn.context('lal_char(c)_q').ratexp('<1/3>c*')
r = vcsn.context('lal_char(d)_r').ratexp('<.4>d*')
CHECK_EQ('<u>a*&<<2>\e>b*&<<0.333333>\e>c*&<<0.4>\e>d*', str(br & z & q & r))

## ----------------- ##
## nullable labels.  ##
## ----------------- ##

def check_equivalent(a1, a2):
    if a1.is_equivalent(a2):
        PASS()
    else:
        FAIL(a1 + " not equivalent to " + a2);

lhs = vcsn.context('lan_char(ab)_b').ratexp('(a+b)*').thompson()
rhs = vcsn.context('lan_char(bc)_b').ratexp('(b+c)*').thompson()
res = vcsn.automaton(r'''digraph
{
  vcsn_context = "lan<lal_char(b)>_b"
  rankdir = LR
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
    I0
    F10
  }
  {
    node [shape = circle]
    0
    1
    2
    3 [color = DimGray]
    4 [color = DimGray]
    5 [color = DimGray]
    6
    7 [color = DimGray]
    8 [color = DimGray]
    9 [color = DimGray]
    10
    11 [color = DimGray]
    12 [color = DimGray]
    13 [color = DimGray]
    14 [color = DimGray]
    15
    16 [color = DimGray]
    17 [color = DimGray]
    18 [color = DimGray]
    19 [color = DimGray]
    20 [color = DimGray]
    21 [color = DimGray]
    22 [color = DimGray]
    23
    24 [color = DimGray]
    25
    26
    27 [color = DimGray]
    28
    29
    30 [color = DimGray]
    31 [color = DimGray]
    32 [color = DimGray]
    33
    34 [color = DimGray]
    35
    36 [color = DimGray]
    37 [color = DimGray]
    38 [color = DimGray]
    39 [color = DimGray]
    40 [color = DimGray]
    41 [color = DimGray]
    42
    43 [color = DimGray]
    44 [color = DimGray]
  }
  I0 -> 0
  0 -> 1 [label = "\\e"]
  0 -> 2 [label = "\\e"]
  0 -> 3 [label = "\\e", color = DimGray]
  0 -> 4 [label = "\\e", color = DimGray]
  1 -> 5 [label = "\\e", color = DimGray]
  1 -> 6 [label = "\\e"]
  1 -> 7 [label = "\\e", color = DimGray]
  1 -> 8 [label = "\\e", color = DimGray]
  2 -> 9 [label = "\\e", color = DimGray]
  2 -> 10 [label = "\\e"]
  3 -> 11 [label = "\\e", color = DimGray]
  3 -> 12 [label = "\\e", color = DimGray]
  5 -> 13 [label = "\\e", color = DimGray]
  5 -> 14 [label = "\\e", color = DimGray]
  6 -> 15 [label = "\\e"]
  6 -> 16 [label = "\\e", color = DimGray]
  7 -> 17 [label = "\\e", color = DimGray]
  7 -> 18 [label = "\\e", color = DimGray]
  9 -> 19 [label = "\\e", color = DimGray]
  9 -> 20 [label = "\\e", color = DimGray]
  10 -> F10
  13 -> 21 [label = "\\e", color = DimGray]
  13 -> 22 [label = "\\e", color = DimGray]
  15 -> 23 [label = "\\e"]
  15 -> 24 [label = "\\e", color = DimGray]
  23 -> 25 [label = "b"]
  25 -> 26 [label = "\\e"]
  25 -> 27 [label = "\\e", color = DimGray]
  26 -> 28 [label = "\\e"]
  26 -> 29 [label = "\\e"]
  26 -> 30 [label = "\\e", color = DimGray]
  27 -> 31 [label = "\\e", color = DimGray]
  27 -> 32 [label = "\\e", color = DimGray]
  28 -> 33 [label = "\\e"]
  29 -> 34 [label = "\\e", color = DimGray]
  29 -> 35 [label = "\\e"]
  29 -> 36 [label = "\\e", color = DimGray]
  30 -> 37 [label = "\\e", color = DimGray]
  30 -> 38 [label = "\\e", color = DimGray]
  32 -> 39 [label = "\\e", color = DimGray]
  32 -> 40 [label = "\\e", color = DimGray]
  33 -> 9 [label = "\\e", color = DimGray]
  33 -> 10 [label = "\\e"]
  34 -> 41 [label = "\\e", color = DimGray]
  35 -> 42 [label = "\\e"]
  36 -> 7 [label = "\\e", color = DimGray]
  36 -> 8 [label = "\\e", color = DimGray]
  38 -> 43 [label = "\\e", color = DimGray]
  38 -> 44 [label = "\\e", color = DimGray]
  41 -> 13 [label = "\\e", color = DimGray]
  41 -> 14 [label = "\\e", color = DimGray]
  42 -> 15 [label = "\\e"]
  42 -> 16 [label = "\\e", color = DimGray]
}''')
CHECK_EQ(res, lhs & rhs)
check_equivalent(res.proper(), vcsn.context("lal_char(b)_b").ratexp("b*").standard())

third = vcsn.context('lan_char(bcd)_b').ratexp('(b+c+d)*').thompson()
res = vcsn.automaton(r'''digraph
{
  vcsn_context = "lan<lal_char(b)>_b"
  rankdir = LR
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
    I0
    F48
  }
  {
    node [shape = circle]
    0
    1
    2
    3 [color = DimGray]
    4 [color = DimGray]
    5 [color = DimGray]
    6 [color = DimGray]
    7 [color = DimGray]
    8
    9 [color = DimGray]
    10 [color = DimGray]
    11 [color = DimGray]
    12 [color = DimGray]
    13 [color = DimGray]
    14
    15 [color = DimGray]
    16 [color = DimGray]
    17 [color = DimGray]
    18 [color = DimGray]
    19 [color = DimGray]
    20 [color = DimGray]
    21 [color = DimGray]
    22 [color = DimGray]
    23 [color = DimGray]
    24 [color = DimGray]
    25 [color = DimGray]
    26 [color = DimGray]
    27 [color = DimGray]
    28 [color = DimGray]
    29 [color = DimGray]
    30
    31 [color = DimGray]
    32 [color = DimGray]
    33 [color = DimGray]
    34 [color = DimGray]
    35 [color = DimGray]
    36 [color = DimGray]
    37 [color = DimGray]
    38 [color = DimGray]
    39 [color = DimGray]
    40 [color = DimGray]
    41 [color = DimGray]
    42 [color = DimGray]
    43 [color = DimGray]
    44 [color = DimGray]
    45 [color = DimGray]
    46 [color = DimGray]
    47 [color = DimGray]
    48
    49 [color = DimGray]
    50 [color = DimGray]
    51 [color = DimGray]
    52 [color = DimGray]
    53 [color = DimGray]
    54 [color = DimGray]
    55 [color = DimGray]
    56 [color = DimGray]
    57 [color = DimGray]
    58 [color = DimGray]
    59 [color = DimGray]
    60 [color = DimGray]
    61 [color = DimGray]
    62 [color = DimGray]
    63 [color = DimGray]
    64 [color = DimGray]
    65 [color = DimGray]
    66 [color = DimGray]
    67 [color = DimGray]
    68 [color = DimGray]
    69 [color = DimGray]
    70 [color = DimGray]
    71
    72 [color = DimGray]
    73 [color = DimGray]
    74 [color = DimGray]
    75 [color = DimGray]
    76 [color = DimGray]
    77 [color = DimGray]
    78 [color = DimGray]
    79 [color = DimGray]
    80 [color = DimGray]
    81 [color = DimGray]
    82 [color = DimGray]
    83 [color = DimGray]
    84 [color = DimGray]
    85 [color = DimGray]
    86 [color = DimGray]
    87 [color = DimGray]
    88 [color = DimGray]
    89 [color = DimGray]
    90 [color = DimGray]
    91 [color = DimGray]
    92 [color = DimGray]
    93 [color = DimGray]
    94 [color = DimGray]
    95 [color = DimGray]
    96 [color = DimGray]
    97 [color = DimGray]
    98 [color = DimGray]
    99 [color = DimGray]
    100 [color = DimGray]
    101 [color = DimGray]
    102 [color = DimGray]
    103 [color = DimGray]
    104 [color = DimGray]
    105 [color = DimGray]
    106 [color = DimGray]
    107 [color = DimGray]
    108 [color = DimGray]
    109 [color = DimGray]
    110 [color = DimGray]
    111 [color = DimGray]
    112 [color = DimGray]
    113 [color = DimGray]
    114 [color = DimGray]
    115 [color = DimGray]
    116
    117 [color = DimGray]
    118 [color = DimGray]
    119 [color = DimGray]
    120 [color = DimGray]
    121 [color = DimGray]
    122 [color = DimGray]
    123 [color = DimGray]
    124 [color = DimGray]
    125 [color = DimGray]
    126 [color = DimGray]
    127 [color = DimGray]
    128 [color = DimGray]
    129 [color = DimGray]
    130 [color = DimGray]
    131 [color = DimGray]
    132 [color = DimGray]
    133 [color = DimGray]
    134 [color = DimGray]
    135 [color = DimGray]
    136 [color = DimGray]
    137 [color = DimGray]
    138 [color = DimGray]
    139 [color = DimGray]
    140 [color = DimGray]
    141 [color = DimGray]
    142 [color = DimGray]
    143 [color = DimGray]
    144
    145 [color = DimGray]
    146 [color = DimGray]
    147 [color = DimGray]
    148 [color = DimGray]
    149 [color = DimGray]
    150
    151
    152 [color = DimGray]
    153 [color = DimGray]
    154
    155
    156 [color = DimGray]
    157 [color = DimGray]
    158 [color = DimGray]
    159 [color = DimGray]
    160 [color = DimGray]
    161 [color = DimGray]
    162 [color = DimGray]
    163
    164 [color = DimGray]
    165 [color = DimGray]
    166
    167 [color = DimGray]
    168 [color = DimGray]
    169 [color = DimGray]
    170 [color = DimGray]
    171 [color = DimGray]
    172 [color = DimGray]
    173 [color = DimGray]
    174 [color = DimGray]
    175 [color = DimGray]
    176 [color = DimGray]
    177 [color = DimGray]
    178 [color = DimGray]
    179 [color = DimGray]
    180 [color = DimGray]
    181 [color = DimGray]
    182 [color = DimGray]
    183
    184 [color = DimGray]
    185 [color = DimGray]
    186 [color = DimGray]
    187 [color = DimGray]
    188 [color = DimGray]
    189 [color = DimGray]
    190
    191 [color = DimGray]
    192 [color = DimGray]
    193 [color = DimGray]
    194 [color = DimGray]
    195 [color = DimGray]
    196 [color = DimGray]
    197 [color = DimGray]
    198 [color = DimGray]
    199 [color = DimGray]
    200 [color = DimGray]
    201 [color = DimGray]
    202 [color = DimGray]
    203 [color = DimGray]
    204 [color = DimGray]
    205 [color = DimGray]
    206 [color = DimGray]
    207 [color = DimGray]
    208 [color = DimGray]
    209 [color = DimGray]
    210 [color = DimGray]
    211 [color = DimGray]
    212 [color = DimGray]
    213 [color = DimGray]
    214 [color = DimGray]
    215
    216 [color = DimGray]
    217 [color = DimGray]
    218 [color = DimGray]
    219 [color = DimGray]
    220 [color = DimGray]
    221 [color = DimGray]
    222 [color = DimGray]
    223 [color = DimGray]
    224 [color = DimGray]
    225 [color = DimGray]
    226 [color = DimGray]
    227 [color = DimGray]
    228 [color = DimGray]
    229 [color = DimGray]
    230
    231 [color = DimGray]
    232 [color = DimGray]
    233 [color = DimGray]
    234 [color = DimGray]
    235 [color = DimGray]
    236 [color = DimGray]
    237 [color = DimGray]
    238 [color = DimGray]
    239 [color = DimGray]
    240 [color = DimGray]
    241 [color = DimGray]
    242 [color = DimGray]
    243 [color = DimGray]
    244 [color = DimGray]
    245 [color = DimGray]
    246 [color = DimGray]
    247 [color = DimGray]
    248 [color = DimGray]
    249 [color = DimGray]
    250 [color = DimGray]
    251 [color = DimGray]
    252 [color = DimGray]
    253 [color = DimGray]
    254 [color = DimGray]
    255 [color = DimGray]
    256 [color = DimGray]
    257 [color = DimGray]
    258 [color = DimGray]
    259 [color = DimGray]
    260 [color = DimGray]
    261 [color = DimGray]
    262 [color = DimGray]
    263 [color = DimGray]
    264 [color = DimGray]
    265 [color = DimGray]
    266 [color = DimGray]
    267 [color = DimGray]
    268 [color = DimGray]
    269 [color = DimGray]
    270 [color = DimGray]
    271 [color = DimGray]
    272 [color = DimGray]
    273 [color = DimGray]
    274 [color = DimGray]
    275 [color = DimGray]
    276 [color = DimGray]
    277
    278 [color = DimGray]
    279 [color = DimGray]
    280 [color = DimGray]
    281 [color = DimGray]
    282 [color = DimGray]
    283 [color = DimGray]
    284 [color = DimGray]
    285 [color = DimGray]
    286 [color = DimGray]
    287 [color = DimGray]
    288 [color = DimGray]
    289 [color = DimGray]
    290 [color = DimGray]
    291 [color = DimGray]
    292 [color = DimGray]
    293 [color = DimGray]
    294 [color = DimGray]
    295 [color = DimGray]
    296 [color = DimGray]
    297 [color = DimGray]
    298 [color = DimGray]
    299 [color = DimGray]
    300 [color = DimGray]
    301 [color = DimGray]
    302 [color = DimGray]
    303 [color = DimGray]
    304 [color = DimGray]
    305 [color = DimGray]
    306 [color = DimGray]
    307 [color = DimGray]
    308 [color = DimGray]
    309 [color = DimGray]
    310 [color = DimGray]
    311
    312 [color = DimGray]
    313 [color = DimGray]
    314 [color = DimGray]
    315 [color = DimGray]
    316 [color = DimGray]
    317 [color = DimGray]
    318 [color = DimGray]
    319 [color = DimGray]
    320 [color = DimGray]
    321 [color = DimGray]
  }
  I0 -> 0
  0 -> 1 [label = "\\e"]
  0 -> 2 [label = "\\e"]
  0 -> 3 [label = "\\e", color = DimGray]
  0 -> 4 [label = "\\e", color = DimGray]
  0 -> 5 [label = "\\e", color = DimGray]
  0 -> 6 [label = "\\e", color = DimGray]
  1 -> 7 [label = "\\e", color = DimGray]
  1 -> 8 [label = "\\e"]
  1 -> 9 [label = "\\e", color = DimGray]
  1 -> 10 [label = "\\e", color = DimGray]
  1 -> 11 [label = "\\e", color = DimGray]
  1 -> 12 [label = "\\e", color = DimGray]
  2 -> 13 [label = "\\e", color = DimGray]
  2 -> 14 [label = "\\e"]
  2 -> 15 [label = "\\e", color = DimGray]
  2 -> 16 [label = "\\e", color = DimGray]
  3 -> 17 [label = "\\e", color = DimGray]
  3 -> 18 [label = "\\e", color = DimGray]
  3 -> 19 [label = "\\e", color = DimGray]
  3 -> 20 [label = "\\e", color = DimGray]
  4 -> 21 [label = "\\e", color = DimGray]
  4 -> 22 [label = "\\e", color = DimGray]
  5 -> 23 [label = "\\e", color = DimGray]
  5 -> 24 [label = "\\e", color = DimGray]
  5 -> 25 [label = "\\e", color = DimGray]
  7 -> 26 [label = "\\e", color = DimGray]
  7 -> 27 [label = "\\e", color = DimGray]
  7 -> 28 [label = "\\e", color = DimGray]
  7 -> 29 [label = "\\e", color = DimGray]
  8 -> 30 [label = "\\e"]
  8 -> 31 [label = "\\e", color = DimGray]
  8 -> 32 [label = "\\e", color = DimGray]
  8 -> 33 [label = "\\e", color = DimGray]
  9 -> 34 [label = "\\e", color = DimGray]
  9 -> 35 [label = "\\e", color = DimGray]
  9 -> 36 [label = "\\e", color = DimGray]
  9 -> 37 [label = "\\e", color = DimGray]
  10 -> 38 [label = "\\e", color = DimGray]
  10 -> 39 [label = "\\e", color = DimGray]
  11 -> 40 [label = "\\e", color = DimGray]
  11 -> 41 [label = "\\e", color = DimGray]
  11 -> 42 [label = "\\e", color = DimGray]
  13 -> 43 [label = "\\e", color = DimGray]
  13 -> 44 [label = "\\e", color = DimGray]
  13 -> 45 [label = "\\e", color = DimGray]
  13 -> 46 [label = "\\e", color = DimGray]
  14 -> 47 [label = "\\e", color = DimGray]
  14 -> 48 [label = "\\e"]
  15 -> 49 [label = "\\e", color = DimGray]
  15 -> 50 [label = "\\e", color = DimGray]
  15 -> 51 [label = "\\e", color = DimGray]
  17 -> 52 [label = "\\e", color = DimGray]
  17 -> 53 [label = "\\e", color = DimGray]
  18 -> 54 [label = "\\e", color = DimGray]
  18 -> 55 [label = "\\e", color = DimGray]
  19 -> 56 [label = "\\e", color = DimGray]
  19 -> 57 [label = "\\e", color = DimGray]
  19 -> 58 [label = "\\e", color = DimGray]
  21 -> 59 [label = "\\e", color = DimGray]
  21 -> 60 [label = "\\e", color = DimGray]
  21 -> 61 [label = "\\e", color = DimGray]
  26 -> 62 [label = "\\e", color = DimGray]
  26 -> 63 [label = "\\e", color = DimGray]
  26 -> 64 [label = "\\e", color = DimGray]
  26 -> 65 [label = "\\e", color = DimGray]
  27 -> 66 [label = "\\e", color = DimGray]
  27 -> 67 [label = "\\e", color = DimGray]
  28 -> 68 [label = "\\e", color = DimGray]
  28 -> 69 [label = "\\e", color = DimGray]
  28 -> 70 [label = "\\e", color = DimGray]
  30 -> 71 [label = "\\e"]
  30 -> 72 [label = "\\e", color = DimGray]
  30 -> 73 [label = "\\e", color = DimGray]
  30 -> 74 [label = "\\e", color = DimGray]
  31 -> 75 [label = "\\e", color = DimGray]
  31 -> 76 [label = "\\e", color = DimGray]
  32 -> 77 [label = "\\e", color = DimGray]
  32 -> 78 [label = "\\e", color = DimGray]
  32 -> 79 [label = "\\e", color = DimGray]
  34 -> 80 [label = "\\e", color = DimGray]
  34 -> 81 [label = "\\e", color = DimGray]
  35 -> 82 [label = "\\e", color = DimGray]
  35 -> 83 [label = "\\e", color = DimGray]
  36 -> 84 [label = "\\e", color = DimGray]
  36 -> 85 [label = "\\e", color = DimGray]
  36 -> 86 [label = "\\e", color = DimGray]
  38 -> 87 [label = "\\e", color = DimGray]
  38 -> 88 [label = "\\e", color = DimGray]
  38 -> 89 [label = "\\e", color = DimGray]
  43 -> 90 [label = "\\e", color = DimGray]
  43 -> 91 [label = "\\e", color = DimGray]
  44 -> 92 [label = "\\e", color = DimGray]
  44 -> 93 [label = "\\e", color = DimGray]
  45 -> 94 [label = "\\e", color = DimGray]
  45 -> 95 [label = "\\e", color = DimGray]
  45 -> 96 [label = "\\e", color = DimGray]
  47 -> 97 [label = "\\e", color = DimGray]
  47 -> 98 [label = "\\e", color = DimGray]
  47 -> 99 [label = "\\e", color = DimGray]
  48 -> F48
  52 -> 100 [label = "\\e", color = DimGray]
  52 -> 101 [label = "\\e", color = DimGray]
  52 -> 102 [label = "\\e", color = DimGray]
  54 -> 103 [label = "\\e", color = DimGray]
  54 -> 104 [label = "\\e", color = DimGray]
  54 -> 105 [label = "\\e", color = DimGray]
  62 -> 106 [label = "\\e", color = DimGray]
  62 -> 107 [label = "\\e", color = DimGray]
  63 -> 108 [label = "\\e", color = DimGray]
  63 -> 109 [label = "\\e", color = DimGray]
  64 -> 110 [label = "\\e", color = DimGray]
  64 -> 111 [label = "\\e", color = DimGray]
  64 -> 112 [label = "\\e", color = DimGray]
  66 -> 113 [label = "\\e", color = DimGray]
  66 -> 114 [label = "\\e", color = DimGray]
  66 -> 115 [label = "\\e", color = DimGray]
  71 -> 116 [label = "\\e"]
  71 -> 117 [label = "\\e", color = DimGray]
  72 -> 118 [label = "\\e", color = DimGray]
  72 -> 119 [label = "\\e", color = DimGray]
  73 -> 120 [label = "\\e", color = DimGray]
  73 -> 121 [label = "\\e", color = DimGray]
  73 -> 122 [label = "\\e", color = DimGray]
  75 -> 123 [label = "\\e", color = DimGray]
  75 -> 124 [label = "\\e", color = DimGray]
  75 -> 125 [label = "\\e", color = DimGray]
  80 -> 126 [label = "\\e", color = DimGray]
  80 -> 127 [label = "\\e", color = DimGray]
  80 -> 128 [label = "\\e", color = DimGray]
  82 -> 129 [label = "\\e", color = DimGray]
  82 -> 130 [label = "\\e", color = DimGray]
  82 -> 131 [label = "\\e", color = DimGray]
  90 -> 132 [label = "\\e", color = DimGray]
  90 -> 133 [label = "\\e", color = DimGray]
  90 -> 134 [label = "\\e", color = DimGray]
  92 -> 135 [label = "\\e", color = DimGray]
  92 -> 136 [label = "\\e", color = DimGray]
  92 -> 137 [label = "\\e", color = DimGray]
  106 -> 138 [label = "\\e", color = DimGray]
  106 -> 139 [label = "\\e", color = DimGray]
  106 -> 140 [label = "\\e", color = DimGray]
  108 -> 141 [label = "\\e", color = DimGray]
  108 -> 142 [label = "\\e", color = DimGray]
  108 -> 143 [label = "\\e", color = DimGray]
  116 -> 144 [label = "\\e"]
  116 -> 145 [label = "\\e", color = DimGray]
  116 -> 146 [label = "\\e", color = DimGray]
  118 -> 147 [label = "\\e", color = DimGray]
  118 -> 148 [label = "\\e", color = DimGray]
  118 -> 149 [label = "\\e", color = DimGray]
  144 -> 150 [label = "b"]
  150 -> 151 [label = "\\e"]
  150 -> 152 [label = "\\e", color = DimGray]
  150 -> 153 [label = "\\e", color = DimGray]
  151 -> 154 [label = "\\e"]
  151 -> 155 [label = "\\e"]
  151 -> 156 [label = "\\e", color = DimGray]
  151 -> 157 [label = "\\e", color = DimGray]
  152 -> 158 [label = "\\e", color = DimGray]
  152 -> 159 [label = "\\e", color = DimGray]
  152 -> 160 [label = "\\e", color = DimGray]
  153 -> 161 [label = "\\e", color = DimGray]
  153 -> 162 [label = "\\e", color = DimGray]
  154 -> 163 [label = "\\e"]
  154 -> 164 [label = "\\e", color = DimGray]
  155 -> 165 [label = "\\e", color = DimGray]
  155 -> 166 [label = "\\e"]
  155 -> 167 [label = "\\e", color = DimGray]
  155 -> 168 [label = "\\e", color = DimGray]
  156 -> 169 [label = "\\e", color = DimGray]
  156 -> 170 [label = "\\e", color = DimGray]
  156 -> 171 [label = "\\e", color = DimGray]
  157 -> 172 [label = "\\e", color = DimGray]
  157 -> 173 [label = "\\e", color = DimGray]
  158 -> 174 [label = "\\e", color = DimGray]
  159 -> 175 [label = "\\e", color = DimGray]
  159 -> 176 [label = "\\e", color = DimGray]
  159 -> 177 [label = "\\e", color = DimGray]
  160 -> 178 [label = "\\e", color = DimGray]
  160 -> 179 [label = "\\e", color = DimGray]
  162 -> 180 [label = "\\e", color = DimGray]
  162 -> 181 [label = "\\e", color = DimGray]
  162 -> 182 [label = "\\e", color = DimGray]
  163 -> 183 [label = "\\e"]
  163 -> 184 [label = "\\e", color = DimGray]
  163 -> 185 [label = "\\e", color = DimGray]
  164 -> 186 [label = "\\e", color = DimGray]
  164 -> 187 [label = "\\e", color = DimGray]
  165 -> 188 [label = "\\e", color = DimGray]
  165 -> 189 [label = "\\e", color = DimGray]
  166 -> 190 [label = "\\e"]
  166 -> 191 [label = "\\e", color = DimGray]
  167 -> 192 [label = "\\e", color = DimGray]
  167 -> 193 [label = "\\e", color = DimGray]
  167 -> 194 [label = "\\e", color = DimGray]
  168 -> 195 [label = "\\e", color = DimGray]
  168 -> 196 [label = "\\e", color = DimGray]
  169 -> 197 [label = "\\e", color = DimGray]
  170 -> 198 [label = "\\e", color = DimGray]
  170 -> 199 [label = "\\e", color = DimGray]
  170 -> 200 [label = "\\e", color = DimGray]
  171 -> 201 [label = "\\e", color = DimGray]
  171 -> 202 [label = "\\e", color = DimGray]
  173 -> 203 [label = "\\e", color = DimGray]
  173 -> 204 [label = "\\e", color = DimGray]
  173 -> 205 [label = "\\e", color = DimGray]
  174 -> 206 [label = "\\e", color = DimGray]
  174 -> 207 [label = "\\e", color = DimGray]
  175 -> 208 [label = "\\e", color = DimGray]
  176 -> 209 [label = "\\e", color = DimGray]
  177 -> 210 [label = "\\e", color = DimGray]
  177 -> 211 [label = "\\e", color = DimGray]
  179 -> 212 [label = "\\e", color = DimGray]
  179 -> 213 [label = "\\e", color = DimGray]
  179 -> 214 [label = "\\e", color = DimGray]
  183 -> 215 [label = "\\e"]
  184 -> 216 [label = "\\e", color = DimGray]
  184 -> 217 [label = "\\e", color = DimGray]
  184 -> 218 [label = "\\e", color = DimGray]
  185 -> 219 [label = "\\e", color = DimGray]
  185 -> 220 [label = "\\e", color = DimGray]
  187 -> 221 [label = "\\e", color = DimGray]
  187 -> 222 [label = "\\e", color = DimGray]
  187 -> 223 [label = "\\e", color = DimGray]
  188 -> 224 [label = "\\e", color = DimGray]
  188 -> 225 [label = "\\e", color = DimGray]
  188 -> 226 [label = "\\e", color = DimGray]
  189 -> 227 [label = "\\e", color = DimGray]
  189 -> 228 [label = "\\e", color = DimGray]
  190 -> 229 [label = "\\e", color = DimGray]
  190 -> 230 [label = "\\e"]
  190 -> 231 [label = "\\e", color = DimGray]
  191 -> 232 [label = "\\e", color = DimGray]
  191 -> 233 [label = "\\e", color = DimGray]
  192 -> 234 [label = "\\e", color = DimGray]
  193 -> 235 [label = "\\e", color = DimGray]
  193 -> 236 [label = "\\e", color = DimGray]
  193 -> 237 [label = "\\e", color = DimGray]
  194 -> 238 [label = "\\e", color = DimGray]
  194 -> 239 [label = "\\e", color = DimGray]
  196 -> 240 [label = "\\e", color = DimGray]
  196 -> 241 [label = "\\e", color = DimGray]
  196 -> 242 [label = "\\e", color = DimGray]
  197 -> 243 [label = "\\e", color = DimGray]
  197 -> 244 [label = "\\e", color = DimGray]
  198 -> 245 [label = "\\e", color = DimGray]
  199 -> 246 [label = "\\e", color = DimGray]
  200 -> 247 [label = "\\e", color = DimGray]
  200 -> 248 [label = "\\e", color = DimGray]
  202 -> 249 [label = "\\e", color = DimGray]
  202 -> 250 [label = "\\e", color = DimGray]
  202 -> 251 [label = "\\e", color = DimGray]
  207 -> 252 [label = "\\e", color = DimGray]
  207 -> 253 [label = "\\e", color = DimGray]
  207 -> 254 [label = "\\e", color = DimGray]
  208 -> 255 [label = "\\e", color = DimGray]
  208 -> 256 [label = "\\e", color = DimGray]
  209 -> 257 [label = "\\e", color = DimGray]
  209 -> 258 [label = "\\e", color = DimGray]
  211 -> 259 [label = "\\e", color = DimGray]
  211 -> 260 [label = "\\e", color = DimGray]
  211 -> 261 [label = "\\e", color = DimGray]
  215 -> 47 [label = "\\e", color = DimGray]
  215 -> 48 [label = "\\e"]
  216 -> 262 [label = "\\e", color = DimGray]
  217 -> 263 [label = "\\e", color = DimGray]
  218 -> 45 [label = "\\e", color = DimGray]
  218 -> 46 [label = "\\e", color = DimGray]
  220 -> 264 [label = "\\e", color = DimGray]
  220 -> 265 [label = "\\e", color = DimGray]
  220 -> 266 [label = "\\e", color = DimGray]
  224 -> 267 [label = "\\e", color = DimGray]
  225 -> 268 [label = "\\e", color = DimGray]
  225 -> 269 [label = "\\e", color = DimGray]
  225 -> 270 [label = "\\e", color = DimGray]
  226 -> 271 [label = "\\e", color = DimGray]
  226 -> 272 [label = "\\e", color = DimGray]
  228 -> 273 [label = "\\e", color = DimGray]
  228 -> 274 [label = "\\e", color = DimGray]
  228 -> 275 [label = "\\e", color = DimGray]
  229 -> 276 [label = "\\e", color = DimGray]
  230 -> 277 [label = "\\e"]
  230 -> 278 [label = "\\e", color = DimGray]
  230 -> 279 [label = "\\e", color = DimGray]
  231 -> 280 [label = "\\e", color = DimGray]
  231 -> 281 [label = "\\e", color = DimGray]
  233 -> 282 [label = "\\e", color = DimGray]
  233 -> 283 [label = "\\e", color = DimGray]
  233 -> 284 [label = "\\e", color = DimGray]
  234 -> 38 [label = "\\e", color = DimGray]
  234 -> 39 [label = "\\e", color = DimGray]
  235 -> 285 [label = "\\e", color = DimGray]
  236 -> 286 [label = "\\e", color = DimGray]
  237 -> 36 [label = "\\e", color = DimGray]
  237 -> 37 [label = "\\e", color = DimGray]
  239 -> 287 [label = "\\e", color = DimGray]
  239 -> 288 [label = "\\e", color = DimGray]
  239 -> 289 [label = "\\e", color = DimGray]
  244 -> 290 [label = "\\e", color = DimGray]
  244 -> 291 [label = "\\e", color = DimGray]
  244 -> 292 [label = "\\e", color = DimGray]
  245 -> 293 [label = "\\e", color = DimGray]
  245 -> 294 [label = "\\e", color = DimGray]
  246 -> 295 [label = "\\e", color = DimGray]
  246 -> 296 [label = "\\e", color = DimGray]
  248 -> 297 [label = "\\e", color = DimGray]
  248 -> 298 [label = "\\e", color = DimGray]
  248 -> 299 [label = "\\e", color = DimGray]
  256 -> 300 [label = "\\e", color = DimGray]
  256 -> 301 [label = "\\e", color = DimGray]
  256 -> 302 [label = "\\e", color = DimGray]
  258 -> 303 [label = "\\e", color = DimGray]
  258 -> 304 [label = "\\e", color = DimGray]
  258 -> 305 [label = "\\e", color = DimGray]
  262 -> 90 [label = "\\e", color = DimGray]
  262 -> 91 [label = "\\e", color = DimGray]
  263 -> 92 [label = "\\e", color = DimGray]
  263 -> 93 [label = "\\e", color = DimGray]
  267 -> 66 [label = "\\e", color = DimGray]
  267 -> 67 [label = "\\e", color = DimGray]
  268 -> 306 [label = "\\e", color = DimGray]
  269 -> 307 [label = "\\e", color = DimGray]
  270 -> 64 [label = "\\e", color = DimGray]
  270 -> 65 [label = "\\e", color = DimGray]
  272 -> 308 [label = "\\e", color = DimGray]
  272 -> 309 [label = "\\e", color = DimGray]
  272 -> 310 [label = "\\e", color = DimGray]
  276 -> 75 [label = "\\e", color = DimGray]
  276 -> 76 [label = "\\e", color = DimGray]
  277 -> 311 [label = "\\e"]
  278 -> 312 [label = "\\e", color = DimGray]
  279 -> 73 [label = "\\e", color = DimGray]
  279 -> 74 [label = "\\e", color = DimGray]
  281 -> 313 [label = "\\e", color = DimGray]
  281 -> 314 [label = "\\e", color = DimGray]
  281 -> 315 [label = "\\e", color = DimGray]
  285 -> 80 [label = "\\e", color = DimGray]
  285 -> 81 [label = "\\e", color = DimGray]
  286 -> 82 [label = "\\e", color = DimGray]
  286 -> 83 [label = "\\e", color = DimGray]
  294 -> 316 [label = "\\e", color = DimGray]
  294 -> 317 [label = "\\e", color = DimGray]
  294 -> 318 [label = "\\e", color = DimGray]
  296 -> 319 [label = "\\e", color = DimGray]
  296 -> 320 [label = "\\e", color = DimGray]
  296 -> 321 [label = "\\e", color = DimGray]
  306 -> 106 [label = "\\e", color = DimGray]
  306 -> 107 [label = "\\e", color = DimGray]
  307 -> 108 [label = "\\e", color = DimGray]
  307 -> 109 [label = "\\e", color = DimGray]
  311 -> 116 [label = "\\e"]
  311 -> 117 [label = "\\e", color = DimGray]
  312 -> 118 [label = "\\e", color = DimGray]
  312 -> 119 [label = "\\e", color = DimGray]
}''')
CHECK_EQ(res, lhs & rhs & third)
check_equivalent(res.proper(), vcsn.context("lal_char(b)_b").ratexp("b*").standard())
