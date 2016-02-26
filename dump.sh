#!/bin/sh
mkdir -p dumps
find obj -name \*.o | sed "s/obj\/\([^\.]*\)\.o/\\1/" | awk '{print "$DVPRO/devkitPPC/bin/powerpc-eabi-objdump -d -l --demangle=gnu -S obj/"$1".o > dumps/"$1".dmp"}' | sh
find obj -name \*.o | sed "s/obj\/\([^\.]*\)\.o/\\1/" | awk '{print "$DVPRO/devkitPPC/bin/powerpc-eabi-objdump -d --demangle=gnu obj/"$1".o > dumps/"$1".S"}' | sh
