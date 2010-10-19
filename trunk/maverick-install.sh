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

# Install script for Ubuntu 10.10 Maverick

# Prerequisite packages
#   GCC 4.2
#   SVN
#   gccxml
#   binutils
sudo apt-get -y install libsigc++-2.0-dev subversion binutils-dev mpi-default-dev g++ g++-4.4 gccxml libgfortran3 libibverbs-dev libibverbs1 libicu-dev libicu42 libnuma1 libopenmpi-dev libopenmpi1.3 libstdc++6-4.4-dev openmpi-common python-dev python2.6-dev libboost1.42-dev libboost1.42-all-dev libboost-dbg libboost-thread1.42.0 libboost-system1.42.0 libboost-regex1.42.0 libboost-program-options1.42.0 libboost-filesystem1.42.0

mkdir External_tools
cd External_tools

# g++-4.2
#     download
mkdir gcc-4.2
cd gcc-4.2
wget http://de.archive.ubuntu.com/ubuntu/pool/universe/g/gcc-4.2/cpp-4.2_4.2.4-5ubuntu1_i386.deb
wget http://de.archive.ubuntu.com/ubuntu/pool/universe/g/gcc-4.2/libstdc++6-4.2-dev_4.2.4-5ubuntu1_i386.deb
wget http://de.archive.ubuntu.com/ubuntu/pool/universe/g/gcc-4.2/gcc-4.2-base_4.2.4-5ubuntu1_i386.deb
wget http://de.archive.ubuntu.com/ubuntu/pool/universe/g/gcc-4.2/gcc-4.2_4.2.4-5ubuntu1_i386.deb
wget http://de.archive.ubuntu.com/ubuntu/pool/universe/g/gcc-4.2/g++-4.2_4.2.4-5ubuntu1_i386.deb
sudo dpkg -i *.deb
cd ..

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
wget http://resp-sim.googlecode.com/files/systemc-2.2.0_gcc4.tar.bz2
tar xvjf systemc-2.2.0_gcc4.tar.bz2
cd systemc-2.2.0
sudo rm -r lib-linux
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
./waf configure --with-systemc=../systemc-2.2.0
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
