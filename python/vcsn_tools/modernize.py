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

def context(ctx: str) -> str:
    '''Convert ctx to the modern syntax.'''
    def s(pattern, subst):
        nonlocal ctx
        ctx = re.sub(pattern, subst, ctx)
    # letterset<char_letters(01)>
    s(r'letterset<char_letters\((.*?)\)>',
      r'[\1]?')
    # lal(a), lal_char(a)
    s(r'lal(?:_char)?\((.*?)\)',
      r'[\1]?')
    # lal, letterset<char_letters>
    s('lal|letterset<char_letters>', '[...]?')

    # wordset<char_letters(01)>
    s(r'wordset<char_letters\((.*?)\)>',
      r'[\1]*')
    # law(a), law_char(a)
    s(r'law(?:_char)?\((.*?)\)',
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

def modernize(s: str) -> str:
    # Dot context in notebooks.
    s = re.sub(r'(?<=\bvcsn_context = \\")(.*)(?=\\"\\n)',
               lambda m: context(m.group(1)),
               s)
    # Dot contexts.
    s = re.sub(r'(?<=\bvcsn_context = ")(.*)(?=")',
               lambda m: context(m.group(1)),
               s)
    # Daut context: strip quotes.
    s = re.sub(r'(?<=\bcontext = )"(.*)"',
               lambda m: re.sub(r'\\(.)', r'\1', context(m.group(1))),
               s)
    # Daut context.
    s = re.sub(r'(?<=\bcontext = )(.*)$',
               lambda m: context(m.group(1)),
               s, flags=re.MULTILINE)
    return s

class Test(unittest.TestCase):
    def test_context(self):
        def check(i, o):
            self.assertEqual(o, context(i))
        check(r'lao, b', '{ε} → 𝔹')

    def test_modernize(self):
        def check(i, o):
            self.assertEqual(o, modernize(i))
        check(r'vcsn_context = \"lal, b\"\n',
              r'vcsn_context = \"[...]? → 𝔹\"\n')
        check(r'vcsn_context = \"lal(abc), b\"\n',
              r'vcsn_context = \"[abc]? → 𝔹\"\n')
        check(r'vcsn_context = "lal(abc), b"',
              r'vcsn_context = "[abc]? → 𝔹"')
        check(r'context = "lal(\"), b"',
              r'context = ["]? → 𝔹')
        check(r'context = "lat<letterset<char_letters(01)>, letterset<char_letters(01)>>, b"',
              r'context = [01]? × [01]? → 𝔹')


if __name__ == '__main__':
    unittest.main(testRunner=taprunner.TAPTestRunner(stream=sys.stdout)) # output='test-reports'
