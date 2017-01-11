import vcsn
import re

def sms_to_fr(sms, grap, synt):
    # Graphemic automaton expect input of the format '[#this#is#my#text#]'.
    sms = re.escape('[#' + sms.replace(' ', '#') + '#]')
    ctx = vcsn.context('lan_char, rmin')
    # Create the automaton corresponding to the sms.
    sms_aut = ctx.expression(sms).automaton().partial_identity().proper()
    # First composition with graphemic automaton.
    aut_g = sms_aut.compose(grap).coaccessible().strip()
    # Second composition with syntactic automaton.
    aut_s = aut_g.compose(synt).coaccessible().strip().project(1).proper()
    # Retrieve the path more likely to correspond to french translation.
    return str(aut_s.lightest())
