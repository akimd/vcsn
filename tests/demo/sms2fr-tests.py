import vcsn
import re
import argparse
from sms2fr import sms_to_fr
import sys

# Handle arguments.
def create_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('-s', '--syntactic', type=str, nargs=1, required=True)
    parser.add_argument('-g', '--graphemic', type=str, nargs=1, required=True)
    return parser.parse_args()

args = create_args()

# Read the graphemic automaton.
grap = vcsn.automaton(filename=args.graphemic[0])
# Read the syntactic automaton.
synt = vcsn.automaton(filename=args.syntactic[0]).partial_identity()


for line in sys.stdin:
    trad = sms_to_fr(line.replace('\n', ''), grap, synt)
    eff = re.match('<(.*)>(.*)', trad).group(2)
    print(eff[3:-3].replace('#', ' '))
