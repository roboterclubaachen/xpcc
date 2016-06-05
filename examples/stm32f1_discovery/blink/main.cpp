#include <xpcc/architecture/platform.hpp>

// Choose a clock source.
// Note: The internal clock does not have a user provided frequency,
//       since its frequency is fixed!
//using pllSource = InternalClock;
using pllSource = ExternalCrystal<MHz8>;
// Choose the PLL output frequency.
// The pll values are automatically computed.
using pll = Pll< pllSource, 22000000 >;

// Choose clock input for system clock.
//using systemSource = InternalClock;
using systemSource = pll;
// Choose AHB prescalers, all resulting frequencies are automatically computed.
using systemClock = SystemClock< systemSource, AhbPrescaler::Div8 >;
// System clock configuration can be asserted at compile time.
static_assert(systemClock::Fcpu == 2750000,
	"CPU Frequency is not 22/8 MHz!");
static_assert(systemClock::Fcpu == pll::OutputFrequency / 8,
	"CPU Frequency is not 1/8 of PLL output frequency!");

// Choose the low speed clock source.
// using lsSource = LowSpeedInternalClock;
using lsSource = LowSpeedExternalCrystal;
// Choose clock input for RTC.
using rtcClock = RealTimeClock< lsSource >;
// Low speed clock configuration can also be asserted at compile time
static_assert(rtcClock::Rtc == 32768,
	"RTC frequency is not 32.768kHz!");

// Choose clock output source.
using clockOutput = ClockOutput< systemSource >;

int
main()
{
	// Enable the clocks and PLL.
	systemClock::enable();
	// Initialize the SysTickTimer with the chosen AHB frequency.
	xpcc::cortex::SysTickTimer::initialize<systemClock>();

	// Enable the low speed clocks.
	rtcClock::enable();

	// Connect Pin A8 to the clock output for measurement.
	GpioOutputA8::connect(clockOutput::Id);
	// Enable clock output with the chosen source clock.
	clockOutput::enable();


	GpioOutputA2::connect(Usart2::Tx);
	Usart2::initialize<systemClock, 115200>(8);

	Board::LedBlue::setOutput(xpcc::Gpio::High);
	Board::LedGreen::setOutput(xpcc::Gpio::Low);

	while (1)
	{
		Board::LedGreen::toggle();
		xpcc::delayMilliseconds(Board::Button::read() ? 100 : 500);

		Board::LedBlue::toggle();
		xpcc::delayMilliseconds(Board::Button::read() ? 100 : 500);
	}

	return 0;
}
