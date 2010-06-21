#! /usr/bin/env python
# -*- coding: iso-8859-1 -*-

#TODO explain better how it works
"""This script is used by the fault injector to execute a fault injection campaign. The executeCAmpaign method of the fault injector 
   passes it as batch option when starting the subprocess. In this way it is executed after the loading of the architecture. 
   FAULTLIST and FIRSTEXP are specified with the additional-command option."""

def run_campaign():
    '''Executes the fault injection campaign. It ends when all the experiments are carried out or if an experiment causes a crash
    of the simulation platform. DO NOTE: The script is specified in a function to avoid that the reset function deletes FAULTLIST 
    and FIRST_EXPERIMENT parameters from the namespace'''
    
    #check in the namespace for the variables specifying the name of the file containing the fault list and the
    #number of the first experiment to be executed
    try:
        FAULTLIST = globals()['FAULTLIST']
    except:
        FAULTLIST = "tmpList.txt"
    try:
        FIRST_EXPERIMENT = int(globals()['FIRST_EXPERIMENT'])
    except:
        FIRST_EXPERIMENT = 0
        
    #get the reference of the fault injector and load the fault list from the file
    fi=manager.getFaultInjector()
    fi.loadFaultList(FAULTLIST)

    #start the fault campaign. this loop ends when all the experiments are carried out or if an experiment makes ReSP to crash
    firstIter = True
    for i in range(FIRST_EXPERIMENT, fi.numberOfExperiments()):
        print "---------------------------------------------------------------------------------"
        print "-- Experiment N° " + str(i)
        print "---------------------------------------------------------------------------------"
        if not firstIter:
          reload_architecture()        
        firstIter = False
        #log the number of the current experiment
        f = open('__currsim', 'w')
        f.write(str(i))
        f.close()
        #execute the experiment
        fi.executeSingleFault(i)
        #reset
        #reset()
        #reload_architecture()

#run the script
run_campaign()
