import vcsn
import re
import argparse
from smstofr import sms_to_fr

# Handle arguments.
def create_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('-s', '--syntactic', type=str, nargs=1, required=True)
    parser.add_argument('-g', '--graphemic', type=str, nargs=1, required=True)
    return parser.parse_args()

args = create_args()

# Read the graphemic automaton.
grap = vcsn.automaton(filename=args.graphemic[0], format='efsm')
# Read the syntactic automaton.
synt = vcsn.automaton(filename=args.syntactic[0], format='efsm').partial_identity()


def check(sms, res):
    trad = sms_to_fr(sms, grap, synt)
    eff = re.match('<(.*)>(.*)', trad).group(2)
    exp = '[#' + res.replace(' ', '#') + '#]'
    if eff == exp:
        print('PASS: {sms} -> {res}'.format(sms=sms, res=res))
    else:
        print('FAIL: ', sms)
        print('Expected output: ', exp)
        print('Effective output: ', eff)


check('slt', 'salut')
check('bjr', 'bonjour')
check('stp', 's\' il te plait')
check('cmt c tro bi1', 'comment c\' est trop bien')
check('difissil', 'difficile')
check('jpeu pa', 'je peux pas')
