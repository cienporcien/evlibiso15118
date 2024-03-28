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


Author Roger Bedell, based on EVerest/libiso15118.


