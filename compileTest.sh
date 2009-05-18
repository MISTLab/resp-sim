#!/bin/sh

./waf distclean
./waf configure --with-systemc=/home/beltrame/Tools/systemc/ --with-tlm=/home/beltrame/Tools/tlm/ > compResult.log 2>&1
echo "configured"
./waf -j3 -C >> compResult.log 2>&1
echo "compiled"
./waf distclean
./waf configure --with-systemc=/home/beltrame/Tools/systemc/ --with-tlm=/home/beltrame/Tools/tlm/ -S >> compResult.log 2>&1
echo "configured"
./waf -j3 -C >> compResult.log 2>&1
echo "compiled"
./waf distclean
./waf configure --with-systemc=/home/beltrame/Tools/systemc/ --with-tlm=/home/beltrame/Tools/tlm/ -P >> compResult.log 2>&1
echo "configured"
./waf -j3 -C >> compResult.log 2>&1
echo "compiled"
./waf distclean
./waf configure --with-systemc=/home/beltrame/Tools/systemc/ --with-tlm=/home/beltrame/Tools/tlm/ -D >> compResult.log 2>&1
echo "configured"
./waf -j3 -C >> compResult.log 2>&1
echo "compiled"
./waf distclean
./waf configure --with-systemc=/home/beltrame/Tools/systemc/ --with-tlm=/home/beltrame/Tools/tlm/ -S -P -D >> compResult.log 2>&1
echo "configured"
./waf -j3 -C >> compResult.log 2>&1
echo "compiled"
./waf distclean
./waf configure --with-systemc=/home/beltrame/Tools/systemc/ --with-tlm=/home/beltrame/Tools/tlm/ --static-platform=architectures/static_platforms/armpid_functional_OSemu.cpp >> compResult.log 2>&1
echo "configured"
./waf -j3 -C >> compResult.log 2>&1
echo "compiled"
./waf distclean
./waf configure --with-systemc=/home/beltrame/Tools/systemc/ --with-tlm=/home/beltrame/Tools/tlm/ -S --static-platform=architectures/static_platforms/armpid_functional.cpp >> compResult.log 2>&1
echo "configured"
./waf -j3 -C >> compResult.log 2>&1
echo "compiled"
./waf distclean
./waf configure --with-systemc=/home/beltrame/Tools/systemc/ --with-tlm=/home/beltrame/Tools/tlm/ --enable-processor="arm7" >> compResult.log 2>&1
echo "configured"
./waf -j3 -C >> compResult.log 2>&1
echo "compiled"
./waf distclean
./waf configure --with-systemc=/home/beltrame/Tools/systemc/ --with-tlm=/home/beltrame/Tools/tlm/ --enable-processor="mips1" >> compResult.log 2>&1
echo "configured"
./waf -j3 -C >> compResult.log 2>&1
echo "compiled"
./waf distclean
./waf configure --with-systemc=/home/beltrame/Tools/systemc/ --with-tlm=/home/beltrame/Tools/tlm/ --enable-processor="arm7ca" >> compResult.log 2>&1
echo "configured"
./waf -j3 -C >> compResult.log 2>&1
echo "compiled"
./waf distclean
./waf configure --with-systemc=/home/beltrame/Tools/systemc/ --with-tlm=/home/beltrame/Tools/tlm/ --enable-processor="powerpc" >> compResult.log 2>&1
echo "configured"
./waf -j3 -C >> compResult.log 2>&1
echo "compiled"
./waf distclean
./waf configure --with-systemc=/home/beltrame/Tools/systemc/ --with-tlm=/home/beltrame/Tools/tlm/ --enable-processor="ppc405" >> compResult.log 2>&1
echo "configured"
./waf -j3 -C >> compResult.log 2>&1
echo "compiled"
./waf distclean
./waf configure --with-systemc=/home/beltrame/Tools/systemc/ --with-tlm=/home/beltrame/Tools/tlm/ --enable-processor="leon2" >> compResult.log 2>&1
echo "configured"
./waf -j3 -C >> compResult.log 2>&1
echo "compiled"
./waf distclean
./waf configure --with-systemc=/home/beltrame/Tools/systemc/ --with-tlm=/home/beltrame/Tools/tlm/ --enable-processor="leon2 arm7" >> compResult.log 2>&1
echo "configured"
./waf -j3 -C >> compResult.log 2>&1
echo "compiled, test ENDED"
