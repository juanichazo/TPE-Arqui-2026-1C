#!/bin/bash
qemu-system-x86_64 -hda Image/x64BareBonesImage.qcow2 -m 512 -machine pcspk-audiodev=speaker -audiodev pa,id=speaker,server=/mnt/wslg/PulseServer,out.buffer-length=5000
