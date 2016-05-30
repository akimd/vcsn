#! /usr/bin/env python

# Check the factories (of automata, expressions, etc.)

import vcsn
from test import *

def get_labels(aut):
    'A set of the labels used in aut.'
    return set(re.findall(r'\w+ -> \w+ (.*)$', a.format('daut'), re.M))
vcsn.automaton.labels = get_labels

## ------- ##
## cerny.  ##
## ------- ##

a = vcsn.context('lal_char(abc), b').cerny(6)
CHECK_EQ(a.info()['number of states'], 6)
CHECK_EQ(vcsn.automaton(filename=medir + '/cerny-6.gv'), a)


## ----------- ##
## de_bruijn.  ##
## ----------- ##

CHECK_EQ(vcsn.automaton(filename=medir + '/de-bruijn-2.gv'),
         vcsn.context('lal_char(ab), b').de_bruijn(2))

CHECK_EQ(vcsn.automaton(filename=medir + '/de-bruijn-3.gv'),
         vcsn.context('lal_char(xyz), b').de_bruijn(3))

## ---------------- ##
## div/quotkbaseb.  ##
## ---------------- ##

b = vcsn.context('lal_char(0-9), b')

XFAIL(lambda: b.divkbaseb(0, 2), "divkbaseb: divisor cannot be 0")
XFAIL(lambda: b.divkbaseb(2, 0), "divkbaseb: base (0) must be at least 2")
XFAIL(lambda: b.divkbaseb(2, 1), "divkbaseb: base (1) must be at least 2")
XFAIL(lambda: b.divkbaseb(2, 11),
      "divkbaseb: base (11) must be less than " +
      "or equal to the alphabet size (10)")

b2 = vcsn.context('lat<lal_char(0-9), lal_char(0-2)>, b')

XFAIL(lambda: b2.quotkbaseb(0, 2), "quotkbaseb: divisor cannot be 0")
XFAIL(lambda: b2.quotkbaseb(2, 1), "quotkbaseb: base (1) must be at least 2")
XFAIL(lambda: b2.quotkbaseb(2, 4),
      "quotkbaseb: base (4) must be less than " +
      "or equal to the right alphabet size (3)")
XFAIL(lambda: b2.quotkbaseb(2, 11),
      "quotkbaseb: base (11) must be less than " +
      "or equal to the left alphabet size (10)")

b3 = vcsn.context('lat<lal_char(0-9), lal_char(0-9)>, b')

def check(k, base, exp):
    a = b3.quotkbaseb(k, base)
    CHECK_EQ(exp, a.shortest(10))
    CHECK_EQ(a.project(0), b.divkbaseb(k, base))

# FIXME: we don't parse polynomials yet.
check(2, 2,
      r'\e|\e + 0|0 + 00|00 + 10|01 + 000|000 + 010|001 + 100|010 + 110|011 + 0000|0000 + 0010|0001')
check(10, 10,
      r'\e|\e + 0|0 + 00|00 + 10|01 + 20|02 + 30|03 + 40|04 + 50|05 + 60|06 + 70|07')
check(5, 10,
      r'\e|\e + 0|0 + 5|1 + 00|00 + 05|01 + 10|02 + 15|03 + 20|04 + 25|05 + 30|06')
check(3, 10,
      r'\e|\e + 0|0 + 3|1 + 6|2 + 9|3 + 00|00 + 03|01 + 06|02 + 09|03 + 12|04')



## ------------- ##
## double_ring.  ##
## ------------- ##

ctx = vcsn.context('lal_char(abcd), b')
CHECK_EQ(ctx.double_ring(0, []),
         vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(abcd), b"
  rankdir = LR
}
'''))

CHECK_EQ(ctx.double_ring(1, [0]),
         vcsn.automaton(filename=medir + '/double-ring-1-0.gv'))

CHECK_EQ(ctx.double_ring(4, [2, 3]),
         vcsn.automaton(filename=medir + '/double-ring-4-2-3.gv'))


## ---------- ##
## ladybird.  ##
## ---------- ##

b = vcsn.context('lal_char(abc), b')
z = vcsn.context('lal_char(abc), z')

exp = vcsn.automaton(filename=medir + '/ladybird-2.gv')

CHECK_EQ(exp, b.ladybird(2))
CHECK_EQ(vcsn.automaton(str(exp).replace(', b', ', z')), z.ladybird(2))

exp = vcsn.automaton(filename=medir + '/ladybird-2-zmin.gv')
CHECK_EQ(exp,
         vcsn.context('lal_char(abc), zmin').ladybird(2))

## ------------ ##
## levenshein.  ##
## ------------ ##

nmin = vcsn.context('lat<lan_char(abc), lan_char(bcd)>, nmin')

exp = vcsn.automaton(filename=medir + '/levenshtein.gv')

CHECK_EQ(exp, nmin.levenshtein())

## -------- ##
## random.  ##
## -------- ##

# Expect a clique.
c1 = vcsn.context('lal_char(a), b').random_automaton(4, 1, 4, 4)
c2 = vcsn.automaton(filename=medir + '/clique-a-4.gv')
CHECK_EQ(c1, c2)


# Expect the right number of states.
a = vcsn.context('lal_char(a), b').random_automaton(100, .1, 20, 30)
CHECK_EQ('mutable_automaton<letterset<char_letters(a)>, b>', a.info()['type'])
CHECK_EQ(100, a.info()['number of states'])
CHECK_EQ(20, a.info()['number of initial states'])
CHECK_EQ(30, a.info()['number of final states'])


# For a while, we were only able to get matching letters (a|A, b|B,
# etc.).
ctx = vcsn.context('lat<lan(a-z), lan(a-z)>, b')
a = ctx.random_automaton(num_states=10, density=1, max_labels=1)
# Get all the labels.
print("random_automaton: {:d}".format(a))
labels = a.labels()
# Make sure there are \e|a and a|\e.
CHECK_NE([l for l in labels if re.match(r'\\e\|[a-z]', l)], [])
CHECK_NE([l for l in labels if re.match(r'[a-z]\|\\e', l)], [])
# Make sure there are a|b labels.
CHECK_NE([l for l in labels
          if (re.match(r'[a-z]\|[a-z]', l)
              and not re.match(r'([a-z])\|\1', l))],
         [])
# Make sure there are \e|\e labels.
CHECK(r'\e|\e' in labels)


# Random on an empty labelset doesn't work
XFAIL(lambda: vcsn.context('lal(), b').random_automaton(2),
      "random_automaton: empty labelset: {}")


# Check that max_labels is honored.
ctx = vcsn.context('lal(a-z), b')
a = ctx.random_automaton(num_states=10, max_labels=5, density=1)
labels = a.labels()
print("random_automaton: {:d}".format(a))
print("labels: {}".format(labels))
# Expect from 1 to 5 labels per entry.
for n in range(1, 7):
    if n == 1:
        pat = r'[a-z]'
    elif n == 2:
        pat = r'[a-z], [a-z]'
    else:
        pat = r'\[[a-z]{{{}}}\]'.format(n)
    ls = [l for l in labels if re.match(pat, l)]
    print("Number of labels:", n, ls)
    if n == 6:
        CHECK_EQ([], ls)
    else:
        CHECK_NE([], ls)

## ------------------- ##
## random_expression.  ##
## ------------------- ##

# Check that a random expression without any operator.
# return only a label
exp = vcsn.context('lan_char(a-z), b').random_expression()
print("Expression: ", exp)
CHECK(re.match(r'\w{1}|\\e', str(exp)))

# Check that operators are present only if the user has specified them.
exp = vcsn.context('lal_char(a), b')\
          .random_expression('+=1,*=0.5,{c}=1,{\\}=0,length=100',
                             identities='none')
print("Expression: ", exp)
info = exp.info()
print("Info: ", info)
CHECK_NE(info['add'], 0)
CHECK_NE(info['complement'], 0)
CHECK_NE(info['star'], 0)

CHECK_EQ(info['conjunction'], 0)
CHECK_EQ(info['infiltrate'], 0)
CHECK_EQ(info['ldiv'], 0)
CHECK_EQ(info['lweight'], 0)
CHECK_EQ(info['mul'], 0)
CHECK_EQ(info['one'], 0)
CHECK_EQ(info['rweight'], 0)
CHECK_EQ(info['shuffle'], 0)
CHECK_EQ(info['zero'], 0)

# Check the length of the expression.
exp = vcsn.context('lal_char(a), b')\
          .random_expression('+=1,length=15',
                             identities='none')
print("Expression: ", exp)
CHECK(str(exp).count('a') < 15)

## ---------------------- ##
## random_deterministic.  ##
## ---------------------- ##

a = vcsn.context('lal_char(a), b').random_deterministic(100)
CHECK_EQ('mutable_automaton<letterset<char_letters(a)>, b>', a.info()['type'])
CHECK_EQ(100, a.info()['number of states'])
CHECK_EQ(1, a.info()['number of initial states'])
CHECK_EQ(1, a.info()['number of final states'])
CHECK(a.is_complete())

## --- ##
## u.  ##
## --- ##

CHECK_EQ(vcsn.automaton(filename=medir + '/u-5.gv'),
         vcsn.context('lal_char(abc), b').u(5))
