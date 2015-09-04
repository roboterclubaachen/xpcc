#include "../stm32f1_discovery.hpp"

using namespace Board;

//using pll = Pll< InternalClock<>, MHz24 >;
using pll = Pll< ExternalClock<MHz8>, 24000000 >;
using systemClock = SystemClock< pll >;

MAIN_FUNCTION
{
	Board::initialize();
	systemClock::enable();

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
