"""This module contains the wrappers for attributes representing the internal state of a component.
They provide an uniform way for accessing the internal state. In fact the internal state is represented and 
can be accessed in different ways in the several components; e.g., built-in type variables are simply 
accessed as common variables, ArchC register object offers to methods (read and write), ... 
The wrappers allows to read, write or apply a fault mask on the wrapped attribute.
If necessay, add here the wrappers for the other components
"""

import exceptions

#################################################################################
## Classes performing basic value modification. They are called mask functions ##
#################################################################################
class value_change:
    def __call__(self, value, mask):
        """Returns the mask value"""
        return mask
    def __repr__(self):
        return "value_change"

class bit_flip:
    def __call__(self, value, mask):
        """Bit-flips the value parameter with the given mask"""
        return value^mask
    def __repr__(self):
        return "bit_flip"

class bit_flip0:
    def __call__(self, value, mask):
        """Bit-flips to 0 the value parameter with the given mask"""
        return ((value & ~mask)|(mask & 0))
    def __repr__(self):
        return "bit_flip0"

class bit_flip1:
    def __call__(self, value, mask):
        """Bit-flips to 1 the value parameter with the given mask"""
        return ((value & ~mask)|(mask & 1))
    def __repr__(self):
        return "bit_flip1"

maskFunctions = [value_change, bit_flip, bit_flip0, bit_flip1]


def getSaboteurMaskFunction(function):
    """given a mask function, it returns the correspondent function for the saboteur"""
    import SaboteurLT32
    if isinstance(function, value_change):
        return SaboteurLT32.SaboteurLT32.VALUE_CHANGE
    elif isinstance(function, bit_flip):
        return SaboteurLT32.SaboteurLT32.BIT_FLIP
    elif isinstance(function, bit_flip1):
        return SaboteurLT32.SaboteurLT32.BIT_FLIP1
    elif isinstance(function, bit_flip0):
        return SaboteurLT32.SaboteurLT32.BIT_FLIP0
    else:
        raise exceptions.Exception(str(function) + " is not a valid mask function")

################################################################################################
## Classes implementing wrappers for provinding an uniform access to every internal attribute ##
################################################################################################


class baseAttributeWrapper:
    """The base wrapper for component internal state (variables, objects,...). All wrappers extend this class """
    def __init__(self):
        """Constructor"""
        self.__component = None
        self.__attribute = None
        self.__faultModel = None
    def getComponent(self):
        """Gets the reference of the component"""
        return self.__component
    def getAttribute(self):
        """Gets the name of the attribute"""
        return self.__attribute
    def getfaultModel(self):
        """Gets the reference of the mask"""
        return self.__faultModel
        
        
class variableWrapper(object, baseAttributeWrapper):
    """The wrapper for attributes of built-in type"""
    def __init__(self, component, attribute, maskFunction, index):
        """Constructor. It takes the reference of the component and the name of 
        the variable and the mask function used for corrupting the variable content.
        Finally, the index parameter is actualy ignored since it is a constant equal to 0"""
        if faultModels.maskFunctions.count(maskFunction.__class__) == 0:
            raise exceptions.Exception(str(maskFunction)+ " is not a valid mask function")
        self.__component = component
        self.__attribute = attribute
        self.__maskFunction = maskFunction
    def applyMask(self, mask):
        """Corrupts the content with a mask passed as parameter"""
        self.value = self.__maskFunction(self.value, mask)
    def __getValue(self):
        """Returns the attribute value"""
        return getattr(self.__component, self.__attribute)
    def __setValue(self, value):
        """Sets a specified value to the attribute"""
        setattr(self.__component, self.__attribute, value)
    value = property(__getValue, __setValue) #define a property

    
class saboteurAccess(object, baseAttributeWrapper): #TODO: to be updated
    """The wrapper for saboteurs"""
    def __init__(self, component, attribute, maskFunction, index = 0):
        """Constructor. It takes the reference of the component and the name of 
        the variable. Index parameter is actualy ignored sinze is a constant equal to 0"""
        import helper
        import SaboteurLT32
        if not isinstance(component,SaboteurLT32.SaboteurLT32):
            raise exceptions.Exception(str(component)+ " is not an instance of Saboteur.Saboteur")
        try:
            saboteurIndex = SaboteurLT32.SaboteurLT32.lineType.values[int(index)]
        except:
            import traceback
            traceback.print_exc()
            raise exceptions.Exception(str(index)+ " is not a valid index for saboteur wrapper")
        if faultModels.maskFunctions.count(maskFunction.__class__) == 0:
            raise exceptions.Exception(str(maskFunction)+ " is not a valid mask function")
        self.__component = component
        self.__attribute = attribute
        self.__maskFunction = maskFunction
        self.__index = saboteurIndex
    def applyMask(self, mask):
        """Corrupts the content with a mask passed as parameter"""
        maskFunction = faultModels.getSaboteurMaskFunction(self.__maskFunction)
        self.__component.setMask(maskFunction, mask, self.__index)
    def __getValue(self):
        """Returns the register value"""
        raise exceptions.Exception('__getValue method cannot be called on saboteurWrapper')
    def __setValue(self, value):
        """Sets the register value"""
        raise exceptions.Exception('__setValue method cannot be called on saboteurWrapper')
    value = property(__getValue, __setValue) #define a property


########################################################################################
## Class representing the descriptor of single location where a fault can be injected ##
########################################################################################
    
class locationDescriptor:
    """The descriptor of an attribute of a component. It is used for describing a possible
    fault location (both as descriptor of the component class and of the component instance. 
    A location is a single variable or an array or an object whose state can be
    represented with a single variable or an array. The descriptor builds also the wrapper for 
    access in a standard way to the fault location"""
    def __init__(self, component, attribute, wrapperClass, lines=1, wordSize=32):
        """Constructor. If it is the descriptor of a location of a component instance, it takes as input
        1) the component name, 
        2) the name of the object attribute, 
        3) the name of the wrapper class (built-in type variable, Trap register, Trap register bank, ...),
        4) the number of lines (base storage cells),
        5) the word size. 
        If it is  the descriptor of a component class, it takes in input 
        1) the class of the component, 
        2) the name of the object attribute,
        3) the name of the wrapper class (built-in type variable, Trap register, Trap register bank, ...),
        4) the number of lines that may be also the name of the function to be called to get the actual value, and 
        5) the word size that may be also the name of the function to be called to get the actual value, and         
        """
        self.__component = component
        self.__attribute = attribute
        self.__wrapperClass = wrapperClass
        self.__lines = lines
        self.__wordSize = wordSize
    def __repr__(self):
        """Returns the string representation of the location descriptor"""
        return 'LocationDescriptor: [component: ' + str(self.__component) + ', attribute: ' +  str(self.__attribute) + \
            ', wrapper: ' + str(self.__wrapperClass) + ', number of lines: ' + str(self.__lines) + \
            ', word size: ' + str(self.__wordSize) + ']'
    def getComponent(self):
        """Returns the component name"""
        return self.__component   
    def getAttribute(self):
        """Returns the attribute name"""
        return self.__attribute   
    def getLines(self):
        """Returns the number of lines contained in the attribute"""
        return self.__lines   
    def getWordSize(self):
        """Returns the size of the line"""
        return self.__wordSize
    def getOverallNbits(self):
        """Returns the overall number of bits contained into the attribute"""
        return self.__wordSize * self.__lines
    def getWrapperClass(self):
        """Returns the wrapper class name"""
        return self.__wrapperClass
