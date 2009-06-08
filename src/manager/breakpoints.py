import respkernel
import sc_controller

""" Commodity module to handle callbacks and breakpoints via Python """

resp_ns = respkernel.get_namespace()
breaks = []

class GenericBreakpoint( sc_controller.DeltaCallback ):
    """ A generic breakpoint class: it stops simulation when a certain condition occurs """

    def __init__(self, object, attribute, checker):
        sc_controller.DeltaCallback.__init__(self)
        self.object = object
        self.attribute = attribute
        self.checker = checker

    def __call__(self):
        value = getattr(self.object, self.attribute)
        if self.checker(value):
            respkernel.controller.pause_simulation()

#
# Commodity checker classes
#
class equals():
    def __init__(self,y):
        self.y = y

    def __call__(self, x):
        if callable(x):
            return x() == self.y
        else:
            return x == self.y

class lesser():
    def __init__(self,y):
        self.y = y

    def __call__(self, x):
        if callable(x):
            return x() < self.y
        else:
            return x < self.y

class greater():
    def __init__(self,y):
        self.y = y

    def __call__(self, x):
        if callable(x):
            return x() > self.y
        else:
            return x > self.y

#
# Breakpoint registration and deregistration
#
def register_breakpoint(object, attribute, checker):
    gb = GenericBreakpoint( object, attribute, checker )
    breaks.append(gb)
    respkernel.controller.register_delta_callback( gb )
    return gb

def unregister_breakpoint(bp):
    respkernel.controller.remove_delta_callback( bp )
    breaks.remove(bp)
