#! /usr/bin/env python

import gdb, gdb.printing, re
from vcsn.demangle import demangle

def build_pretty_printer():
  pp = gdb.printing.RegexpCollectionPrettyPrinter("")
  pp.add_printer('Vcsn', '.*$', VcsnPrinter)
  return pp

class VcsnPrinter:
  """Print a Vcsn object."""

  def __init__(self, val):
    self.val = val

  def to_string(self):
    return demangle(str(self.val.type))

gdb.printing.register_pretty_printer(
  gdb.current_objfile(),
  build_pretty_printer())
