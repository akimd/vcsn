import os

def env(key, default):
    res = os.environ[key] if key in os.environ else default
    res = res.strip()
    return res

def integer(key):
    if key in os.environ:
        try:
            val = int(os.environ[key])
        except ValueError:
            val = 1
    else:
        val = 0
    return val

config = {
    'version': '@PACKAGE_VERSION@',

    'python':   env('VCSN_PYTHON',   r'''@PYTHON@'''),

    'ccache':   env('VCSN_CCACHE',   r'''@CCACHE@'''),
    'cppflags': env('VCSN_CPPFLAGS', r'''@VCSN_CPPFLAGS@'''),
    'cxx':      env('VCSN_CXX',      r'''@VCSN_CXX@'''),
    'cxxflags': env('VCSN_CXXFLAGS', r'''@CXXFLAGS@'''),
    'ldflags':  env('VCSN_LDFLAGS',  r'''@VCSN_LDFLAGS@'''),

    # Where out files are installed.
    'datadir':    env('VCSN_DATA_PATH',  r'''@DATADIR@'''),
    'includedir': env('VCSN_INCLUDEDIR', r'''@INCLUDEDIR@'''),
    'libdir':     env('VCSN_LIBDIR',     r'''@LIBDIR@'''),
    'pyexecdir':  env('VCSN_PYEXECDIR',  r'''@PYEXECDIR@'''),

    # Various flags.
    'verbose':  integer('VCSN_VERBOSE'),
}
