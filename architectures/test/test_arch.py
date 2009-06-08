#This file contains a simple architecture composed of a master and a slave; they exchange characters. Trough this file it
#is possible to test if the setup was succesfull

#Creation of the master and slave components; note how what we do is actually calling the constructor of the classes. The
#call here is performed in python, and this in turn calls the C++ constructor. These python constructors indeed have
#the same signature of the c++ ones
instMaster = SimpleMaster.SimpleMaster('master')
instSlave = SimpleSlave.SimpleSlave('slave')

#Connection of the ports: first we specify the source component and its ports; then the target component and its port.
connectPortsForce(instMaster, instMaster.initiator_port, instSlave, instSlave.target_port)

#finally we start the simulation for 10 NS
run_simulation(10)
