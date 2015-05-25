import os

def env(key, default):
    return os.environ[key] if key in os.environ else default

config = {
    'version': '@PACKAGE_VERSION@',

    'ccache':   env('VCSN_CCACHE', r'''@CCACHE@'''),
    'cppflags': env('VCSN_CPPFLAGS', r'''@CPPFLAGS@'''),
    'cxx':      env('VCSN_CXX', r'''@CXX_NO_CCACHE@'''),
    'cxxflags': env('VCSN_CXXFLAGS', r'''@CXXFLAGS@'''),
    'ldflags':  env('VCSN_LDFLAGS', r'''@LDFLAGS@'''),

    # Where the automaton library is stored.
    'datadir':  env('VCSN_DATA_PATH', r'''@DATADIR@'''),
}
