ISO 15118-20 EV library suite with ACDP
=======================

Currently, this is a proof of concept library. It runs against libiso15118 dash-20-poc-acdp branch. (iso15118-20 only).


This is an implementation of the EV side of ISO 15118-20 using cbexigen and the new framework built on top of cbexigen
generated EXI source code.
It is intended to replace Josev, since Josev is Python based, and has the issue with the EXI jar not being open source, so it could not
be changed or adapted to WPS or ACDP. As well, cbexigen is very transparent and easy to work with, and uses the Apache 2.0
license.

The EV side is sort of a mirror of the EVSE side, but is the master of the communication sequence instead of the slave.
cbexigen already has provided all the EV side messages in src/exi, so the exi isn't a problem.

The idea was to use more or less the same architecture as the EVSE side. Essentially, I reversed all Req to Res 
(and vice versa) in the state files, and added conversions in the reverse direction to the message files.

SDP had to be rewritten, and the state flow is slightly different, since the EV side is the initiator of all messages.

A new EVerest module, CbexiEV has been written as well to interface with the EVerest car simulator.

In this ACDP branch, the functionality for Automatic Connection Device Pantograph (ACDP) was added in the branch evlibiso15118_with_ACDP.
SDP for Wireless was also added.

To build both sides (EV and EVSE) of the ACDP implementation, you can do the following:
Tested on a clean Raspberry Pi 5, 4gb RAM

# Install the dev tools you need
sudo apt update
sudo apt install -y python3-pip git rsync wget cmake doxygen graphviz build-essential clang-tidy cppcheck openjdk-17-jdk npm docker docker-compose libboost-all-dev nodejs libssl-dev libsqlite3-dev clang-format curl rfkill libpcap-dev libevent-dev pkg-config libcap-dev

# The UWBPPD uses libserial - install this (should be in dependencies, but isn't yet)
sudo apt install libserial-dev

# Create a checkout directory in home
mkdir checkout
cd checkout

# Install the Everest Dependency Manager
git clone https://github.com/EVerest/everest-dev-environment
cd everest-dev-environment/dependency_manager
# Has to be sudo, otherwise it won't install correctly
sudo python3 -m pip install .
# If you get the dreaded "error: externally-managed-environment", you can do this hack, though apparently not recommended
sudo python3 -m pip install . --break-system-packages

# Create a new folder under checkout, i.e. everest_exi
# cd to that directory
cd everest_exi

# Clone everest-core from the fork in cienporcien/everest-core
git clone https://github.com/cienporcien/everest-core.git

# Clone the forked libiso15118 from cienporcien/libiso15118
git clone https://github.com/cienporcien/libiso15118.git

# Clone the evlibiso15118 from cienporcien/evlibiso15118
git clone https://github.com/cienporcien/evlibiso15118.git

# Select the correct branches for ACDP
cd everest-core
git checkout testing/iso15118-20-ACDP
cd ../libiso15118
git checkout dash-20-poc-acdp
cd ../evlibiso15118
git checkout evlibiso15118_with_ACDP

# From here build. There are two ways, with vscode (using ninja) or continue in a command line (using make)
# From the command line:
cd ../everest-core
mkdir build
cd build
cmake ..
# if you have plenty of RAM, make using -j to build faster:
# make -j$(nproc) install
# Note: on a 4gb rpi, using all 4 cores causes Raspbian (Raspberry Pi OS) to crash eventually, so use this instead though it takes much longer:
make install 


# note, don't build in the command line using make if you want to use vscode, since vscode uses ninja, and you will
# get an error if you don't build it within vscode.

# Open vscode if you want to use vscode 
# Open the everest-core folder
# Add the libiso15118 folder
# Add the evlibiso15118 folder

# Build everest-core in vscode using the vscode cmake. Note that you need to have the cmake tools, the correct toolchain selected etc.
# vscode is great for debugging, but needs at least 8GB RAM to run well, not so good on my 4gb rpi 5.

# This builds both the EV side and the EVSE side.
# If all you need is the EV side, then

vscode debugging configuration
{
				"name": "CbexiEV",
				"type": "cppdbg",
				"request": "launch",
				"program": "/home/opbrid/checkout/everest_exi_ev/everest-core/build/modules/CbexiEV/CbexiEV",
				"args": [
					"--config",
					"config-sil-EV-ACDP",
					"--module",
					"iso15118_car"
				],
				"stopAtEntry": false,
				"cwd": "/home/opbrid/checkout/everest_exi",
				"environment": [],
				"externalConsole": false,
				"MIMode": "gdb",
				"setupCommands": [
					{
						"description": "Enable pretty-printing for gdb",
						"text": "-enable-pretty-printing",
						"ignoreFailures": true
					},
					{
						"description": "Set Disassembly Flavor to Intel",
						"text": "-gdb-set disassembly-flavor intel",
						"ignoreFailures": true
					},
					{ "text": "set output-radix 16" }
				]
			}

Author Roger Bedell, based on EVerest/libiso15118.


