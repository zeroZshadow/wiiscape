#!/usr/bin/env python

import os
import socket
import subprocess
import filecmp

# Create dolphin process
devnull = open(os.devnull, 'wb')
p = subprocess.Popen(['tools/Emulator/dolphin.exe', '/b', '/e ../../wiiscape.elf'], stdout=devnull, stderr=devnull)

# Create and connect to Dolphin's USBGekko
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(("127.0.0.1", 55020))

# Try to read frame time
string = ""
while 1:
	data = s.recv(1024)
	if not data:
		break
	string += data
	if data.endswith("\n"):
		break

# Print reference time and quit dolphin
print string.strip()
s.close()
p.terminate()

# Get last saved frame
dirpath = "tools/Emulator/User\Dump/Textures/00000000"
reference = "tools/Emulator/User\\Dump/Textures/00000000/reference.png"

entries = (os.path.join(dirpath, fn) for fn in os.listdir(dirpath))
entries = ((os.stat(path), path) for path in entries)
filestats = [ (i, f) for i,f in entries if f.endswith('.png') ]
files = [ f for i,f in sorted(filestats) ]

lastframe = files[len(files)-1]

# Check if its content is the same as the reference one
print "Frame content is OK" if filecmp.cmp(reference, lastframe) else "[WARN] Output is different!"