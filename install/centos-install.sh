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

# Install script for CentOS 5

# Prerequisite packages
#   GCC 4.4
#   SVN
#   gccxml
#   binutils
yum -y install openmpi-devel gcc-c++ libgfortran libibverbs-devel libibverbs binutils-devel python-devel gcc44-c++.i386

mkdir External_tools
cd External_tools

wget http://dl.iuscommunity.org/pub/ius/stable/Redhat/5/i386/epel-release-1-1.ius.el5.noarch.rpm

yum -y localinstall *.rpm --nogpgcheck
yum -y install gccxml libsigc++20 libsigc++20-devel python-hashlib python-ctypes

# Boost
wget http://downloads.sourceforge.net/project/boost/boost/1.41.0/boost_1_41_0.tar.gz
tar -xvzf boost_1_41_0.tar.gz
cd boost_1_41_0
./bootstrap.sh --prefix=/usr/local
./bjam install
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

# Make gcc recognizable
ln -s /usr/bin/g++ /usr/bin/g++-4.2.1

cd software
./waf configure --arm-compiler=../External_tools/cross-compilers/arm --sparc-compiler=../External_tools/cross-compilers/sparc
./waf
cd ..

# Configure resp
CC=gcc44 CXX=g++44 ./waf configure --with-systemc=External_tools/systemc-2.2.0 --with-tlm=External_tools/TLM2 --with-momh-header=/usr/local/include/libmomh
./waf

./waf -C
