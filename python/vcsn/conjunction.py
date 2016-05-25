class Conjunction(object):

    '''A proxy class that delays calls to the & operator in order
    to turn a & b & c into a variadic evaluation of
    vcsn.automaton.conjunction(a, b, c).

    Based on
    http://code.activestate.com/recipes/496741-object-proxying
    '''
    __slots__ = ["auts", "__weakref__"]

    def __init__(self, *args):
        object.__setattr__(self, "auts", [args[0]])
        for arg in args[1:]:
            object.__getattribute__(self, "__and__")(arg)

    def __and__(self, arg):
        if isinstance(arg, int):
            object.__getattribute__(self, "auts")[-1] =  \
                object.__getattribute__(self, "auts")[-1].conjunction(arg)
        else:
            object.__getattribute__(self, "auts").append(arg)
        return self

    def __value__(self):
        aut = object.__getattribute__(self, "auts")
        if isinstance(aut, list):
            if len(aut) == 1:
                aut = aut[0]
            else:
                aut = aut[0].conjunction(*(aut[1:]), lazy=False)
            object.__setattr__(self, "auts", aut)
        return aut

    def __getattribute__(self, name):
        val = object.__getattribute__(self, "__value__")
        if name is "__value__":
            return val
        else:
            return getattr(val(), name)

    def __delattr__(self, name):
        delattr(self.__value__(), name)

    def __setattr__(self, name, value):
        setattr(self.__value__(), name, value)

    def __nonzero__(self):
        return bool(self.__value__())

    def __str__(self):
        return str(self.__value__())

    def __repr__(self):
        return repr(self.__value__())

    _special_names = [
        '__abs__', '__add__',
        '__call__',
        '__cmp__', '__coerce__',
        '__contains__', '__delitem__', '__delslice__', '__div__', '__divmod__',
        '__eq__', '__float__', '__floordiv__', '__ge__', '__getitem__',
        '__getslice__', '__gt__', '__hash__', '__hex__', '__iadd__', '__iand__',
        '__idiv__', '__idivmod__', '__ifloordiv__', '__ilshift__', '__imod__',
        '__imul__', '__int__', '__invert__', '__ior__', '__ipow__', '__irshift__',
        '__isub__', '__iter__', '__itruediv__', '__ixor__', '__le__', '__len__',
        '__long__', '__lshift__', '__lt__', '__mod__', '__mul__', '__ne__',
        '__neg__', '__oct__', '__or__', '__pos__', '__pow__', '__radd__',
        '__rand__', '__rdiv__', '__rdivmod__', '__reduce__', '__reduce_ex__',
        '__repr__', '__reversed__', '__rfloorfiv__', '__rlshift__', '__rmod__',
        '__rmul__', '__ror__', '__rpow__', '__rrshift__', '__rshift__', '__rsub__',
        '__rtruediv__', '__rxor__', '__setitem__', '__setslice__', '__sub__',
        '__truediv__', '__xor__', 'next',
    ]

    @classmethod
    def _create_class_proxy(cls, theclass):
        def make_method(name):
            def method(self, *args, **kw):
                return getattr(self.__value__(), name)(*args, **kw)
            return method
        namespace = {}
        for name in cls._special_names:
            if hasattr(theclass, name):
                namespace[name] = make_method(name)
        return type("%s(%s)" % (cls.__name__, theclass.__name__), (cls,), namespace)

    def __new__(cls, obj, *args, **kwargs): # pylint: disable=unused-argument
        try:
            cache = cls.__dict__["_class_proxy_cache"]
        except KeyError:
            cls._class_proxy_cache = cache = {}
        try:
            theclass = cache[obj.__class__]
        except KeyError:
            cache[obj.__class__] = theclass = cls._create_class_proxy(
                obj.__class__)
        ins = object.__new__(theclass)
        return ins
