import os

if 'abs_builddir' in os.environ:
    __path__.append(os.environ['abs_builddir']+'/python/vcsn_tools')
