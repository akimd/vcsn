import re

from IPython.core.magic import (Magics, magics_class, line_magic)
from IPython.core.magic_arguments import (
    argument, magic_arguments, parse_argstring)
try:
    import ipywidgets as widgets
except ImportError:
    from IPython.html import widgets
from IPython.display import display
from pylab import *
import matplotlib.pyplot as plt
import pandas as pd

import vcsn

# some parameters
pd.set_option('display.mpl_style', 'default')
pd.set_option('display.max_rows', 500)
pd.set_option('display.max_columns', 500)


@magics_class
class Benchmarks(Magics):

    @magic_arguments()
    @argument('file', type=str, help='the name of the csv file.')
    @argument('regex', type=re.compile, nargs='?',
              help='''regex expression to match some algos''')
    @argument('mode', type=str, nargs='?', default='line',
              help='''the name of the visual mode to display the
              graph. Default: line''')
    @line_magic
    def plot(self, line):
        def _display(algo, several):
            group = df[algo]
            ax = group.plot(kind=args.mode)
            if several:
                ax.legend(loc='center left', bbox_to_anchor=(1, 0.5))
            else:
                plt.title(algo)
            ax = plt.figure()
        args = parse_argstring(self.plot, line)
        df = pd.read_csv(args.file)
        df.index = df[' ']
        df = df.drop(' ', 1)
        if args.regex:
            algos = []
            for ag in list(df.columns.values):
                if re.search(args.regex, ag):
                    algos.append(ag)
            _display(algos, True)
        else:
            algos = list(df.columns.values)
            widget.interact(lambda algo: _display(algo, False), algo=algos)

ip = get_ipython()
ip.register_magics(Benchmarks)
