import vcsn
def test():
    def test1():
        def test2():
            '{:f}'.format(vcsn.Q.expression('abc').automaton())
        test2()
    test1()
vcsn._traceback_verbose = True
test()
