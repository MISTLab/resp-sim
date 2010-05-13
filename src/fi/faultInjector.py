"""This module contains the fault injection manager. Its tasks are to generate and execute fault simulations"""

#import of required modules
import attributeWrapper
from attributeWrapper import *
from faultDistribution import *
                       
class faultInjector:
    """ This class support the definition and execution a fault injection campaign """
    def __init__(self, locationDistribution, timeDistribution):
        """Constructor"""
        #set to empty list the locations and the current fault list. 
        #The location is a list of location descriptors used for generating the fault list
        self.__currentFaultList = []
        self.__locationDistribution = locationDistribution
        self.__timeDistribution = timeDistribution
           
    def generateFaultList(self, simulationDuration, numberOfSims, numberOfTimeIntervals =1):
        """Generates a fault injection list"""
        if not (isinstance(simulationDuration,int) or isinstance(simulationDuration,long)):
            raise exceptions.Exception("simulationDuration must be a number")
        if not (simulationDuration > 0):
            raise exceptions.Exception("simulationDuration must be a positive number")
        if not (isinstance(numberOfSims,int) or isinstance(numberOfSims,long)):
            raise exceptions.Exception("numberOfSims must be a number")
        if not (numberOfSims > 0):
            raise exceptions.Exception("numberOfSims must be a positive number")
        if not (isinstance(numberOfTimeIntervals,int) or isinstance(numberOfTimeIntervals,long)):
            raise exceptions.Exception("numberOfTimeIntervals must be a number")
        if not (numberOfTimeIntervals > 0):
            raise exceptions.Exception("numberOfTimeIntervals must be a positive number")

        #reset current fault list
        self.__currentFaultList = []
        self.__timeDistribution.setSimulationDuration(simulationDuration)
        self.__timeDistribution.setNumberOfTimeIntervals(numberOfTimeIntervals)
        #generate numberOfSims simulations
        for s in range(0,numberOfSims):
            #get time instants
            times = self.__timeDistribution()
            currSim = {}
            t = -1
            for t in range(0,len(times)-1):
                curr_t = times[t]
                curr_l = self.__locationDistribution()
                currSim[curr_t] = curr_l
            #insert a dummy entry for executing the last time interval
            currSim[times[t+1]] = {}
            self.__currentFaultList.append(currSim)
     
    def printFaultList(self):
        """Prints on the screen the current fault list """
        for s in range(0,len(self.__currentFaultList)):
            print 'Simulation #' + str(s)
            times = self.__currentFaultList[s].keys()
            times.sort()
            for t in times:
                print '\t' + str(t) 
                for f in self.__currentFaultList[s][t]:
                    for i in f.keys():
                        print '\t\t' + str(i) + '\t' + str(f[i])       
            
    def saveFaultList(self, filename):
        """"Saves in a file the current fault list"""
        import server
        fp=open(filename,'w')
        fp.write(server.encode_compound(self.__currentFaultList))
        fp.close()
        
    def loadFaultList(self, filename):
        """Loads a fault list from a file"""
        import server
        fp=open(filename,'r')
        filecontent = ''
        self.__currentFaultList = []
        for l in fp:
            filecontent = filecontent + l
        fp.close()
        self.__currentFaultList = server.decode_compound(filecontent)        
    
