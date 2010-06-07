"""This module contains the fault injection manager. Its tasks are to generate and execute fault simulations"""

import exceptions
                       
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
    
    def numberOfExperiments(self):
        """Returns the number of experiments in the fault list"""
        return len(self.__currentFaultList)
    
    def executeCampaign(self):
        """Executes a fault injection campaign with the current fault list"""
        
        #save fault list. It will be loaded by the subprocess executing the campaign...
        self.saveFaultList('__temp_list')
        #import necessary modules     
        import os
        import subprocess
        import sys
        import respkernel
        import resp
                            
        #set-up control stuff
        subproc = None #server subprocess reference
        num_of_errors = 0 #number of subprocesses that have crashed

        #execute the fault campaign        
        currExperiment = 0
        try:
            while currExperiment < self.numberOfExperiments():
                archFile = os.path.abspath(respkernel.get_architecture_filename())
                respFile = os.path.abspath(sys.modules['resp'].__file__)
                batchFile = os.path.abspath(sys.modules['fi'].__path__[0]) + '/executeCampaign.py'
                parameters = 'FAULT_INJECTION_CAMPAIGN=True;FAULTLIST=\'__temp_list\';FIRST_EXPERIMENT='+str(currExperiment)
                subproc = subprocess.Popen( ['python', respFile, '-a', archFile, '--silent', '--no-banner', '--batch', batchFile, '-d',parameters]) #, stdin=subprocess.PIPE, stdout=sys.stdout )
                subproc.wait()
                num_of_errors = num_of_errors + 1
                f = open('__currsim', 'r')
                executed = long(f.readline())
                f.close()
                currExperiment = executed + 1
            if os.path.isfile('__temp_list'):
                os.remove('__temp_list')
            if os.path.isfile('__currsim'):
                os.remove('__currsim')
            num_of_errors = num_of_errors - 1
        except Exception, e:
            import traceback
            traceback.print_exc()
            if os.path.isfile('__temp_list'):
                os.remove('__temp_list')
            if os.path.isfile('__currsim'):
                os.remove('__currsim')
            raise e
        
        if self.numberOfExperiments() > 0:
            try:
                # Call a custom statsprinter if registered
                import respkernel
                respkernel.campaignReportPrinter()
            except:
                print "\n\n-------------------------------------------------------------------------------------------------------" 
                print "Statistics:"
                print "Number of simulations: " + str(len(self.__currentFaultList))
                print "Number of simulation terminated with an error: " + str(num_of_errors)
                print "-------------------------------------------------------------------------------------------------------" 
        else:
            print 'No experiment has been executed; fault list is empty'
            
    def executeSingleFault(self, num):
        """Executes a fault simulation"""
        import respkernel
        resp_ns = respkernel.get_namespace()
        controller = resp_ns['controller']

        #import of required modules
        import attributeWrapper

        if controller.interactive == True:
            raise exceptions.Exception('It is not possible to run executeSingleFault function in the interactive mode')
        
        if not isinstance(num,int) and not isinstance(num,long):
            raise exceptions.Exception('Not compatible parameter received by the executeSingleFault function')
        if not num >= 0 and num < self.__currentFaultList:
            raise exceptions.Exception('Not compatible parameter received by the executeSingleFault function')            
        command = self.__currentFaultList[num]
        
        times = command.keys()
        if not (len(times) > 0):
            raise exceptions.Exception('Empty fault list: the simulation cannot be executed')
        
        #compute time deltas to be run
        times.sort()
        delta = {}
        delta[times[0]] = times[0]
        for i in range(1,len(times)):
            delta[times[i]] = times[i] - times[i-1]
        #print times
        for t in times: #execute all delta and inject
            #run for a delta time            
            controller.run_simulation(delta[t])                        
            #print controller.get_simulated_time()
            #inject fault
            if not t == times[len(times)-1]: #after the last delta time no injection is performed
                injections = command[t]
                #inject all the faults
                for fault in injections:
                    #get the reference to the class of the mask function
                    maskFunctionName = fault['mask_function']
                    if dir(resp_ns).count(maskFunctionName) != 0:
                        maskFunction = resp_ns[maskFunctionName]()
                    elif dir(attributeWrapper).count(maskFunctionName) != 0:#maskFunctions.count(maskFunctionName) != 0:
                        maskFunction = getattr(attributeWrapper,maskFunctionName)()
                    else:
                        raise exceptions.Exception( str(maskFunctionName) + ' is not a valid mask function')
                    
                    #get the reference to the class of the attribute wrapper
                    wrapperClassName = fault['wrapper']
                    if dir(resp_ns).count(wrapperClassName) != 0:
                        wrapperClass = resp_ns[wrapperClassName]
                    elif dir(attributeWrapper).count(wrapperClassName) != 0:
                        wrapperClass = getattr(attributeWrapper,wrapperClassName)
                    else:
                        raise exceptions.Exception( str(wrapperClassName) + ' is not a valid wrapper class')
                    
                    wrapper = wrapperClass(resp_ns['manager'].getCompInstance(fault['component']), fault['attribute'], maskFunction, fault['line'])
                    wrapper.applyMask(fault['mask'])                    

            #check if simulation is finished
            if controller.is_finished(): 
                break
        if not controller.is_finished():
            controller.stop_simulation()
       
