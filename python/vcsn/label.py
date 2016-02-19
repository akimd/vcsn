## ------- ##
## label.  ##
## ------- ##

from vcsn_cxx import label
from vcsn.tools import _extend, _is_equal

@_extend(label)
class label:
    __eq__ = _is_equal
    __mul__ = label.multiply
    __repr__ = lambda self: self.format('text')
    _repr_latex_ = lambda self: '$' + self.format('latex') + '$'
