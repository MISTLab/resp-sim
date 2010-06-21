from respkernel import get_namespace
controller = get_namespace()['controller']
import sc_controller_wrapper

""" Commodity module to handle callbacks and breakpoints via Python """

global breaks
breaks = []

class GenericBreakpoint( sc_controller_wrapper.DeltaCallback ):
    """ A generic breakpoint class: it stops simulation when a certain condition occurs """

    def __init__(self, object, attribute, checker):
        sc_controller_wrapper.DeltaCallback.__init__(self)
        self.object = object
        self.attribute = attribute
        self.checker = checker

    def __call__(self):
        value = getattr(self.object, self.attribute)
        if self.checker(value):
            controller.pause_simulation()

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
    """Register a breakpoint. When the condition specified in the checker is verified, the simulation is paused"""
    global breaks

    gb = GenericBreakpoint( object, attribute, checker )
    breaks.append(gb)
    sc_controller_wrapper.registerDeltaCallback( gb )

def reset():
    global breaks
    for i in breaks:
        del i
    breaks = []

#TODO support it into the simulation controller
#def unregister_breakpoint(bp):
#    respkernel.controller.remove_delta_callback( bp )
#    breaks.remove(bp)
