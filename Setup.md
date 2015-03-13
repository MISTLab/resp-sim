# Simplified installation for Ubuntu 9.10 Karmic Koala #

Ubuntu 9.10 has all the proper versions of dependent software in its package repository. We provide a single script to collect all the dependent software and perform its installation.
Prerequisites:
  * You need to have administrator rights to perform the automatic installation. The script uses the standard "sudo" command, and will most likely require the input of your password only once
  * You must have an active internet connection to download the required packages
  * ReSP must have been downloaded from the SVN repository

Installation:
Simply run the script karmic-install.sh from the root of the ReSP tree.

# Virtual Machine #

A Virtual Machine running with VirtualBox v3.0 is distributed on the project [project SFTP server](SftpPage.md) with a version of ReSP already configured and compiled. The user can simply download it and directly start to use the tool.

# Building and Installation Instruction #

ReSP uses waf (http://code.google.com/p/waf) as build system; from a user point of view it is similar to the usual autotools
based project: first there is a configuration step, then the compilation and, finally, the installation.

## Required Libraries and Tools ##

  * BFD: part of the binutils, it is used to read and parse the application software which is going to be executed on the processors. The development version of this package is needed. Note that on 64 bits architectures you might need to manually download binutils sources and patch them according to the instructions in _ext/binutils_ (you should see if you need to patch them if you get an error during the configuration process)
  * SystemC: downloadable from http://www.systemc.org, it is used to manage simulated time and the communication among the hardware modules. Note that on 64 bits architectures you might need to patch the sources; instructions on how to patch is are in _ext/systemc_ (you should see if you need to patch them if you get an error during the configuration process)
  * TLM 2.0: downloadable from http://www.systemc.org, it is used to manage connections among the hardware modules.
  * Boost: Boost libraries, downloadable from http://www.boost.org.
  * Python version 2.4 - 2.6. Python 2.6 works only if the latest Boost libraries are installed (>= 1.42).
  * gccxml: at least version 0.9. Since there are different 0.9 releases, we suggest you go to website http://www.gccxml.org/HTML/Download.html and get the latest version from the CVS repository.
  * pyplusplus and pygccxml: at least version 1.5 is required. Such version has not been released yet as a separate download, but it is available on the development SVN repository (note that revision at least 1740 is required). Instructions on how to install it can be found at http://www.language-binding.net/pyplusplus/download.html.
  * TRAP: library for the description of instruction set simulators, dowloadable from the SVN repository of http://code.google.com/p/trap-gen/. Do not use the archive in the downloads since it is usually not updated.

There are also optional library which, if present, enhance ReSP's capabilities:

  * sigc++2.0: available in the packet manager of your distribution whatever you are using, it is necessary in case you also need to install the MOMH library
  * MOMH: necessary to enable the framework for automatic design space exploration. Instructions on how to download it are available at http://home.gna.org/momh/

### Configuration and Compilation ###

Once the processor implementation files are created, they need to be compiled; do do this the _waf_ build system is again used.

The configuration options which can be passed to the _./waf configure_ command are:
  1. _--with-systemc=SYSC\_FOLDER_ specifies the location of the SystemC 2.2 library. It is compulsory to specify such option
  1. _--with-tlm=TLM\_FOLDER_ specifies the location of the TLM 2.0 library. It is compulsory to specify such option
  1. _--with-trap=TRAP\_FOLDER_ specifies the location of the TRAP runtime libraries and headers. It must be specified if TRAP was installed in a custom folder
  1. _--boost-includes=BOOSTINCLUDES_ specifies the location of the include files of the boost lbraries; it has to be specified in case the libraries are not installed in a standard path.
  1. _--boost-libs=BOOSTLIBS_ specifies the location of the library files of the boost lbraries; it has to be specified in case the libraries are not installed in a standard path.
  1. _--with-bfd=BFD\_FOLDER_ specifies the location of the BFD library; if this option is not specified ReSP will look for libbfd in the standard library search path.

After the configuration successfully ends, compilation can be started with the usual _./waf_ command; after compilation, run the ./startSim.sh script to start the simulator.

### Cross-compiled Software ###

In order to run pieces of software on the Instruction Set Simulators, it is required to compile them using some specific cross-compilers. These can be downloaded from the [project SFTP server](SftpPage.md).
Once the compilers are decompressed, the applications can be properly cross-compiled using the _`[arch]`-elf-`[gcc/g++]`_ tool in the bin directory of the desired architecture. For the proper compilation of the applications making calls to operating system routines, the _-specs=osemu.specs_ command line option has to be passed to the compiler.
ReSP also provides a full set of example and benchmarks programs: the compilation of these applications can be automatically performed using the waf tool inside the _trunk/software_ directory of the SVN repository. The configuration options which can be passed to the _./waf configure_ command are:

  1. _--arm-compiler=/PATH\_TO/cross-compilers/arm/_ specifies the location of the ARM cross-compiler
  1. _--sparc-compiler=/PATH\_TO/cross-compilers/sparc/_ specifies the location of the SPARC cross-compiler for the LEON ISSs

After the configuration successfully ends, compilation can be started with the usual _./waf_ command.

### Platform Testing ###
To check the correct compilation of the entire platform, the command _./waf -C_ can be issued in ReSP main directory. This command will launch the simulation of some architectures using some application contained into the software folder, thus even the software should be cross-compiled before launching the test.