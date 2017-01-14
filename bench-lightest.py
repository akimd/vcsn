import vcsn
from timeit import timeit
from subprocess import Popen, PIPE
import os
import sys
import matplotlib.pyplot as plt
import re


def datafile(f):
    return '{datadir}/sms2fr/{file}.efsm'.format(
        datadir=vcsn.config('configuration.datadir'),
        file=f)

grap = vcsn.automaton(filename=datafile('graphemic'))
synt = vcsn.automaton(filename=datafile('syntactic')).partial_identity()

def sms_to_fr(sms):
    sms = re.escape('[#' + sms.replace(' ', '#') + '#]')
    ctx = vcsn.context('lan_char, rmin')
    sms_aut = ctx.expression(sms).automaton().partial_identity().proper()
    aut_g = sms_aut.compose(grap).coaccessible().strip()
    aut_s = aut_g.compose(synt).coaccessible().strip().project(1).proper()
    return aut_s

class Toolbar:
    def __init__(self, nmax):
        self.toolbar_width = 100
        self.nb = 0
        self.nmax = nmax
        sys.stdout.write("[{}]".format(" " * self.toolbar_width))
        sys.stdout.flush()
        sys.stdout.write("\b" * (self.toolbar_width + 1))

    def update(self, n):
        ratio = int(float(n) / self.nmax * self.toolbar_width)
        if self.nb <= ratio:
            sys.stdout.write("#" * (ratio - self.nb))
            self.nb += ratio - self.nb
            sys.stdout.flush()
            if self.nb >= self.toolbar_width:
                sys.stdout.write("]\n")


os.environ['VCSN_CXXFLAGS']='-DNDEBUG -Ofast -std=c++14'
repetitions = 3

def time_vcsn(algo, aut, k):
    return timeit('aut.lightest({}, "{}")'.format(k, algo), number=repetitions, globals=locals())

def time_fst(aut, k):
    with open("test.fst", "wb") as fstaut:
        p1 = Popen(['efstcompile'], stdin=PIPE, stdout=fstaut, stderr=PIPE)
        p1.stdin.write(aut.format('efsm').encode('utf-8'))
        p1.stdin.close()
        p1.wait()
    test = '''
p2 = Popen(["fstshortestpath", "--nshortest={}", "--queue_type=shortest", "test.fst", "out.fst"], stdin=PIPE, stdout=PIPE, stderr=PIPE)
p2.wait()
    '''.format(k)
    return timeit(test, number=repetitions, setup="from subprocess import Popen, PIPE")


algos = {
    "breadth-first" : lambda aut, k: time_vcsn("breadth-first", aut, k),
    "yen"           : lambda aut, k: time_vcsn("yen", aut, k),
    "eppstein"      : lambda aut, k: time_vcsn("eppstein", aut, k),
    # "fst"           : lambda aut, k: time_fst(aut, k),
}

range_k = range(1, 100, 5)
range_size = range(1, 20, 1)

ctx = vcsn.context("lal_char(abc), nmin")

data = {
    "fixed"  : lambda n: ctx.expression("(<1>a + <2>b)?{{{}}}".format(n * 5)).standard(),
    "random" : lambda n: ctx.random_automaton(n * 10, max_labels=1, density=1, loop_chance=0),
    "sms2fr" : lambda n: sms_to_fr('slt ' * n * 5)
}

res = {}
for m_name, method in data.items():
    tmp = method(1)
    for a_name, algo in algos.items():
        time = algo(tmp, 1)
    print("> Creating Benches for " + m_name + ".")
    res[m_name] = {
        'scores' : { 'increasing k' : {}, 'increasing size' : {} },
        'infos' : {}
    }

    print("> Initializing structures.")
    for a_name, algo in algos.items():
        res[m_name]['scores']['increasing size'][a_name] = []
        res[m_name]['scores']['increasing k'][a_name] = []

    print("> Timing.")
    size_max = range_size[-1]
    k_max = range_k[-1]

    tb = Toolbar(len(range_size) + len(range_k))
    nb_iter = 1
    for n in range_size:
        aut = method(n)
        res[m_name]['infos'][n] = aut.info()
        for a_name, algo in algos.items():
            time = algo(aut, k_max)
            res[m_name]['scores']['increasing size'][a_name].append(time)
        tb.update(nb_iter)
        nb_iter += 1

    aut = method(size_max)
    for k in range_k:
        for a_name, algo in algos.items():
            time = algo(aut, k)
            res[m_name]['scores']['increasing k'][a_name].append(time)
        tb.update(nb_iter)
        nb_iter += 1

print("> Plotting.")
def plot_one(name, scores, sizes, xlabel):
    possible_colors = ['b', 'g', 'r', 'c', 'm', 'y', 'k', 'w']
    index = 0
    for a_name, algo in algos.items():
        plt.plot(sizes, scores[a_name], possible_colors[index], label=a_name)
        index += 1

    plt.title(name)
    plt.legend(bbox_to_anchor=(0.4, 1), loc=1, borderaxespad=1.)
    plt.yscale('log')
    plt.ylabel('time (in s)')
    plt.xlabel(xlabel)
    plt.savefig(name.replace(' ', '-'))
    plt.close()

def plot_scores(name, scores, infos):
    sizes = [info['number of transitions'] for info in infos.values()]
    plot_one(name + ' increasing k', scores['increasing k'], range_k, 'Value of k')
    plot_one(name + ' increasing size', scores['increasing size'], sizes, 'Automaton size (in number of transitions)')

for m_name, method in data.items():
    plot_scores(m_name, res[m_name]['scores'], res[m_name]['infos'])
