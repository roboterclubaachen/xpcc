#include "../stm32f1_discovery.hpp"

using namespace Board;

//using pll = Pll< InternalClock, MHz24 >;
using pll = Pll< ExternalCrystal<MHz8>, 22000000 >;
using systemClock = SystemClock< pll, AhbPrescaler::Div8 >;
using clockOutput = ClockOutput< pll >;

static_assert(systemClock::Fcpu == 2750000, "CPU Frequency is not 22/8 MHz!");
static_assert(systemClock::Fcpu == pll::OutputFrequency / 8, "CPU Frequency is not 22/8 MHz!");

MAIN_FUNCTION
{
	Board::initialize();
	systemClock::enable();
	xpcc::cortex::SysTickTimer::initialize<systemClock>();

	clockOutput::enable();
	GpioOutputA8::connect(clockOutput::Id);

	GpioOutputA2::connect(Usart2::Tx);
	Usart2::initialize<systemClock, 115200>(8);

	LedBlue::set();

	while (1)
	{
		LedGreen::toggle();
		xpcc::delayMilliseconds(Button::read() ? 100 : 500);

		LedBlue::toggle();
		xpcc::delayMilliseconds(Button::read() ? 100 : 500);
	}

	return 0;
}
