Errata notes
============


## LCL_3 is left unconfigured
The LCL_3 pin is connected to the SETI pin of the Latchup current limiter.
If its configured as a GPIO output, it will pull SETI lowe, and trigger the LCL.
The voltage on SETI is directly proportional to the current on the LCL. We are 
backdriving it. Future revision will have it set to an DAC/ADC depending on desired use.