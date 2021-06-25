# Tauba Auerbach

## Membrane Sculpture

A Solar powered kinetic sculpture that creates a membrane by drawing apart soaped ropes. The ropes are drawn apart, then brought back together in a cycle repeated for the duration of its exhibition.

Originally created for its debut exhibition at the ARtists Institute, this code is now being updated for exhibit at SFMoMA. Previously, setup and control was done on-site via a button matrix UI for the arduino, which was running the motion control and timing software.

The SFMoMA installation will eventually move to a single Raspberry Pi. In the transistion period, a bi-directional serial communication protocol will be developed so that adjustments to the sculpture can be made via a webapp running on a raspberry pi connected to the arduino fitted with the motordrivers. This configuration will be simplified to a single raspberry pi zero w fitted with a motor control shield. The raspberry pi will host a control interface accessible via the local network and VPN, and run the motion control software.


### Variables

```

```
