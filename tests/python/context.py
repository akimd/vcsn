#! /usr/bin/env python

import re

import vcsn
from test import *

def check(ctx, exp=None, format="sname"):
    c = vcsn.context(ctx)
    if exp is None:
        exp = ctx
    CHECK_EQ(exp, c.format(format))

# Invalid context: invalid weightset.
XFAIL(lambda: vcsn.context("lal_char(a)_UNKNOWN"))

# Invalid context: trailing garbage.
XFAIL(lambda: vcsn.context("lal_char(a), b_z"))

## --------------------- ##
## LabelSet: letterset.  ##
## --------------------- ##

# Different types of syntactic sugar.
for c in ['letterset<char_letters(abc)>, b',
          'lal<char(abc)>, b',
          'lal<char_letters(abc)>, b',
          'lal_char(abc), b',
          'lal(abc), b']:
    check(c, 'letterset<char_letters(abc)>, b')

# An open context is not printed as open in text.
for c in ['letterset<char_letters()>, b',
          'lal<char>, b',
          'lal<char_letters>, b',
          'lal_char, b',
          'lal, b']:
    check(c, 'letterset<char_letters()>, b')

# An open context is printed as open in LaTeX.
check('lal_char, b',      r'\{\ldots\}\to\mathbb{B}', 'latex')
check('lal_char, b',      r'{...} -> B',               'text')
check('lal_char, b',      r'{...} → 𝔹',               'utf8')
check('lal_char(abc), b', r'\{a, b, c\}\to\mathbb{B}', 'latex')
check('lal_char(abc), b', r'{abc} -> B',          'text')
check('lal_char(abc), b', r'{abc} → 𝔹',          'utf8')
check('lal_char(), b',    r'\{\}\to\mathbb{B}',  'latex')
check('lal_char(), b',    r'{} -> B',             'text')
check('lal_char(), b',    r'{} → 𝔹',             'utf8')

# letterset and different char_letters.
check(r'lal_char(), b',       r'letterset<char_letters()>, b')
check(r'lal_char(ab), b',     r'letterset<char_letters(ab)>, b')
check(r'lal_char(a-kv-z), b', r'letterset<char_letters(abcdefghijkvwxyz)>, b')
check(r'lal_char(-0-9), b',   r'letterset<char_letters(\-0123456789)>, b')
check(r'lal_char(<>[]{}), b', r'letterset<char_letters(<>[]{})>, b')
check(r'lal_char(\\\-\)), b', r'letterset<char_letters(\)\-\\)>, b')
check(r'lal_char(\--\-), b',  r'letterset<char_letters(\-)>, b')

# Check that we cover all the possible chars.  Unfortunately as this
# is not valid UTF-8, Python will refuse to display it.  This is a
# workaround, trying to find some means to display the full range.
CHECK_EQ(r'''\x01 + \x02 + \x03 + \x04 + \x05 + \x06 + \x07 + \x08 + \x09 + \n + \x0b + \x0c + \x0d + \x0e + \x0f + \x10 + \x11 + \x12 + \x13 + \x14 + \x15 + \x16 + \x17 + \x18 + \x19 + \x1a + \x1b + \x1c + \x1d + \x1e + \x1f +   + ! + " + # + $ + % + & + ' + ( + ) + * + + + , + - + . + / + 0 + 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9 + : + ; + < + = + > + ? + @ + A + B + C + D + E + F + G + H + I + J + K + L + M + N + O + P + Q + R + S + T + U + V + W + X + Y + Z + [ + \\ + ] + ^ + _ + ` + a + b + c + d + e + f + g + h + i + j + k + l + m + n + o + p + q + r + s + t + u + v + w + x + y + z + { + | + } + ~ + \x7f + \x80 + \x81 + \x82 + \x83 + \x84 + \x85 + \x86 + \x87 + \x88 + \x89 + \x8a + \x8b + \x8c + \x8d + \x8e + \x8f + \x90 + \x91 + \x92 + \x93 + \x94 + \x95 + \x96 + \x97 + \x98 + \x99 + \x9a + \x9b + \x9c + \x9d + \x9e + \x9f + \xa0 + \xa1 + \xa2 + \xa3 + \xa4 + \xa5 + \xa6 + \xa7 + \xa8 + \xa9 + \xaa + \xab + \xac + \xad + \xae + \xaf + \xb0 + \xb1 + \xb2 + \xb3 + \xb4 + \xb5 + \xb6 + \xb7 + \xb8 + \xb9 + \xba + \xbb + \xbc + \xbd + \xbe + \xbf + \xc0 + \xc1 + \xc2 + \xc3 + \xc4 + \xc5 + \xc6 + \xc7 + \xc8 + \xc9 + \xca + \xcb + \xcc + \xcd + \xce + \xcf + \xd0 + \xd1 + \xd2 + \xd3 + \xd4 + \xd5 + \xd6 + \xd7 + \xd8 + \xd9 + \xda + \xdb + \xdc + \xdd + \xde + \xdf + \xe0 + \xe1 + \xe2 + \xe3 + \xe4 + \xe5 + \xe6 + \xe7 + \xe8 + \xe9 + \xea + \xeb + \xec + \xed + \xee + \xef + \xf0 + \xf1 + \xf2 + \xf3 + \xf4 + \xf5 + \xf6 + \xf7 + \xf8 + \xf9 + \xfa + \xfb + \xfc + \xfd + \xfe''',
         vcsn
         .context(r'lal_char(\x01-\xFE), b')
         .expression('[^]')
         .shortest(len=1))

check('lal_char(ab), q', 'letterset<char_letters(ab)>, q')


## ------------------- ##
## LabelSet: wordset.  ##
## ------------------- ##

check('wordset<string_letters>, b', 'wordset<string_letters()>, b')


## ------------------------- ##
## LabelSet: expressionset.  ##
## ------------------------- ##

check('expressionset<lal_char(ab), b>, b', 'expressionset<letterset<char_letters(ab)>, b>, b')
# expressions weighted by expressions.
check('expressionset<letterset<char_letters(ab)>, expressionset<letterset<char_letters(xy)>, q>>, b')

## -------------------------- ##
## WeightSet: polynomialset.  ##
## -------------------------- ##

check('lal_char(ab), polynomialset<law_char(xy), q>',
      'letterset<char_letters(ab)>, polynomialset<wordset<char_letters(xy)>, q>')


## -------------------- ##
## LabelSet: tupleset.  ##
## -------------------- ##

check('lat<letterset<char_letters(a)>, letterset<char_letters(a)>>, b')
check('lat<lal_char(a),lal_char(a)>,b', 'lat<letterset<char_letters(a)>, letterset<char_letters(a)>>, b')
check('lal_char(ab), lat<b, z>', 'letterset<char_letters(ab)>, lat<b, z>')

# Tuple contexts.
c1 = vcsn.context('lal(abc), b')
c2 = vcsn.context('lan(xyz), q')
CHECK_EQ(vcsn.context('lat<lal(abc), lan(xyz)>, q'), c1 | c2)


# End of level bosses.
check('lat<lat<lal_char(a)>>, b',
      'lat<lat<letterset<char_letters(a)>>>, b')

ctx = '''lat<lal_char(ba),lan_char(vu), law_char(x-z)>,
         lat<expressionset<lat<lal_char(fe), lal_char(hg)>, q>, r, q>'''
check(ctx,
      'lat<letterset<char_letters(ab)>, nullableset<letterset<char_letters(uv)>>, wordset<char_letters(xyz)>>, lat<expressionset<lat<letterset<char_letters(ef)>, letterset<char_letters(gh)>>, q>, r, q>', 'sname')
check(ctx,
      '{ab} x {uv}? x {xyz}* -> RatE[{ef} x {gh} -> Q] x R x Q', 'text')
check(ctx,
      '{ab} × {uv}? × {xyz}* → RatE[{ef} × {gh} → ℚ] × ℝ × ℚ', 'utf8')


# Check that spaces are generously accepted.
ctx = '''
nullableset < lat < lal_char (ba) ,
                    lat < lan_char(vu) , law_char(x-z) >
               >
           >
,
lat < expressionset < nullableset <lat < lan < char (fe) > , lan_char (hg) > > ,
                      lat < r, q > > ,
      lat < b , q >
    >
'''
exp = 'nullableset<lat<letterset<char_letters(ab)>, lat<nullableset<letterset<char_letters(uv)>>, wordset<char_letters(xyz)>>>>, lat<expressionset<lat<nullableset<letterset<char_letters(ef)>>, nullableset<letterset<char_letters(gh)>>>, lat<r, q>>, lat<b, q>>'
check(ctx, exp)
ctx = re.sub(r'\s+', '', ctx)
check(ctx, exp)
