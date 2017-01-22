import vcsn
import re
import argparse
from sms2fr import sms_to_fr
import sys

def datafile(f):
    return '{datadir}/sms2fr/{file}.efsm'.format(
        datadir=vcsn.config('configuration.datadir'),
        file=f)

# Handle arguments.
def create_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('-s', '--syntactic', type=str,
                        default=datafile('syntactic'))
    parser.add_argument('-g', '--graphemic', type=str,
                        default=datafile('graphemic'))
    return parser.parse_args()

args = create_args()

# Read the graphemic automaton.
grap = vcsn.automaton(filename=args.graphemic)
# Read the syntactic automaton.
synt = vcsn.automaton(filename=args.syntactic).partial_identity()

for line in sys.stdin:
    trad = sms_to_fr(line.replace('\n', ''), grap, synt)
    eff = re.match('<(.*)>(.*)', trad).group(2)
    print(eff[3:-3].replace('#', ' '))
