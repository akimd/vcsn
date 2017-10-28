#! /usr/bin/env python

import subprocess
import vcsn
from test import *

from vcsn.automaton import _automaton_fst, _automaton_fst_files

# Do we have OpenFST installed?
# Calling 'fstprint --help' is tempting, but it exits 1.
have_ofst = which('fstprint') is not None


def check(aut, fefsm):
    'Check the conversion to and from FSM.'
    print(here() + ": check:", fefsm)

    # The reference: text in efsm format.
    efsm = open(medir + "/" + fefsm).read().strip()

    # Check output to EFSM.
    CHECK_EQ(efsm, aut.format('efsm'))

    # Check print | read | print to EFSM.
    #
    # We used to check that the output is exactly what we get when
    # reading back and printing again.  This does not work for automata
    # with several initial states or a non-one initial weight (which is
    # approximated below as !is_standard), as then we show pre as the
    # real initial state.  The pre state is displayed as the state
    # number immediately after the highest state number, and when read
    # and printed back by OpenFST, it is renumbered as 0.
    #
    # So (read | print) is not the identity.
    aut2 = vcsn.automaton(efsm, 'efsm')
    if aut.is_standard():
        print(here(), 'case standard')
        CHECK_EQ(aut, aut2)
        CHECK_EQ(efsm, aut2.format('efsm'))
    else:
        print(here(), 'case non standard')
        CHECK_EQUIV(aut, normalize(aut2))

    # Check that OpenFST accepts and reproduces our EFSM files.
    if have_ofst:
        if aut.is_standard():
            print(here(), 'fstcat: eq')
            CHECK_EQ(aut, aut.fstcat())
        else:
            print(here(), 'fstcat: equiv')
            CHECK_EQUIV(aut, normalize(aut.fstcat()))
    else:
        SKIP('OpenFST is missing')

a = load('lal_char_b/a1.gv')
check(a, 'a1.efsm')

a = load('lal_char_zmin/slowgrow.gv')
check(a, 'slowgrow.efsm')

for f in ["lal-char-zmin", "lat-zmin", "ascii-to-one"]:
    check(meaut(f, 'gv'), f + '.efsm')

# Check the case of an automaton without any transition.
a = vcsn.context('lal_char(), b').expression(r'\e').standard()
check(a, 'one.efsm')

# Check the support of spontaneous transitions.
# Note that "sort" is critical here, otherwise the transitions
# are not issued in the state-order, so when we read back, the
# states numbers are different.
a = vcsn.context('lal_char(ab), b').expression('ab*').thompson().sort().strip()
check(a, 'abs.efsm')

# Don't try law<char>, it does not make any sense for OpenFST.
a = vcsn.context('lal<string>, b')\
    .expression("'grand'*('child'+'parent')").thompson().sort().strip()
check(a, 'str.efsm')

# A transducer that looks like an acceptor when looking at the symbol
# numbers.
a = vcsn.context('lat<lal_char(abc),lal_char(xyz)>, b')\
    .expression("(a|x+b|y+c|z)*").standard().sort().strip()
check(a, 'a2x.efsm')

# A transducer that cannot be seen as an acceptor.
a = vcsn.context('lat<lal_char(a),lal_char(xyz)>, b')\
    .expression("(a|x+a|y+a|z)*").standard().sort().strip()
check(a, 'a2xyz.efsm')

# A transducer that mixes char and string.
a = vcsn.context('lat<lal<char>,lal<string>>, b')\
    .expression("(c|'child'+p|'parent')*").standard().sort().strip()
check(a, 'char-string.efsm')


def compose(l, r):
    print("Compose:", l, r)
    # We need to enforce the letters, otherwise fstcompose complains
    # about incompatible symbol tables.
    ctx = vcsn.context('lat<lal(amxy), lal(amxy)>, zmin')
    l = ctx.expression(l).automaton()
    r = ctx.expression(r).automaton()
    c_vcsn = l.compose(r).strip()
    c_ofst = l.fstcompose(r)
    # We get a narrower context, restore the original one so that
    # CHECK_EQ does not fail because of context differences.
    c_ofst = c_ofst.automaton(ctx)
    return (c_vcsn, c_ofst)

def check_proper(aut):
    pfst = aut.fstproper().sort().strip()
    p = aut.proper().sort().strip()
    CHECK_EQUIV(p, pfst)

if have_ofst:
    # Conjunction: check that OpenFST and Vcsn understand the weights
    # the same way.  We have zmin and log in common.
    for f in [vcsn.datadir + '/lal_char_zmin/minblocka.gv',
              vcsn.datadir + '/lal_char_zmin/slowgrow.gv',
              medir + '/lal-char-log.gv']:
        print("Conjunction:", f)
        #
        # a & 2 by Vcsn.
        a = vcsn.automaton(filename=f)
        a2_vcsn = a & 2

        # c1 & c1 by OpenFST.
        a2_ofst = a.fstconjunction(a)

        CHECK_EQ(a2_vcsn, a2_ofst)

    # Make sure determinizations agree.  This automaton, determinized,
    # has weights on the final states only, which exercises a bug we
    # once had.
    print("Determinize")
    zmin = vcsn.context('lal_char(ab), zmin')
    a = zmin.expression('[ab]*a(<2>[ab])').automaton()
    d_vcsn = a.determinize().strip()
    d_ofst = a.fstdeterminize()
    CHECK_EQ(d_vcsn, d_ofst)

    # Make sure compositions agree, even when there are empty words on
    # some tapes.
    c_vcsn, c_ofst = compose('<2>a|m', '<3>m|x')
    CHECK_EQ(c_vcsn, c_ofst)

    c_vcsn, c_ofst = compose(r'<2>a|\e', r'<3>\e|x')
    CHECK_EQ(c_vcsn, c_ofst)

    # In this case, we get two different automata: OpenFST's result is
    # trim, and has one less state: we return a tree, they manage to
    # keep a single final state.
    c_vcsn, c_ofst = compose(r'<2>a|\e + <3>a|m', r'<4>\e|x + <3>m|y')
    CHECK_EQUIV(c_vcsn, c_ofst)

    # Check our proper against OpenFST
    import glob
    files = [f for f in glob.glob(os.path.join(medir, '*.efsm'))
             if not os.path.basename(f).startswith('bad_')]
    for fn in files:
        check_proper(vcsn.automaton(filename=fn))

else:
    SKIP('OpenFST is missing')

# Invalid final weight.
XFAIL(lambda: meaut('bad_final_weight', 'efsm'),
'''Zmin: unexpected trailing characters: "df"
  while reading: "7df"
  while setting final state: <7df>0
  while reading automaton: bad_final_weight.efsm''')
# Invalid transition weight.
XFAIL(lambda: meaut('bad_weight', 'efsm'),
'''Zmin: unexpected trailing characters: "er"
  while reading: "4er"
  while adding transition: (0, <4er>a, 1)
  while reading automaton: bad_weight.efsm''')
