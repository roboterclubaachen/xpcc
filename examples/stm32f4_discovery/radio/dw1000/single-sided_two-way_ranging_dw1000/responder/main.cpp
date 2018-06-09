/* This is an Example application for one STM32F4 in combination with a Decawave DM1000 Chip
 * Connections are the following:
 *
 * USART:       STM32 Pin
 * TXD          -> PA2
 * RXD          -> PA3 (not used in this example)
 *
 * DM1000:
 * SPI:         STM32 Pin
 * Miso         -> PB15
 * Mosi         -> PC2
 * Clock        -> PB10
 *
 * RES          -> PA8
 * Chipselect   -> PE8
 * IRQ          -> PD10 (not used in this example)
 *
 * What this program does is:
 * 1) Waits for any ranging Frame on the channel and sends a corrosponding answer
*/


#include <xpcc/architecture/platform.hpp>					// include for platform
#include <xpcc/debug/logger.hpp>							// include for DW1000 Error/Debug Messaging
#include <xpcc/driver/radio/dw1000/dw1000.hpp>				// DW1000 Driver
#include <xpcc/communication/Frame802154/Frame802154.hpp>	// MAC Frame for IEEE 802.15.4
#include <xpcc/positioning/ranging.hpp>						// ranging operations for DW1000
#include <xpcc/processing/timer.hpp>						// Timer and Timeouts


// ------------------------------XPCC----------------------------------------------
xpcc::IODeviceWrapper< Usart2, xpcc::IOBuffer::BlockIfFull > loggerDevice;

// // Set all four logger streams to use the UART
xpcc::log::Logger xpcc::log::debug(loggerDevice);
xpcc::log::Logger xpcc::log::info(loggerDevice);
xpcc::log::Logger xpcc::log::warning(loggerDevice);
xpcc::log::Logger xpcc::log::error(loggerDevice);

//Namespaces and renaming
using namespace Board;
using SPI				= SpiMaster2;
using RES				= GpioOutputA8;
using CS				= GpioOutputE8;
using IRQ				= GpioInputD10;

using dwm				= xpcc::Dw1000 < SPI, CS, RES, IRQ >;
using ranging			= xpcc::Ranging < dwm >;


//------------------------------DWM Config------------------------------------------------

static xpcc::dw1000::config_t config =
{
	2,							/* Channel number. */
	xpcc::dw1000::PRF_64M,		/* Pulse repetition frequency. */
	xpcc::dw1000::PLEN_128,		/* Preamble length. Used in TX only. */
	xpcc::dw1000::PAC8,			/* Preamble acquisition chunk size. Used in RX only. */
	9,							/* TX preamble code. Used in TX only. */
	9,							/* RX preamble code. Used in RX only. */
	0,							/* 0 to use standard SFD, 1 to use non-standard SFD. */
	xpcc::dw1000::BR_6M8,		/* Data rate. */
	xpcc::dw1000::PHRMODE_STD,	/* PHY header mode. */
	(129 + 8 - 8)				/* SFD timeout (preamble length + 1 + SFD length - PAC size). Used in RX only. */
};


/* Default antenna delay values for 64 MHz PRF*/
static constexpr int TX_ANT_DLY = 16436;
static constexpr int RX_ANT_DLY = 16436;
static constexpr uint16_t hostaddress= 0xAAAA;


xpcc::Frame802154 receiveframe;
xpcc::ShortTimeout timeout;
uint8_t buffer[256];
bool isrx = false;
uint32_t length;


//-------------------------------MAIN-----------------------------------------------------
int
main()
{
	//setup USART
	GpioOutputA2::connect(Usart2::Tx);
	GpioInputA3::connect(Usart2::Rx, Gpio::InputType::PullUp);
	Usart2::initialize<Board::systemClock, 115200>(12);
	//initialize the board
	Board::initialize();
	//activate the CS on the DW1000
	CS::setOutput(xpcc::Gpio::High);
	//setup SPI
	GpioOutputB15::connect(SPI::Mosi);
	GpioInputC2::connect(SPI::Miso);
	GpioOutputB10::connect(SPI::Sck);
	SPI::initialize<Board::systemClock, 1312500ul>();
	SPI::setDataMode(SPI::DataMode::Mode0);
	//Init with the config
	while (!dwm::init(config))
	{LedRed::set();}
	LedRed::reset();
	/* Apply antenna delay value.*/
	dwm::setRXAntennaDelay(RX_ANT_DLY);
	dwm::setTXAntennaDelay(TX_ANT_DLY);

	dwm::hostaddress = hostaddress;


	while (true)
	{
		isrx = false;
		dwm::rxEnable();
		LedOrange::set();
		while(not(isrx = dwm::checkForRX() || dwm::checkForRXError()))
		{}
		LedOrange::reset();
		if (isrx)
		{
			length = dwm::rxlength();
			dwm::readrx(length, buffer);
			receiveframe.loadFrame(length, buffer);
			if (receiveframe.getDestinationAddress16() == hostaddress || receiveframe.getDestinationAddress16() == 0xFFFF)
			{
				LedGreen::set();
				if(ranging::IsRangingFrame(receiveframe))
				{
					ranging::sendAnswer(receiveframe);
					timeout.restart(5);
					while(not(dwm::isFrameSent() || timeout.isExpired()))
					{}

				}
				LedGreen::reset();
			}
			else
			{
				LedBlue::set();
				xpcc::delayMilliseconds(100);
				LedBlue::reset();
			}

		}
		else
		{
			LedRed::set();
			xpcc::delayMilliseconds(100);
			LedRed::reset();
		}
		dwm::trxdisable();
		dwm::rxreset();

	}
}
