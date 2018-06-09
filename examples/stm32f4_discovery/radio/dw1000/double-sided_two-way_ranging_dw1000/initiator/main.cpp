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
 * 1) Sends out a DSTW-Ranging Frame on Broadcast
 * 2) Waits for an Answer and Computes the Range between the both
 *
 * For 1 Ranging there will be 4 messages but this technique is more precise than SSTW
 *
 *
 * Init -> Resp |Payload{init DSTWR}
 * Resp -> Init |Payload{receive and transmittime of Resp}
 * Init -> Resp |Payload{receive and transmittime of Init}
 * Resp -> Init |Payload{Time of flight of second ranging round}
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
static constexpr uint16_t hostaddress= 0xBBBB;


xpcc::Frame802154 receiveframe;
xpcc::ShortTimeout timeout;
bool isrx = false;
uint32_t responserx, responsetx;
uint64_t owntx,ownrx;
double distance;
uint8_t buffer[1024];


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
	while (!(dwm::init(config)))
	{LedRed::set();}
	LedRed::reset();
	/* Apply antenna delay value.*/
	dwm::setRXAntennaDelay(RX_ANT_DLY);
	dwm::setTXAntennaDelay(TX_ANT_DLY);

	dwm::hostaddress = hostaddress;

	while (true)
	{
		LedBlue::set();

		//--------------------------------------- Start sending init ---------------------------------------------------------------
		XPCC_LOG_DEBUG << "Send init"<< xpcc::endl;
		ranging::sendDSTWRinit();
		timeout.restart(5);
		while(not(dwm::isFrameSent()||timeout.isExpired())){}
		LedBlue::reset();

		//------------------------------------------------------------ Receive Message--------------------------------------------------------------------
		timeout.restart(5);
		dwm::rxEnable();
		LedOrange::set();
		while(not(dwm::checkForRXError() || (isrx = dwm::checkForRX())  || timeout.isExpired())){}
		LedOrange::reset();
		if (isrx)
		{
			dwm::readrx(dwm::rxlength(),buffer);
			receiveframe.loadFrame(dwm::rxlength(),buffer);
			if (receiveframe.getDestinationAddress16() == hostaddress)
			{
				if (ranging::IsRangingFrame(receiveframe))
				{
					//--------------------------------------- Start get distance and send return ---------------------------------------------------------------
					distance = ranging::computeSsTwrDistance(receiveframe);
					dwm::rxdisable();
					ranging::sendAnswer(receiveframe);
					//---------------------------------------Wait for TOF from other device---------------------------------------------------------------------
					timeout.restart(5);
					while(not(dwm::isFrameSent()||timeout.isExpired())){}
					dwm::rxreset();
					dwm::rxEnable();
					isrx = false;
					timeout.restart(150);
					LedOrange::set();
					while(not(dwm::checkForRXError() || (isrx = dwm::checkForRX())  || timeout.isExpired())){}
					LedOrange::reset();
					if (isrx)
					{
						LedGreen::toggle();
						dwm::readrx(dwm::rxlength(),buffer);
						receiveframe.loadFrame(dwm::rxlength(),buffer);
						if (receiveframe.getDestinationAddress16() == hostaddress)
						{
							if (ranging::IsRangingFrame(receiveframe))
							{
								distance = ranging::computeDsTwrDistance(distance,receiveframe);
								XPCC_LOG_INFO.printf("DISTANCE TO %0x IS %5.2fm \n",receiveframe.getSourceAddress16(),distance);
								timeout.restart(5);
							}

						}
					}
				}
			}
			else
			{
				dwm::frame_seq_nb = receiveframe.getSequenceNumber() + 1 ;
				XPCC_LOG_DEBUG << "Wrong Answer --- Address was wrong"<< xpcc::endl;
			}
		}

		else if (dwm::checkForRXError())
		{
			XPCC_LOG_ERROR<< "RX--ERROR!"<< xpcc::endl;
			timeout.restart(10);
		}
		dwm::rxdisable();
		while(not(timeout.isExpired())){}

	}
}
