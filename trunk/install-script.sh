#!/bin/sh

################################################################################
#
#
#
#
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
#
#
#
#
#
#         v0.5 - Politecnico di Milano, European Space Agency
#            This tool is distributed under the GPL License
#
#
#
#
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
#
#
#
################################################################################

# Install script for Ubuntu 9.10 Karmic Koala

# Prerequisite packages
#   GCC 4.2
sudo apt-get -y install g++-4.2
#   SVN
sudo apt-get -y install subversion
#   python
sudo apt-get -y install python2.5
#   gccxml
sudo apt-get -y install gccxml
#   binutils
sudo apt-get -y install binutils-dev
#   boost
sudo apt-get -y install libboost-dev
#   doxygen (sorry)
sudo apt-get -y install doxygen

# Update alternatives
sudo update-alternatives --install /usr/bin/python python /usr/bin/python2.5 40
sudo update-alternatives --install /usr/bin/python python /usr/bin/python2.6 10

mkdir External_tools
cd External_tools

# Pyplusplus
echo "Downloading py++, be patient!"
#   pygccxml
#       download/install
svn co https://pygccxml.svn.sourceforge.net/svnroot/pygccxml/pygccxml_dev pygccxml
cd pygccxml
sudo python setup.py install
cd ..
#   py++
#       download/install
svn co https://pygccxml.svn.sourceforge.net/svnroot/pygccxml/pyplusplus_dev pyplusplus
cd pyplusplus
sudo python setup.py install
cd ..

# libMOMH
#   download
wget http://www.jumpjoe.com/sysc/libmomh-1.91.3.tar.bz2
tar xvzf libmomh-1.91.3.tar.bz2
#   configure/install
cd libmomh-1.91.3
./configure
make -j 2
sudo make install
cd ..

# SystemC 2.2
wget http://www.jumpjoe.com/sysc/systemc-2.2.0_gcc4.tar.bz2
tar xvjf systemc-2.2.0_gcc4.tar.bz2
cd systemc-2.2.0
./configure
make -j 2
sudo make install
cd ..

# TLM
wget http://www.jumpjoe.com/sysc/TLM2.tar.bz2
tar xvjf TLM2.tar.bz2

# TRAP
#   download
svn checkout http://trap-gen.googlecode.com/svn/trunk/ trap-gen
#   configure/install
cd trap-gen
./waf configure --with-systemc=../systemc-2.2.0
./waf
sudo ./waf install
cd ../..

# Configure resp
./waf configure --with-systemc=External_tools/systemc-2.2.0 --with-tlm=External_tools/TLM2 --with-momh-header=/usr/local/include/libmomh
./waf

#cleanup
