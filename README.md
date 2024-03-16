ISO 15118 EV library suite
=======================

Currently, this is a proof of concept library. It runs against libiso15118 dash-20-poc branch. (iso15118-20 only).


This is a new implementation of the EV side of ISO 15118-20 using cbexigen and the new framework built on top of cbexigen
generated EXI source code.
It is intended to replace Josev, since Josev has the issue with the EXI jar not being open source, so it could not
be changed or adapted to WPS or ACDP. As well, cbexigen is very transparent and easy to work with, and uses the Apache 2.0
license.

The EV side is sort of a mirror of the EVSE side, but is the master of the communication sequence instead of the slave.
cbexigen already has provided all the EV side messages in src/exi, so the exi isn't a problem.

The idea was to use more or less the same architecture as the EVSE side. Essentially, I reversed all Req to Res 
(and vice versa) in the state files, and added conversions in the reverse direction to the message files.

SDP had to be rewritten, and the state flow is slightly different, since the EV side is the initiator of all messages.

Author Roger Bedell, based on libiso15118.


