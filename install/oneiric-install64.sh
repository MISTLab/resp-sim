#!/bin/sh

################################################################################
#              ___           ___           ___           ___
#             /  /\         /  /\         /  /\         /  /\
#            /  /::\       /  /:/_       /  /:/_       /  /::\
#           /  /:/\:\     /  /:/ /\     /  /:/ /\     /  /:/\:\
#          /  /:/~/:/    /  /:/ /:/_   /  /:/ /::\   /  /:/~/:/
#         /__/:/ /:/___ /__/:/ /:/ /\ /__/:/ /:/\:\ /__/:/ /:/
#         \  \:\/:::::/ \  \:\/:/ /:/ \  \:\/:/~/:/ \  \:\/:/
#          \  \::/~~~~   \  \::/ /:/   \  \::/ /:/   \  \::/
#           \  \:\        \  \:\/:/     \__\/ /:/     \  \:\
#            \  \:\        \  \::/        /__/:/       \  \:\
#             \__\/         \__\/         \__\/         \__\/
#
#         v0.5 - Politecnico di Milano, European Space Agency
#            This tool is distributed under the GPL License
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the
#  Free Software Foundation, Inc.,
#  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA.
#
################################################################################

# Install script for Ubuntu 11.10 Oneiric Ocelot

# Boost libraries ver. 1.46 are not supported
sudo apt-get -y remove libboost-thread1.46-dev libboost-filesystem1.46.1 libboost-regex1.46-dev libboost-program-options1.46-dev libboost-graph-parallel1.46-dev libboost-graph-parallel1.46.1 libboost-graph1.46-dev libboost-all-dev libboost-system1.46.1 libboost-date-time-dev libboost-python1.46.1 libboost-mpi1.46-dev libboost-signals1.46.1 libboost-signals-dev libboost-program-options1.46.1 libboost-date-time1.46.1 libboost-thread-dev libboost-test1.46.1 libboost-iostreams1.46.1 libboost-program-options-dev libboost-wave1.46.1 libboost-test-dev libboost-wave-dev libboost-graph1.46.1 libboost-graph-parallel-dev libboost-date-time1.46-dev libboost-serialization-dev libboost-system-dev libboost1.46-dev libboost-regex-dev libboost-iostreams1.46-dev libboost-math-dev libboost-thread1.46.1 libboost-math1.46-dev libboost-python1.46-dev libboost-signals1.46-dev libboost-mpi-dev libboost-mpi1.46.1 libboost-system1.46-dev libboost-filesystem-dev libboost-serialization1.46-dev libboost-wave1.46-dev libboost-dev libboost-math1.46.1 libboost-iostreams-dev libboost-mpi-python-dev libboost-regex1.46.1 libboost-filesystem1.46-dev libboost-test1.46-dev libboost-graph-dev libboost-python-dev

# Prerequisite packages
sudo apt-get -y install libsigc++-2.0-dev subversion binutils-dev mpi-default-dev g++ gcc-multilib g++-multilib gccxml libgfortran3 libibverbs-dev libibverbs1 libicu-dev libicu44 libnuma1 libopenmpi-dev libopenmpi1.3 libstdc++6-4.4-dev openmpi-common python-dev ia32-libs patch automake libboost1.42-dev libboost1.42-all-dev libboost1.42-dbg libboost-thread1.42.0 libboost-system1.42.0 libboost-regex1.42.0 libboost-program-options1.42.0 libboost-filesystem1.42.0

mkdir External_tools
cd External_tools

# Pyplusplus
#     pygccxml
#         download/install
svn co https://pygccxml.svn.sourceforge.net/svnroot/pygccxml/pygccxml_dev pygccxml
cd pygccxml
sudo python setup.py install
cd ..
#     py++
#         download/install
svn co https://pygccxml.svn.sourceforge.net/svnroot/pygccxml/pyplusplus_dev pyplusplus
cd pyplusplus
sudo python setup.py install
cd ..

# libMOMH
#     download
wget http://resp-sim.googlecode.com/files/libmomh-1.91.3.tar.bz2
tar xvjf libmomh-1.91.3.tar.bz2
#     configure/install
cd libmomh-1.91.3
./configure
make -j 2
sudo make install
cd ..

# SystemC 2.2
wget http://resp-sim.googlecode.com/files/systemc-2.2.0_gcc4.6.tar.bz2
tar xvjf systemc-2.2.0_gcc4.6.tar.bz2
cd systemc-2.2.0
aclocal
automake --add-missing --copy
autoconf
./configure
make -j 2
sudo make install
cd ..

# TLM
wget http://resp-sim.googlecode.com/files/TLM2.tar.bz2
tar xvjf TLM2.tar.bz2

# TRAP
#     download
svn checkout http://trap-gen.googlecode.com/svn/trunk/ trap-gen
#     configure/install
cd trap-gen
./waf configure --with-systemc=../systemc-2.2.0 --license=gpl
./waf
sudo ./waf install
cd ..

# Cross-Compilers
#     download
mkdir cross-compilers
cd cross-compilers
wget http://resp-sim.googlecode.com/files/arm.tar.bz2
tar xvjf arm.tar.bz2
wget http://resp-sim.googlecode.com/files/sparc.tar.bz2
tar xvjf sparc.tar.bz2
cd ../..

# Configure software
cd software
./waf configure --arm-compiler=../External_tools/cross-compilers/arm --sparc-compiler=../External_tools/cross-compilers/sparc
./waf
cd ..

# Configure resp
./waf configure --with-systemc=External_tools/systemc-2.2.0 --with-tlm=External_tools/TLM2 --with-momh-header=/usr/local/include/libmomh
./waf

./waf -C
