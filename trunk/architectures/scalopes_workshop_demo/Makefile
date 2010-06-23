host:
	gcc ../../software/apps/edgeDetector/edgeDetector.c ../../software/apps/edgeDetector/ReadWriteBitmaps.c -DSAME_INPUT_FOLDER -o edgeDetector

cross:
	../../External_tools/cross-compilers/arm/bin/arm-elf-gcc ../../software/apps/edgeDetector/edgeDetector.c ../../software/apps/edgeDetector/ReadWriteBitmaps.c -o ../../edgeDetectorARM -specs=osemu.specs

