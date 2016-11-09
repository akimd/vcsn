import sys

if sys.version_info.major < 3:
    raise ValueError("Vcsn requires Python 3 or better: ", sys.version_info)
