# Clock Driver Interface

### Clock Sources

On almost every controller supported by XPCC there are three different clock
sources:

1. **InternalClock**: the clock the controller runs on at startup
2. **ExternalClock**: an active external clock source
3. **ExternalCrystal**: an external crystal oscillator that need to be
  driven by the controller

Please note that for example on the STM32 platform the _ExternalClock_ and the
_ExternalCrystal_ both use the same hardware. Nonetheless the two separate
classes need to implemented in order to provide a common interface.

### PLL

An important part of every clock tree is a Pll. These are used to multiply
an input frequency. Thus the need to be connected to a Clock Source and to
be enabled with a target frequency as an argument. If there is more than one
Pll on your system, they should be called: Pll, Pll2, Pll3 etc.
