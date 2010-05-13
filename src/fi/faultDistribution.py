"""This module contains the classes implementing the fault distributions (time and locations)"""

import random
import exceptions
import attributeWrapper
from attributeWrapper import value_change
from attributeWrapper import bit_flip
from attributeWrapper import bit_flip0
from attributeWrapper import bit_flip1

class uniformTimeDistribution:
    """This class represents a uniform distribution function on the specified time interval.
    """
    def __init__(self, simulationDuration = 1, numberOfTimeIntervals = 1):
        """Constructor. It receives as input the simulationDuration and optionally the number of time instants to be generated. 
        Then it initializes the random number generator"""
        if not (isinstance(simulationDuration,int) or isinstance(simulationDuration,long)):
            raise exceptions.Exception("simulationDuration must be a number")
        if not (simulationDuration > 0):
            raise exceptions.Exception("simulationDuration must be a positive number")
        if not (isinstance(numberOfTimeIntervals,int) or isinstance(numberOfTimeIntervals,long)):
            raise exceptions.Exception("numberOfTimeIntervals must be a number")
        if not (numberOfTimeIntervals >= 0):
            raise exceptions.Exception("numberOfTimeIntervals must be a not negative number")
        self.__simulationDuration = simulationDuration
        self.__numberOfTimeIntervals = numberOfTimeIntervals
        random.seed()

    def __call__(self):
        """Returns an ordered list of random number in the interal [0,simulationDuration]"""
        return self.getTimes()
        
    def setSimulationDuration(self, simulationDuration):
        if not (isinstance(simulationDuration,int) or isinstance(simulationDuration,long)):
            raise exceptions.Exception("simulationDuration must be a number")
        if not (simulationDuration > 0):
            raise exceptions.Exception("simulationDuration must be a positive number")
        self.__simulationDuration = simulationDuration

    def setNumberOfTimeIntervals(self, numberOfTimeIntervals):
        if not (isinstance(numberOfTimeIntervals,int) or isinstance(numberOfTimeIntervals,long)):
            raise exceptions.Exception("numberOfTimeIntervals must be a number")
        if not (numberOfTimeIntervals >= 0):
            raise exceptions.Exception("numberOfTimeIntervals must be a not negative number")
        self.__numberOfTimeIntervals = numberOfTimeIntervals        
    
    def getTimes(self):
        """Returns an ordered list of random number in the interal [0,simulationDuration]"""
        times = []
        for i in range(0,self.__numberOfTimeIntervals):
            times.append(random.randint(0,self.__simulationDuration - 1))
        times.sort()
        times.append(self.__simulationDuration)
        return times
        

class uniformLocationsDistribution:
    """This class represents a uniform distribution on the provided list of locations. It selects 
    randomly a bit to be corrupted among all the possible locations and provides all information 
    for performing the injection (mask function, the mask for corrupting the value, ...)
    """
    def __init__(self, locations = None):
        """Constructor. It initializes the random number generator"""
        self.__name__ = self.__class__.__name__
        self.__locations = locations
        self.__maskFunction = bit_flip()
        random.seed()
    
    def setLocations(self, locations):
        """Sets the list of locations of the current design"""
        self.__locations = locations

    def setFaultFunction(self, maskFunction):
        """Sets the mask function"""
        self.__maskFunction = maskFunction
    
    def __call__(self):
        """Generates the descriptor of fault"""
        return self.generateFault()
    
    def generateFault(self):
        """Generates a fault (or a list of faults) according to the fault model. This model select a random bit
        and apply on it a bitflip mask"""
        if self.__locations == None:
            raise exceptions.Exception('No location list has been set in the SoftErrorDistribution object')
        
        #no locations return empty dictiornary
        if len(self.__locations) == 0:
            return {}
        
        #compute the total number bits of locations
        numOfBits = 0
        for l in self.__locations:
            if not isinstance(l,attributeWrapper.locationDescriptor):
                raise exceptions.Exception('Locations must be object of locationDescriptor class')
            numOfBits = numOfBits + l.getOverallNbits()
        
        #generate the bit to be affected by the fault
        faultloc = random.randint(0,numOfBits-1)
        
        #decode the location
        #get the component index i in the description list
        currBit = self.__locations[0].getOverallNbits()
        i = 1
        while currBit < faultloc:
            currBit = currBit + self.__locations[i].getOverallNbits()
            i = i + 1
        i = i - 1
        currBit = currBit - self.__locations[i].getOverallNbits()

        #decode the line
        currBit = faultloc - currBit
        lineNum = int(currBit/self.__locations[i].getWordSize())
        #decode the selected bit
        bitNum = currBit - lineNum * self.__locations[i].getWordSize()
        
        #generate mask #TODO referred only to bit-flip.. support other models
        mask = 1
        for j in range(0,bitNum):
            mask = mask << 1
        
        #store all informations into a dictionary
        fault = {}
        fault['fault_model'] = self.__name__
        fault['mask_function'] = str(self.__maskFunction)
        fault['component'] = self.__locations[i].getComponent()
        fault['attribute'] = self.__locations[i].getAttribute()
        fault['wrapper'] = self.__locations[i].getWrapperClass()
        fault['line'] = lineNum
        fault['bit'] = bitNum
        fault['mask'] = mask
        
        #returns a list since the fault injector supports also MultiBit Upsets
        return [fault]
