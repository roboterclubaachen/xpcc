#include "../stm32f1_discovery.hpp"

using namespace Board;

//using systemClock = SystemClock< InternalClock<> >;
using systemClock = SystemClock< ExternalCrystal<MHz8> >;

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
