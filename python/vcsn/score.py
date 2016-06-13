import re
import sys

try:
    import ipywidgets as widgets
except ImportError:
    from IPython.html import widgets # pylint: disable=no-name-in-module
from IPython.core.magic import (Magics, magics_class, line_magic)
from IPython.core.magic_arguments import (
    argument, magic_arguments, parse_argstring)

# pylint: disable=import-error
import matplotlib.pyplot as plt
import pandas as pd


# some parameters
plt.style.use('default')
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
        # if we are not running in a notebook
        if 'connection_file' not in ip.config['IPKernelApp']:
            print('This functionnality is only available in notebooks.', file=sys.stderr)
            return
        def _display(algo, several):
            group = df[algo]
            ax = group.plot(kind=args.mode)
            if several:
                ax.legend(loc='center left', bbox_to_anchor=(1, 0.5))
            else:
                plt.title(algo)
        args = parse_argstring(self.plot, line)
        df = pd.read_csv(args.file,
                         # Consider 'N/A' etc. as NaN
                         na_values=['FAIL', 'N/A'],
                         skipinitialspace=True)
        # Compound titles
        def format(x):
            if x > 1:
                return ', ' + str(x) + 'x'
            return ''
        df.index = df['Command'] + ' # ' + df['Setup'] + df['Repetitions'].map(format)
        # Drop now useless columns
        df = df.drop(['Command','Setup','Repetitions'], 1)
        # Swap axes to get back to old score-compare format
        df = df.T
        if args.regex:
            algos = []
            for ag in list(df.columns.values):
                if re.search(args.regex, ag):
                    algos.append(ag)
            _display(algos, True)
        else:
            algos = list(df.columns.values)
            widgets.interact(lambda algo: _display(algo, False), algo=algos)

ip = get_ipython()
ip.register_magics(Benchmarks)

# if we are running in a notebook
if 'connection_file' in ip.config['IPKernelApp']:
    ip.magic("matplotlib inline")
