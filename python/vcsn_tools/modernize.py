import sys
import unittest
import vcsn
import vcsn_tools.taprunner as taprunner

try:
    import regex as re
    has_regex = True
except ImportError:
    import warnings
    warnings.warn('you should install regex for Python')
    has_regex = False
    import re


def log(s):
    print(s, file=sys.stderr)


def labelset(ls: str) -> str:
    return ls

weightsets = {
    "b": "𝔹",
    "f2": "𝔽₂",
    "log": "Log",
    "nmin": "ℕmin",
    "q": "ℚ",
    "qmp": "ℚmp",
    "r": "ℝ",
    "rmin": "ℝmin",
    "z": "ℤ",
    "zmin": "ℤmin",
}


def weightset(ws: str) -> str:
    return weightsets.get(ws.lower(), ws)


def context_regex(ctx: str) -> str:
    '''Convert ctx to the modern syntax.'''
    def s(pattern, subst):
        nonlocal ctx
        ctx = re.sub(pattern, subst, ctx)
    # letterset<char_letters(01)>
    s(r'letterset<char_letters\(((\\.|[^\\)])*)\)>',
      r'[\1]?')
    # lal(a), lal_char(a)
    s(r'lal(?:_char)?\(((\\.|[^)])*)\)',
      r'[\1]?')
    # lal, letterset<char_letters>
    s('lal|letterset<char_letters>', '[...]?')

    # wordset<char_letters(01)>
    s(r'wordset<char_letters\(((\\.|[^)])*)\)>',
      r'[\1]*')
    # law(a), law_char(a)
    s(r'law(?:_char)?\(((\\.|[^)])*)\)',
      r'[\1]*')
    # law, wordset<char_letters>
    s('law|wordset<char_letters>', '[...]*')

    # lao
    s('oneset|lao', '{ε}')

    # lat<...>
    s(r'lat<(.*?)>',
      lambda m: ' × '.join([x.strip()
                            for x in m.group(1).split(',')]))
    s(r'^(.*), *(.*)$',
      lambda m: '{} → {}'.format(m.group(1),
                                 weightset(m.group(2))))
    return ctx


def context_parse(ctx: str) -> str:
    log("parse: {} -> ...".format(ctx))
    return vcsn.context(ctx).format('utf8')


def strip_escapes(s: str) -> str:
    return re.sub(r'\\(.)', r'\1', s)


def context(ctx: str, safe=False) -> str:
    log("{} -> ...".format(ctx))
    syntax = 'raw'
    if ctx.startswith('"'):
        syntax = 'quoted'
        ctx = ctx[1:-1]
        ctx = strip_escapes(ctx)
    if safe:
        res = context_parse(ctx)
    else:
        res = context_regex(ctx)
    if syntax == 'quoted':
        res = '"{}"'.format(re.sub(r'(\\|")', r'\\\1', res))
    log("... -> {}".format(res))
    return res



def modernize(s: str, safe=False) -> str:
    # Dot context in notebooks.
    ctx = lambda s: context(s, safe)
    s = re.sub(r'(?<=\bvcsn_context = \\")((\\.|[^"])*)(?=\\"\\n)',
               lambda m: ctx(m.group(1)),
               s)
    # Dot contexts.
    s = re.sub(r'(?<=\bvcsn_context = )("(\\.|[^"])*")',
               lambda m: ctx(m.group(1)),
               s)
    # Daut context in double quotes.
    s = re.sub(r'(?<=\bcontext = )"((\\.|[^"])*)"',
               lambda m: re.sub(r'\\(.)', r'\1', ctx(strip_escapes(m.group(1)))),
               s)
    # Daut context.
    s = re.sub(r'(?<=\bcontext = )(.*)$',
               lambda m: ctx(m.group(1)),
               s, flags=re.MULTILINE)
    return s


# pylint: disable=line-too-long
class Test(unittest.TestCase):

    def test_context(self):
        def check(i, o):
            self.assertEqual(o, context(i))
        check(r'lao, b', '{ε} → 𝔹')
        check(r'lal(a-z), b', '[a-z]? → 𝔹')
        check(r'lal(\(\)), b', r'[\(\)]? → 𝔹')

    def test_modernize(self):
        def check(i, o, safe=False):
            self.assertEqual(o, modernize(i, safe))
        check(r'vcsn_context = \"lal, b\"\n',
              r'vcsn_context = \"[...]? → 𝔹\"\n')
        check(r'vcsn_context = \"lal(abc), b\"\n',
              r'vcsn_context = \"[abc]? → 𝔹\"\n')
        check(r'vcsn_context = "lal(abc), b"',
              r'vcsn_context = "[abc]? → 𝔹"')
        check(r'''vcsn_context = "lat<letterset<char_letters(!\"#$%&\\'\\(\\)*+\,-./:;\\<=\\>?@\\[\\\\\\]^_`{\\|}~)>, letterset<char_letters()>>, b"''',
              r'''vcsn_context = "[!\"#$%&\\'()*+\\,\\-./:;\\<=\\>?@\\[\\\\\\]^_`{\\|}~]? × []? → 𝔹"''',
              safe=True)
        check(r'context = "lal(\"), b"',
              r'context = ["]? → 𝔹')
        check(r'context = "lat<letterset<char_letters(01)>, letterset<char_letters(01)>>, b"',
              r'context = [01]? × [01]? → 𝔹')


if __name__ == '__main__':
    unittest.main(testRunner=taprunner.TAPTestRunner(stream=sys.stdout))
