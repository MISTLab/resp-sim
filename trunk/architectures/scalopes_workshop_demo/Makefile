host:
	gcc ../../software/apps/edgeDetector/edgeDetector.c ../../software/apps/edgeDetector/ReadWriteBitmaps.c -DSAME_INPUT_FOLDER -o edgeDetector

cross:
	../../External_tools/cross-compilers/arm/bin/arm-elf-gcc ../../software/apps/edgeDetector/edgeDetector.c ../../software/apps/edgeDetector/ReadWriteBitmaps.c -o ../../edgeDetectorARM -specs=osemu.specs

crossomp:
	../../External_tools/cross-compilers/arm/bin/arm-elf-gcc ../../software/apps/edgeDetector/edgeDetectorParallel.c ../../software/apps/edgeDetector/ReadWriteBitmaps.c -o ../../edgeDetectorARMParallel -fopenmp -I../../software/suites/OmpSCR_v2.0/include/ -L../../software/build/arm/lib/libgomp/ -L../../software/build/arm/lib/pthread_stub/ -lgomp_arm -lpthread_stub_arm -specs=osemu.specs

