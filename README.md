ISO 15118 EV library suite
=======================

Currently, this is a proof of concept library.  Therefore it is located
in the branch `dash-20-dash-20-poc`.

RB:
This is a new implementation of the EV side of ISO 15118-2 and -20 using cbexigen and the new framework built on top of cbexigen.

I started with a copy of everest LIBISO15118 that handles both -20 and -2 on the EVSE side

The EV side is sort of a mirror of the EVSE side, but is the master of the communication sequence instead of the slave.
cbexigen already has provided all the EV side messages in src/exi, so exi isn't a problem.

The idea is to use more or less the same architecture as the EVSE side. Not sure how this will pan out..