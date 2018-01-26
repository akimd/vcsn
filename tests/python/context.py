#! /usr/bin/env python

import re

import vcsn
from test import *

def check(ctx, exp=None, fmt="utf8"):
    c = vcsn.context(ctx)
    if exp is None:
        exp = ctx
    CHECK_EQ(exp, c.format(fmt))

# Invalid context: invalid weightset.
XFAIL(lambda: vcsn.context("lal(a)_UNKNOWN"),
      '''expected "->" here:
lal(a)_UNKNOWN
      ^_''')

# Invalid context: trailing garbage.
XFAIL(lambda: vcsn.context("lal(a), b_z"),
      '''expected end of input here:
lal(a), b_z
         ^_''')

# Invalid context: missing comma.
XFAIL(lambda: vcsn.context("lal"),
      '''expected "->" here:
lal
   ^_''')

## --------------------- ##
## LabelSet: letterset.  ##
## --------------------- ##

# Different types of syntactic sugar.
for c in ['lal<char(abc)>, b', 'lal(abc), b']:
    check(c, '[abc]? → 𝔹')

for c in ['lal<char>, b', 'lal, b']:
    check(c, '[...]? → 𝔹')

# An open context is printed as open in LaTeX.
check('lal, b',      r'\{\ldots\}^?\to\mathbb{B}', 'latex')
check('lal, b',      r'[...]? -> B',               'text')
check('lal, b',      r'[...]? → 𝔹',               'utf8')
check('lal(abc), b', r'\{a, b, c\}^?\to\mathbb{B}', 'latex')
check('lal(abc), b', r'[abc]? -> B',          'text')
check('lal(abc), b', r'[abc]? → 𝔹',          'utf8')
check('lal(), b',    r'\{\}^?\to\mathbb{B}',  'latex')
check('lal(), b',    r'[]? -> B',             'text')
check('lal(), b',    r'[]? → 𝔹',             'utf8')

# letterset and different char_letters.
check(r'lal(), b',       r'[]? → 𝔹')
check(r'lal(ab), b',     r'[ab]? → 𝔹')
check(r'lal(a-kv-z), b', r'[abcdefghijkvwxyz]? → 𝔹')
check(r'lal(-0-9), b',   r'[\-0123456789]? → 𝔹')
check(r'lal(<>[]{}), b', r'[\<\>\[\]{}]? → 𝔹')
check(r'lal(\\\-\)), b', r'[)\-\\]? → 𝔹')
check(r'lal(\--\-), b',  r'[\-]? → 𝔹')

# Check that we cover all the possible chars.  Unfortunately as this
# is not valid UTF-8, Python will refuse to display it.  Using
# `shortest` is a workaround to display the full range.
CHECK_EQ(r'''\x01 + \x02 + \x03 + \x04 + \x05 + \x06 + \x07 + \x08 + \x09 + \n + \x0b + \x0c + \x0d + \x0e + \x0f + \x10 + \x11 + \x12 + \x13 + \x14 + \x15 + \x16 + \x17 + \x18 + \x19 + \x1a + \x1b + \x1c + \x1d + \x1e + \x1f + \  + ! + " + # + $ + % + & + \' + ( + ) + * + + + \, + \- + . + / + 0 + 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9 + : + ; + \< + = + \> + ? + @ + A + B + C + D + E + F + G + H + I + J + K + L + M + N + O + P + Q + R + S + T + U + V + W + X + Y + Z + \[ + \\ + \] + ^ + _ + ` + a + b + c + d + e + f + g + h + i + j + k + l + m + n + o + p + q + r + s + t + u + v + w + x + y + z + { + \| + } + ~ + \x7f + \x80 + \x81 + \x82 + \x83 + \x84 + \x85 + \x86 + \x87 + \x88 + \x89 + \x8a + \x8b + \x8c + \x8d + \x8e + \x8f + \x90 + \x91 + \x92 + \x93 + \x94 + \x95 + \x96 + \x97 + \x98 + \x99 + \x9a + \x9b + \x9c + \x9d + \x9e + \x9f + \xa0 + \xa1 + \xa2 + \xa3 + \xa4 + \xa5 + \xa6 + \xa7 + \xa8 + \xa9 + \xaa + \xab + \xac + \xad + \xae + \xaf + \xb0 + \xb1 + \xb2 + \xb3 + \xb4 + \xb5 + \xb6 + \xb7 + \xb8 + \xb9 + \xba + \xbb + \xbc + \xbd + \xbe + \xbf + \xc0 + \xc1 + \xc2 + \xc3 + \xc4 + \xc5 + \xc6 + \xc7 + \xc8 + \xc9 + \xca + \xcb + \xcc + \xcd + \xce + \xcf + \xd0 + \xd1 + \xd2 + \xd3 + \xd4 + \xd5 + \xd6 + \xd7 + \xd8 + \xd9 + \xda + \xdb + \xdc + \xdd + \xde + \xdf + \xe0 + \xe1 + \xe2 + \xe3 + \xe4 + \xe5 + \xe6 + \xe7 + \xe8 + \xe9 + \xea + \xeb + \xec + \xed + \xee + \xef + \xf0 + \xf1 + \xf2 + \xf3 + \xf4 + \xf5 + \xf6 + \xf7 + \xf8 + \xf9 + \xfa + \xfb + \xfc + \xfd + \xfe''',
         vcsn
         .context(r'lal(\x01-\xFE), b')
         .expression('[^]')
         .shortest(len=1))

check('lal(ab), q', '[ab]? → ℚ')


## ------------------- ##
## LabelSet: wordset.  ##
## ------------------- ##

check('wordset<string_letters>, b', '[...]* → 𝔹')


## ------------------------- ##
## LabelSet: expressionset.  ##
## ------------------------- ##

check('expressionset<lal(ab), b>, b', 'RatE[[ab]? → 𝔹] → 𝔹')
# expressions weighted by expressions.
check('RatE[[ab]? → RatE[[xy]? → ℚ]] → 𝔹')

## -------------------------- ##
## WeightSet: polynomialset.  ##
## -------------------------- ##

check('lal(ab), polynomialset<law(xy), q>',
      '[ab]? → Poly[[xy]* → ℚ]')


## -------------------- ##
## LabelSet: tupleset.  ##
## -------------------- ##

check('[a]? × [a]? → 𝔹')
check('lat<lal(a),lal(a)>,b', '[a]? × [a]? → 𝔹')
check('lal(ab), lat<b, z>', '[ab]? → 𝔹 × ℤ')

# Tuple contexts.
c1 = vcsn.context('lal(abc), b')
c2 = vcsn.context('lal(xyz), q')
CHECK_EQ(vcsn.context('lat<lal(abc), lal(xyz)>, q'), c1 | c2)


## --------------------- ##
## End of level bosses.  ##
## --------------------- ##

# This one triggered a bug: shortest is not instantiable because we
# fail to support label * word in this context.
check('lat<lal>, b',
      '[...]? → 𝔹')

check('lat<lat<lal(a)>>, b',
      '([a]?) → 𝔹')

ctx = '''lat<lal(ba),lal(vu), law(x-z)>,
         lat<expressionset<lat<lal(fe), lal(hg)>, q>, r, q>'''
check(ctx,
      'lat<letterset<char_letters(ab)>, letterset<char_letters(uv)>, wordset<char_letters(xyz)>>, lat<expressionset<lat<letterset<char_letters(ef)>, letterset<char_letters(gh)>>, q>, r, q>', 'sname')
check(ctx,
      '[ab]? x [uv]? x [xyz]* -> RatE[[ef]? x [gh]? -> Q] x R x Q', 'text')
check(ctx,
      '[ab]? × [uv]? × [xyz]* → RatE[[ef]? × [gh]? → ℚ] × ℝ × ℚ', 'utf8')


# Check that spaces are generously accepted.
ctx = '''
lat < lal (ba) ,
      lat < lal(vu) , law(x-z) >
    >
,
lat < expressionset < lat < lal < char (fe) > , lal (hg) >  ,
                      lat < r, q > > ,
      lat < b , q >
    >
'''
exp = '[ab]? × ([uv]? × [xyz]*) → RatE[[ef]? × [gh]? → ℝ × ℚ] × (𝔹 × ℚ)'
check(ctx, exp)
ctx = re.sub(r'\s+', '', ctx)
check(ctx, exp)


## ---------------------------------- ##
## Check the comparison of contexts.  ##
## ---------------------------------- ##

def check(c1, c2):
    c1 = vcsn.context(c1)
    c2 = vcsn.context(c2)
    CHECK_EQ(True, c1 == c1)
    CHECK_EQ(True, c2 == c2)
    CHECK_EQ(False, c1 == c2)
    CHECK_EQ(False, c2 == c1)
    CHECK_EQ(False, c1 != c1)
    CHECK_EQ(False, c2 != c2)
    CHECK_EQ(True, c1 != c2)
    CHECK_EQ(True, c2 != c1)
check('lal(abc), b', 'lal(abcd), b')
check('lal(abc), b', 'law(abc), b')
check('lal(abc), b', 'lal(abc), q')
# Regression: at some point they were considered equal, because there
# were no difference when printed in UTF-8.  We now check the sname.
check('lat<lal(abc)>, b', 'lal(abc), b')
