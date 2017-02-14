## --------- ##
## context.  ##
## --------- ##

# Known pylint issue: https://github.com/PyCQA/pylint/issues/848
# Reported pylint issue: https://github.com/PyCQA/pylint/issues/931
# Last seen in pylint 1.6.4, astroid 1.4.9
# FIXME: Remove comment once issues are fixed.
# pylint: disable=unused-import,undefined-variable
from vcsn_cxx import context, label, polynomial, expression, weight
from vcsn.tools import _extend, _format


@_extend(context)
class context:
    __eq__ = lambda l, r: l.format('sname') == r.format('sname')
    __or__ = lambda l, r: context._tuple([l, r])
    __plus__ = context.join
    __repr__ = lambda self: self.format('text')
    _repr_latex_ = lambda self: '$' + self.format('latex') + '$'

    def __format__(self, spec):
        """Format the context according to `spec`.

        Parameters
        ----------
        spec : str, optional
            a list of letters that specify how the context
            should be formatted.

        Supported specifiers
        --------------------

        - 's': use the `sname` format, used to generate code
        - 't': use text ASCII syntax (default)
        - 'u': use text UTF-8 syntax
        - 'x': use LaTeX syntax

        - ':spec': pass the remaining specification to the
                   formating function for strings.

        """

        syntaxes = {'s': 'sname',
                    't': 'text',
                    'u': 'utf8',
                    'x': 'latex',
                    }
        return _format(self, spec, 'text', syntaxes)

    expression = lambda *a, **kw: expression(*a, **kw)
    label = lambda *a, **kw: label(*a, **kw)
    polynomial = lambda *a, **kw: polynomial(*a, **kw)
    weight = lambda *a, **kw: weight(*a, **kw)

    # Add support for `length` as a kwarg.
    _random_expression_orig = context.random_expression
    def random_expression(self, parameters='', *args, **kwargs):
        if 'length' in kwargs:
            parameters = '{}{}length={}'.format(parameters,
                                                ', ' if parameters else '',
                                                kwargs['length'])
            del kwargs['length']
        return self._random_expression_orig(parameters, *args, **kwargs)
