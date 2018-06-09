/* This is an Example application for two STM32F4 in combination with a Decawave DM1000 Chip
 * Flash this App on both sides
 * Connections are the following:
 *
 * USART:       STM32 Pin
 * TXD          -> PA1
 * RXD          -> PA2 (not used in this example)
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
 * 1) Send a broadcast message with the word "PING" to all nearby UWB receivers configured with the same Parameters (Orange LED)
 * 2) Receive 3 Seconds long all messages  (Blue LED)
 * 3) If a Message was received, check if it is a Broadcasted Message and if its Payload contains "PING"
 *  -> 4a) If both is true answer the message with a "PONG"(Green LED)
 *      -> 5a) Goto (2)
 * 4b) If its Payload contains "PONG" wait 100ms and Goto (1) (Green LED)
 * 4c) If a Message was received and it includes any other payload than "PING" or "PONG" Goto (1) (Red LED)
 * 4d) If nothing was received goto (1)
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

static constexpr uint16_t hostaddress= 0xABCD;


//------------------------------Variables--------------------------------------------
xpcc::Frame802154 sendframe;
xpcc::Frame802154 receiveframe;
xpcc::ShortTimeout timeout1,timeout2,sendtimeout;

uint8_t buffer[256];
uint8_t initpayload[5] = {0xFF,'P','I','N','G'};
uint8_t resppayload[5] = {0xEF,'P','O','N','G'};

//------------------------------Helper Function---------------------------------------
void presetsendframe() // This Function sets up the sendframe
{
    sendframe.setControl(0x8841);                   // 16Bit Addresses short PAN DATA
    sendframe.setSequenceNumber(dwm::frame_seq_nb); //Set SequenceNumber
    sendframe.setDestinationPANAddress(0xDECA);     //Set PANAddress
    sendframe.setDestinationAddress16(0xFFFF);      //Set destinationaddress
    sendframe.setSourceAddress16(hostaddress);      //Set Sourceaddress to own address
}

//-------------------------------MAIN-----------------------------------------------------
int
main()
{
	Board::initialize();
	//Configure USART
	GpioOutputA2::connect(Usart2::Tx);
	GpioInputA3::connect(Usart2::Rx, Gpio::InputType::PullUp);
	Usart2::initialize<Board::systemClock, 115200>(12);

	XPCC_LOG_DEBUG   << xpcc::endl << xpcc::endl << xpcc::endl << "-----------------Simple TX RX v1.1 *mja*-----------------"   << xpcc::endl << xpcc::endl;


	CS::setOutput(xpcc::Gpio::High);    //activate the CS on the DW1000
	//setup SPI
	GpioOutputB15::connect(SPI::Mosi);
	GpioInputC2::connect(SPI::Miso);
	GpioOutputB10::connect(SPI::Sck);
	SPI::initialize<Board::systemClock, 1312500ul>();
	SPI::setDataMode(SPI::DataMode::Mode0);

	while (!dwm::init(config)) //Init with the config
	{};
	/* Apply antenna delay value.*/
	dwm::setRXAntennaDelay(RX_ANT_DLY);
	dwm::setTXAntennaDelay(TX_ANT_DLY);
	presetsendframe(); // set the sendframe given to the function below
	timeout2.restart(0); // set TO2 to expired

	while (true) //-----------MAINLOOP--------------
	{
		//  --- Send Message
		if (timeout2.isExpired())
		{
			LedOrange::toggle();
			sendframe.setSequenceNumber(dwm::frame_seq_nb); //add actual framesequence numper to sendframe
			sendframe.setPayload(5,initpayload); // set init payload {0xFF,'P','I','N','G'} for sendframe
			sendframe.addPayload(1,&dwm::frame_seq_nb); // add sequencenumber to payload (Testing)
			sendframe.getFrame(buffer); // load sendframe to buffer
			dwm::send(sendframe.length,buffer); // send frame over DW1000
			sendtimeout.restart(5);
			while(!dwm::isFrameSent()||sendtimeout.isExpired())
			{} //wait until the DW1000 sent the frame
			XPCC_LOG_DEBUG << "----------------------------Send Init-Frame---------------------   "<< xpcc::endl;
			dwm::frame_seq_nb++; //increase the sequence number
			LedOrange::toggle();
		}
		// Receive Message
		timeout1.restart(3000);
		dwm::rxEnable(); // Einable the reception of the DW1000
		LedBlue::toggle();
		while(not(dwm::checkForRX() || timeout1.isExpired() || dwm::checkForRXError()))
		{} //wait until frame received/rxerror or
		LedBlue::toggle();
		if (dwm::checkForRX())
		{
			dwm::readrx(dwm::rxlength() ,buffer); //get the received data into the buffer
			receiveframe.loadFrame(dwm::rxlength(), buffer); //write the buffer
			if (receiveframe.getDestinationAddress16() == hostaddress || receiveframe.getDestinationAddress16() == 0xFFFF)
			{
				if (receiveframe.payloadlength>0) {
					receiveframe.getPayload(receiveframe.payloadlength , buffer);
					XPCC_LOG_DEBUG << xpcc::endl << "----------------Received Frame-------------"<< xpcc::endl;
					receiveframe.debugToString();
					if (buffer[0] == 0xFF)
					{
						LedGreen::toggle();
						timeout2.restart(500);
						dwm::frame_seq_nb = receiveframe.getSequenceNumber()+1;
						sendframe.setSequenceNumber(dwm::frame_seq_nb);
						sendframe.setPayload(5,resppayload);
						sendframe.addPayload(1,&dwm::frame_seq_nb);
						sendframe.getFrame(buffer);
						XPCC_LOG_DEBUG << xpcc::endl << "-----------------Send Response-Frame------------------  "<< xpcc::endl;
						dwm::send(sendframe.length,buffer);
						sendtimeout.restart(5);
						while(!dwm::isFrameSent()||sendtimeout.isExpired())
						{} //wait until the DW1000 sent the frame
						dwm::frame_seq_nb++;
						LedGreen::toggle();
					}
					else
					{
						LedGreen::toggle();
						dwm::frame_seq_nb = receiveframe.getSequenceNumber()+1;
						xpcc::delayMilliseconds (100);
						LedGreen::toggle();
					}
				}
			}
		}
		else if (dwm::checkForRXError())
		{
			LedRed::toggle();
			XPCC_LOG_DEBUG << xpcc::endl << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!RX-ERROR!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!11"<< xpcc::endl;
			timeout1.restart(0);
			timeout2.restart(0);
			LedRed::toggle();
		}
		timeout1.restart(0);
		dwm::trxdisable();
		dwm::rxreset();
	}
}
