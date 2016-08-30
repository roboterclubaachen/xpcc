#include <xpcc/architecture/platform.hpp>

// Use SPI 2
using SpiMaster = SpiMaster2;

int
main()
{
	Board::initialize();

	// Enable SPI
	GpioOutputB12::connect(SpiMaster::Nss);
	GpioOutputB15::connect(SpiMaster::Mosi);
	GpioInputB14::connect(SpiMaster::Miso);
	GpioOutputB13::connect(SpiMaster::Sck);
	SpiMaster::initialize<Board::systemClock, MHz20>();

	while (1)
	{
		SpiMaster::transferBlocking(0xF0);
	}

	return 0;
}
