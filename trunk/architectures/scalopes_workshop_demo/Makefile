host:
	gcc ../../software/apps/edgeDetector/edgeDetector.c ../../software/apps/edgeDetector/ReadWriteBitmaps.c -DSAME_INPUT_FOLDER -o edgeDetector

cross:
	../../External_tools/cross-compilers/arm/bin/arm-elf-gcc ../../software/apps/edgeDetector/edgeDetector.c ../../software/apps/edgeDetector/ReadWriteBitmaps.c -o ../../edgeDetectorARM -specs=osemu.specs -fdata-sections -ffunction-sections -static

crossomp:
	../../External_tools/cross-compilers/arm/bin/arm-elf-gcc ../../software/apps/edgeDetector/edgeDetectorParallel.c -o ../../edgeDetectorARMParallel.o -c -specs=osemu.specs -fdata-sections -ffunction-sections -fopenmp -static -I../../software/suites/OmpSCR_v2.0/include/
	../../External_tools/cross-compilers/arm/bin/arm-elf-gcc ../../software/apps/edgeDetector/ReadWriteBitmaps.c -o ../../ReadWriteBitmaps.o -c -specs=osemu.specs -fdata-sections -ffunction-sections -static
	../../External_tools/cross-compilers/arm/bin/arm-elf-gcc ../../edgeDetectorARMParallel.o ../../ReadWriteBitmaps.o -o ../../edgeDetectorARMParallel -specs=osemu.specs -L../../software/build/arm/lib/libgomp/ -L../../software/build/arm/lib/pthread_stub/ -L../../software/build/arm/suites/OmpSCR_v2.0/common/ -lompsrc_cc -lgomp_arm -lpthread_stub_arm
