/* This is an Example application for one STM32 in combination with a Decawave DM1000 Chip
*
* Connections are the following:
*
* USART:       STM32F4&F103 Pin
* TXD          -> PA2
* RXD          -> PA3 (not used in this example)
*
* DM1000 -> SPI-Master-2
* SPI:		 STM32F4 Pin	   STM32F107
* Miso         -> PC02		   PB14
* Mosi         -> PB15		   PB15
* Clock        -> PB10		   PB13
*
* RES          -> PA08		   PA00 (not used in this example)
* Chipselect   -> PE08		   PB12
* IRQ          -> PD10		   PB08(not used in this example)
*
*
* This Program performs a trilateration with three Anchor Points
* For this a double sided two way ranging is done with every Anchor
* With the results it estimating the X and Y Postition for the Tag.
* Additionally a Residuum is given wich can be interpreted as position on the Z-axis or the error of the ranging
*
*
*/


#include <xpcc/architecture/platform.hpp>					// include for platform
#include <xpcc/debug/logger.hpp>							// include for DW1000 Error/Debug Messaging
#include <xpcc/driver/radio/dw1000/dw1000.hpp>			// DW1000 Driver
#include <xpcc/communication/Frame802154/Frame802154.hpp>	// MAC Frame for IEEE 802.15.4
#include <xpcc/positioning/ranging.hpp>						// ranging operations for DW1000
#include <xpcc/positioning/multilateration.hpp>				// trilateration operations for DW1000
#include <xpcc/processing/timer.hpp>						// Timer and Timeouts


//-------------------------Namespaces and renaming----------------------------------
// change things up for each individual board
using namespace Board;
using SPI		=	SpiMaster2;
using SPISCK	=	GpioOutputB10;
using SPIMISO	=	GpioInputC2;
using SPIMOSI	=	GpioOutputB15;
using RES		=	GpioOutputA8;
using CS		=	GpioOutputE8;
using IRQ		=	GpioInputD10;


using dwm = xpcc::Dw1000 < SPI, CS, RES, IRQ >;
using ranging = xpcc::Ranging < dwm >;

//------------------------------LOGGER----------------------------------------------
#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::DEBUG
xpcc::IODeviceWrapper< Usart2, xpcc::IOBuffer::BlockIfFull > loggerDevice;

// Set all four logger streams to use the UART
xpcc::log::Logger xpcc::log::debug(loggerDevice);
xpcc::log::Logger xpcc::log::info(loggerDevice);
xpcc::log::Logger xpcc::log::warning(loggerDevice);
xpcc::log::Logger xpcc::log::error(loggerDevice);



/*
*	The Following Variables are subject of being changed.
*	Please fit them individually for your setup!
*	Each anchor needs its unique Position and Hostname else any trilateration try will fail
*/
static constexpr uint16_t AnchorAdresses[] = {0xA000, 0xA001,0xA003};
static constexpr uint16_t hostaddress= 0xAACD;
floatunit distanceToAnchor[3]				=	{0.0, 0.0, 0.0};
xpcc::Vector<floatunit,3> anchor0Position	=	{0.0, 0.0, 0.0};
xpcc::Vector<floatunit,3> anchor1Position	=	{2.0, 0.0, 0.0};
xpcc::Vector<floatunit,3> anchor2Position	=	{0.0, 2.0, 0.0};
xpcc::Vector<floatunit,3> position			=   {0.0, 0.0, 0.0};

//------------------------------DWM Config------------------------------------------------

static xpcc::dw1000::config_t config =
{
	2,								/* Channel number. */
	xpcc::dw1000::PRF_64M,         /* Pulse repetition frequency. */
	xpcc::dw1000::PLEN_128,        /* Preamble length. Used in TX only. */
	xpcc::dw1000::PAC8,            /* Preamble acquisition chunk size. Used in RX only. */
	9,								/* TX preamble code. Used in TX only. */
	9,								/* RX preamble code. Used in RX only. */
	0,								/* 0 to use standard SFD, 1 to use non-standard SFD. */
	xpcc::dw1000::BR_6M8,			/* Data rate. */
	xpcc::dw1000::PHRMODE_STD,		/* PHY header mode. */
	(129 + 8 - 8)					/* SFD timeout (preamble length + 1 + SFD length - PAC size). Used in RX only. */
};

uint8_t anchor = 0;
xpcc::Frame802154 receiveframe;
xpcc::ShortTimeout timeout;
floatunit distance;
int counter = 0 ;
bool isrx = false;
uint8_t buffer[256];

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
	XPCC_LOG_INFO << "GENERIC Trilateration INIT v1.1"<< xpcc::endl;
	//activate the CS on the DW1000
	CS::setOutput(xpcc::Gpio::High);
	//setup SPI
	SPIMOSI::connect(SPI::Mosi);
	SPIMISO::connect(SPI::Miso);
	SPISCK::connect(SPI::Sck);
	SPI::initialize<Board::systemClock,2500000,xpcc::Tolerance::DontCare>();
	SPI::setDataMode(SPI::DataMode::Mode0);
	//Init with the config
	while (!(dwm::init(config)))
	{
		Board::Leds::toggle();
		xpcc::delayMilliseconds(100);
	}
	/* Apply antenna delay value.*/
	dwm::setRXAntennaDelay(dwm::standardAntennaDelay);
	dwm::setTXAntennaDelay(dwm::standardAntennaDelay);
	dwm::hostaddress = hostaddress;

	while (true)
	{
		//--------------------------------------- Start sending init ---------------------------------------------------------------
		//XPCC_LOG_DEBUG << "Send init"<< xpcc::endl;
		if (dwm::isChannelFree())
		{
			ranging::sendDSTWRinitAt(AnchorAdresses[anchor]);
			timeout.restart(5);
			while(not(dwm::isFrameSent()||timeout.isExpired()))
			{}
			//------------------------------------------------------------ Receive Message--------------------------------------------------------------------
			timeout.restart(10);
			dwm::rxEnable();
			while(not(dwm::checkForRXError() || (isrx = dwm::checkForRX())  || timeout.isExpired()));
			if (isrx)
			{
				dwm::readrx(dwm::rxlength(),buffer);
				receiveframe.loadFrame(dwm::rxlength(),buffer);
				if (receiveframe.getDestinationAddress16() == hostaddress)
				{
					if (ranging::IsRangingFrame(receiveframe))
					{
						//---------------------------------------Start get distance and send return ---------------------------------------------------------------
						distance = ranging::computeSsTwrDistance(receiveframe);
						ranging::sendAnswer(receiveframe);
						//---------------------------------------Wait for TOF from other device--------------------------------------------------------------------
						timeout.restart(5);
						while(not(dwm::isFrameSent()||timeout.isExpired()))
						{}
						dwm::rxEnable();
						isrx = false;
						timeout.restart(4);
						while(not(dwm::checkForRXError() || (isrx = dwm::checkForRX())  || timeout.isExpired()))
						{}
						//---------------------------------------Getting last ranging and compute distance---------------------------------------
						if (isrx)
						{
							dwm::readrx(dwm::rxlength(),buffer);
							receiveframe.loadFrame(dwm::rxlength(),buffer);
							if (receiveframe.getDestinationAddress16() == hostaddress)
							{
								if (ranging::IsRangingFrame(receiveframe))
								{
									distance = ranging::computeDsTwrDistance(distance,receiveframe);
									distanceToAnchor[anchor] = distance;
								}
							}
						}
						else if (dwm::checkForRXError())
						{
							XPCC_LOG_ERROR.printf ("RX--ERROR! with %x\n",AnchorAdresses[anchor]) ;

						}
						else
						{
							XPCC_LOG_DEBUG.printf ("Timeout with %x\n",AnchorAdresses[anchor]);
						}
					}
				}
				else
				{
					dwm::frame_seq_nb = receiveframe.getSequenceNumber() + 1 ;
					XPCC_LOG_INFO.printf ("Wrong Answer --- Address was wrong from %x\n",AnchorAdresses[anchor]);
				}
			}

			else if (dwm::checkForRXError())
			{
				XPCC_LOG_ERROR.printf ("RX--ERROR! with %x\n",AnchorAdresses[anchor]) ;
				timeout.restart(10);
			}
			else
			{
				XPCC_LOG_DEBUG.printf ("Timeout with %x\n",AnchorAdresses[anchor]);
			}
			// Increase the anchor iteration Var
			if (anchor < 2) {
				anchor++;}
			// If anchor iteration var == 2 compute trilateration
			else{
				xpcc::multilateration::trilateration(position,anchor0Position,anchor1Position,anchor2Position,distanceToAnchor[0],distanceToAnchor[1],distanceToAnchor[2]);
				XPCC_LOG_INFO.printf ("Distance A000:%.2f , A001: %.2f , A003: %.2f \n",distanceToAnchor[0],distanceToAnchor[1],distanceToAnchor[2]);
				XPCC_LOG_INFO.printf ("The actual Position is: X: %.2f , Y: %.2f  Residuum: %.2f \n",position[0],position[1],position[2]);
				anchor = 0;
				timeout.restart(200);

			}
			dwm::rxdisable();
		}
		else {timeout.restart(40);}
		while(not(timeout.isExpired()))
		{}


	}
}
