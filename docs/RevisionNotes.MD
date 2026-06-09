Changes requested for the supervisor
====================================

UART
----

1. Signal `MCU_RS422_EN` is used to enable the transmission on the RS422 converter. Not needed.
2. Flow control signals (RTS CTS) would be usefull for communication with the CPU.


Cristal Oscillators
-------------------

The main idea behind the cristal oscillators is to enable a real time clock, but the design 
seems to be connected the external clock inputs.
Maybe the designers confused RCC with RTC?
> TODO: check the design.


RTC
---

> TODO: How to use the external clocks to drive it?

Latching current limiter
------------------------

1. pin LCL_4 is not connected to anything important, is just a test point.
2. PIN LCL_3 should be connected to the ADC, for measuring the exact value on the current limiter. Not sure if this is actually necessary, since the value is set by resistor
	NOTE: Configuring this pin on MX will cause it to pull the line low, and instead of using SETI
	to measure the current, it will set the current on the LCL to zero.
3. pin LCL_2 is connected to `FLAG which is asserted when there is a high current (logical value 1?). Add this to a ALARM
4. Overall those  pins need a more descriptive name.



