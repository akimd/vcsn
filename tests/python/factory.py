#! /usr/bin/env python

# Check the factories (of automata, expressions, etc.)

import vcsn
from test import *

def get_labels(aut):
    'A set of the labels used in aut.'
    return set(re.findall('\w+ -> \w+ (.*)$', a.format('daut'), re.M))
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

## ----------- ##
## divkbaseb.  ##
## ----------- ##

b = vcsn.context('lal_char(0-9), b')

XFAIL(lambda: b.divkbaseb(0, 2))
XFAIL(lambda: b.divkbaseb(2, 0))
XFAIL(lambda: b.divkbaseb(2, 1))
XFAIL(lambda: b.divkbaseb(2, 11))

# FIXME: we don't parse polynomials yet.
CHECK_EQ(r'\e + 0 + 00 + 10 + 000 + 010 + 100 + 110 + 0000 + 0010',
         str(b.divkbaseb(2, 2).shortest(10)))
CHECK_EQ(r'\e + 0 + 00 + 10 + 20 + 30 + 40 + 50 + 60 + 70',
         str(b.divkbaseb(10, 10).shortest(10)))
CHECK_EQ(r'\e + 0 + 5 + 00 + 05 + 10 + 15 + 20 + 25 + 30',
         str(b.divkbaseb(5, 10).shortest(10)))
CHECK_EQ(r'\e + 0 + 3 + 6 + 9 + 00 + 03 + 06 + 09 + 12',
         str(b.divkbaseb(3, 10).shortest(10)))


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
c1 = vcsn.context('lal_char(a), b').random(4, 1, 4, 4)
c2 = vcsn.automaton(filename=medir + '/clique-a-4.gv')
CHECK_EQ(c1, c2)


# Expect the right number of states.
a = vcsn.context('lal_char(a), b').random(100, .1, 20, 30)
CHECK_EQ('mutable_automaton<letterset<char_letters(a)>, b>', a.info()['type'])
CHECK_EQ(100, a.info()['number of states'])
CHECK_EQ(20, a.info()['number of initial states'])
CHECK_EQ(30, a.info()['number of final states'])


# For a while, we were only able to get matching letters (a|A, b|B,
# etc.).
ctx = vcsn.context('lat<lan(a-z), lan(a-z)>, b')
a = ctx.random(num_states=10, density=1, max_labels=1)
# Get all the labels.
print("random: {:d}".format(a))
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
XFAIL(lambda: vcsn.context('lal(), b').random(2),
      "random: empty labelset: {}")


# Check that max_labels is honored.
ctx = vcsn.context('lal(a-z), b')
a = ctx.random(num_states=10, max_labels=5, density=1)
labels = a.labels()
print("random: {:d}".format(a))
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
CHECK_NE(info['complement'], 0)
CHECK_EQ(info['conjunction'], 0)
CHECK_EQ(info['infiltration'], 0)
CHECK_EQ(info['ldiv'], 0)
CHECK_EQ(info['lweight'], 0)
CHECK_EQ(info['one'], 0)
CHECK_EQ(info['prod'], 0)
CHECK_EQ(info['rweight'], 0)
CHECK_EQ(info['shuffle'], 0)
CHECK_NE(info['star'], 0)
CHECK_NE(info['sum'], 0)
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
