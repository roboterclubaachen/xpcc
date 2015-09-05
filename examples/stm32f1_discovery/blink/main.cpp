#include "../stm32f1_discovery.hpp"

using namespace Board;

//using pll = Pll< InternalClock<>, MHz24 >;
using pll = Pll< ExternalClock<MHz8>, 22000000 >;
using systemClock = SystemClock< pll >;

using clockOutput = ClockOutput< pll >;

MAIN_FUNCTION
{
	Board::initialize();
	systemClock::enable();

	clockOutput::enable();
	GpioOutputA8::connect(clockOutput::Id);

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
