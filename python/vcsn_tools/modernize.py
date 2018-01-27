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

def modernize(s: str) -> str:
    # Dot context in notebooks.
    s = re.sub(r'(?<=\bvcsn_context = \\")(.*)(?=\\"\\n)',
               lambda m: vcsn.context(m.group(1)).format('utf8'),
               s)
    # Daut context: strip quotes.
    s = re.sub(r'(?<=\bcontext = )"(.*)"',
               lambda m: re.sub(r'\\(.)', r'\1', m.group(1)),
               s)
    # Daut context.
    s = re.sub(r'(?<=\bcontext = )(.*)$',
               lambda m: vcsn.context(m.group(1)).format('utf8'),
               s, flags=re.MULTILINE)
    return s

class Test(unittest.TestCase):
    def check(self, i, o):
        self.assertEqual(o, modernize(i))

    def test_notebook(self):
        self.check(r'vcsn_context = \"lal, b\"\n',
                   r'vcsn_context = \"[...]? → 𝔹\"\n')
        self.check(r'vcsn_context = \"lal(abc), b\"\n',
                   r'vcsn_context = \"[abc]? → 𝔹\"\n')
        self.check(r'context = "lal(\"), b"',
                   r'context = ["]? → 𝔹')

if __name__ == '__main__':
    unittest.main(testRunner=taprunner.TAPTestRunner(stream=sys.stdout)) # output='test-reports'
