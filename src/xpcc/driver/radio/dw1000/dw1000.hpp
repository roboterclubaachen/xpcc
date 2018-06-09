/**
 * Copyright (c) 2017, Marten Junga (Github.com/Maju-Ketchup)
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 *
*/


/*
 * This API is addded on top of the DWM1000 API by DECAWAVE
 * I added some 'easier to use functions'
 * and set all others to privat *mja*
 *
 * DONE --> translating to C++, easy-to-use functions without register calls
 * WIP  --> move static constexpr in class part?! -> create enums
 * TODO --> XPCC integration, IRQ, Cleanup
*/
#ifndef _DW1000_
#define _DW1000_
#ifndef XPCC__LOGGER_HPP
#include <xpcc/debug/logger.hpp>
#ifndef XPCC_LOG_LEVEL
	#define	XPCC_LOG_LEVEL xpcc::log::DISABLED
#endif
#endif
#include "./params.hpp"
namespace xpcc
{
//-----Without those at this place there will be unfixable bugs (cannot put them into params.hpp) -  if anyone knows why pls change
//-----------------------------------------


//------------------------------------------------------- CLASS BEGIN -----------------------------------------------------------------------

/**
	*@brief
	*This class is a driver class for a Decawave Dw1000 Ultra-Wide-Band transmitter/receiver
	*
	*It needs: \n
	*SPI for communication with the host Microcontroler \n
	*Chipselect GPIO \n
	*Reset GPIO \n
	*Interrupt Request Line GPIO \n
	*@ingroup	dw1000
	*@author	Marten Junga
	*/
template < typename Spi, typename Cs, typename Reset, typename Irq >
class
		Dw1000 : public xpcc::dw1000
{
public:

	static dw1000::local_data_t dw1000local;

	static uint8_t frame_seq_nb; /**< The pending Framenumber (init value 0)	*/

	static uint64_t hostaddress; /**< The Hostaddress (init value 0xFFFFFFFFFFFFFFFF)	*/

	static uint32_t TX_ANT_DLY; /**< Sending Antenna Delay (init to standart: 16436)	*/

	static uint32_t RX_ANT_DLY; /**< Receiving Antenna Delay (init to standart: 16436)	*/
	//----------Boot----------------------------------------------------------------------------------

	/**
	*This function initializes the controller with an config_t returns true if startup was successful (include configure)
	*@param conf
	*/
	static bool
	init(config_t &conf);

	static void
	/**
	*@brief
	*This function configures the DWM with a given config_t for configuration changes while running
*
	*@param conf
	*/
	configure(config_t *conf);
	static void
	/**
	*@brief
	*This function sets the receive antenna delay standart 16436
	*@param rxDelay
	*/
	setRXAntennaDelay(uint16_t rxDelay);

	static void
	/**
	*@brief
	*This function sets the transmit antenna delay 16436
	*@param rxDelay
	*/
	setTXAntennaDelay(uint16_t rxDelay);


	//----------------Sending

	static bool
	/**
	*@brief
	*This function sends 'size' bytes of the given data array
	*@param size
	*minimum 3 maximim 1023
	*@param data
	*@return bool
	*returns if the send was successfull
	*/
	send(int size, uint8_t data[]);

	static bool
	/**
	*@brief
	*This function will send the given data at a specific point in time of the DW1000 clock
	*@param size
	*@param data
	*@param time
	*must be in the future of the DW1000 clock else send will not be successfull
	*@return bool
	*returns if the send was successfull
	*/
	sendAt(int size, uint8_t data[], uint32_t time);

	static bool
	/**
	*@brief
	*This function returns true if the frame was sent
	*@return bool
	*/
	isFrameSent();

	static void
	/**
	*@brief
	*This function reads the txtimestamp
	*@param timestamp[]
	*must has at least length 5
	*/
	readTXTimestamp(uint8_t timestamp[]); // 40 bit register

	static uint64_t
	/**
	*@brief
	* This function Reads the whole 40Bit register into an 64bit Integer
*
	*@return uint64_t
	*/
	readTXTimestamp64();

	static uint32_t
	/**
	*@brief
	*This function reads the low 32bit of the tx time stamp
	*@return uint32_t
	*/
	readTXTimestamplo32();

	static uint32_t
	/**
	*@brief
	*This function reads the high 32bit of the tx time stamp
	*@return uint32_t
	*/
	readTXTimestamphi32();

	static bool
	/**
	*@brief
	*This function listens 1 ms to the channel if the channel is free it returns true if it is busy false
	*@return bool
	*/
	isChannelFree();




	//----------------------Receiving
	static void
	/**
	*@brief
	*This function enables the receiver. -- Note that sending must be completed first
	*/
	rxEnable();

	static bool
	/**
	*@brief
	*This function enables  the receiver at the given time of the DWM1000
	*@param time
	*@return bool
	*/
	rxEnableAt(uint32_t time);



	static bool
	/**
	*@brief
	*This function returns true if a frame was received
	*@return bool
	*/
	checkForRX();

	static bool
	/**
	*@brief
	*This function returns true if there was an RX-Error
	*@return bool
	*/
	checkForRXError();

	static uint16_t
	/**
	*@brief
	*This function returns the length of the received frame
	*@return uint16_t
	*/
	rxlength();

	static void
	/**
	*@brief
	*This function fills the buffer with [size] bytes of the received frame
	*@param size
	*@param buffer
	*/
	readrx(uint32_t size,uint8_t buffer[]);

	static void
	/**
	*@brief
	*This function turns off the receiver and soft-resets the receiver register
	*/
	rxdisable();

	static void
	/**
	*@brief
	*This function reads the 40 bit register into the given buffer - length needs to be at least 5
	*@param timestamp[]
	*/
	readRXTimestamp(uint8_t timestamp[]);

	static uint64_t
	/**
	*@brief
	*This function reads 40 bit register into 64Bit Integer
	*@return uint64_t
	*/
	readRXTimestamp64();

	static uint32_t
	/**
	*@brief
	*This function reads the low 32bits of the rxtimestamp
	*@return uint32_t
	*/
	readRXTimestamplo32(void);

	static uint32_t
	/**
	*@brief
	*This function reads the high 32bits of the rxtimestamp
	*@return uint32_t
	*/
	readRXTimestamphi32(void);

	static void
	/**
	*@brief
	*This function sets an timeout for how long the DWM will try to receive data.
	*If 0 only rxdisable() will stop the dwm from receiving
	*@param time
	*/
	setrxtimeout(uint16_t time);

	static void
	/**
	*@brief
	*This function resets the receiver
	*/
	rxreset(void);


	//-----------------------Common receive and transmit functions

	static void
	/**
	*@brief
*
	*/
	enableExternalSync();

	static void
	/**
	*@brief
	*This function stops any transmit and receive
	*/
	trxdisable();

	static void
	/**
	*@brief
	* This function set starttime for the next transmit/receive
	*@param starttime
	*/
	setdelayedtrxtime(uint32_t starttime);

	//IRQ Handling

	/**
	*@brief
	*This function turns on and off IRQs when Frames were received
	*/
	static void toggleReceiveIRQ();

	/**
	*@brief
	*This function turns on and off IRQs when Frames were send
	*/
	static void toggleSendIRQ();

	/**
	*@brief
	*This function turns on and off IRQs when a Sync signal was received
	*/
	static void toggleSyncIRQ();

	/**
	*@brief
	*Returns the following:
	*RX_Complete = 1,
	*TX_Complete = 2,
	*RX_Error = 3,
	*Sync = 4,
	*Unknown = 0
	*@return IRQreason
	*/
	static IRQreason getIRQReason();



	//Other Stuff
	/**
	*@brief
	*
	*@return uint32_t
	*/
	static uint32_t readStatusRegister();

	/**
	*@brief
	*
	*@return uint32_t
	*/
	static uint32_t readStatusMRegister();

	/**
	*@brief
	*
	*@return uint32_t
	*/
	static void setCrystaltrim(uint8_t trim);



	//Debug
	/**
	*@brief
	* This function displays the statusregister for Debugging in the logstream
	*/
	static void displayStatusRegister();

	/**
	*@brief
	*This function displays the mask statusregister for Debugging in the logstream -- for IRQ
	*/
	static void displayMaskRegister();


	static void
	/**
	*@brief
*
	*@param timestamp[]
	*/
	readsystime(uint8_t timestamp[]);

	static uint32_t
	/**
	*@brief
*
	*@return uint32_t
	*/
	readsystimestamphi32(void);

	static uint32_t
	/**
	*@brief
*
	*@return uint32_t
	*/
	readsystimestamplo32(void);

	//config stuff
	static constexpr uint32_t DEVICE_ID  = 0xDECA0130;
	static constexpr int FORCE_SYS_XTI	= 0;
	static constexpr int XTRIM_ADDRESS	= 0x1E;
	static constexpr int LDOTUNE_ADDRESS= 0x04;
	static constexpr int PARTID_ADDRESS = 0x06;
	static constexpr int LOTID_ADDRESS	= 0x07;
	static constexpr int ENABLE_ALL_SEQ = 1;
	//init stuff
	static constexpr int FORCE_LDE		= 14;
	static constexpr int FORCE_SYS_PLL	= 2;
	static constexpr int READ_ACC_ON	= 7;
	static constexpr int READ_ACC_OFF	= 8;
	static constexpr int FORCE_OTP_ON	= 11;
	static constexpr int FORCE_OTP_OFF	= 12;
	static constexpr int FORCE_TX_PLL	= 13;

	//----------------------------------------PRIVATE-STUFF------------------------------------------------------------------------------------------------------------
private:


	static int
	/**
	*@brief
*
	*@param mode
	*@return int
	*/
	rxenable(RX_Mode mode);

	static int
	initialise(uint16_t conf);

	static int
	/**
	*@brief
*
	*@param headerLength
	*@param headerBuffer
	*@param bodylength
	*@param bodyBuffer
	*@return int
	*/
	writetospi(uint16_t headerLength, const uint8_t *headerBuffer, uint32_t bodylength, const uint8_t *bodyBuffer);

	static int
	/**
	*@brief
	*
	*@param headerLength
	*@param headerBuffer
	*@param readlength
	*@param readBuffer
	*@return int
	*/
	readfromspi(uint16_t headerLength, const uint8_t *headerBuffer, uint32_t readlength, uint8_t *readBuffer);

	static uint32_t
	/**
	*@brief
	*
	*@return uint32_t
	*/
	readdevid();

	static void
	/**
	*@brief
	*
	*@param value
	*/
	setxtaltrim(uint8_t value);

	static void
	/**
	*@brief
	*
	*/
	softreset();

	static int
	/**
	*@brief
	*
	*@param txFrameLength
	*@param txFrameBytes
	*@param txBufferOffset
	*@return int
	*/
	writetxdata(uint16_t txFrameLength, uint8_t *txFrameBytes, uint16_t txBufferOffset);

	static void
	/**
	*@brief
	*
	*@param txFrameLength
	*@param txBufferOffset
	*@param ranging
	*/
	writetxfctrl(uint16_t txFrameLength, uint16_t txBufferOffset, int ranging);

	static void
	/**
	*@brief
	*
	*@param regFileID
	*@param regOffset
	*@param regval
	*/
	write8bitoffsetreg(int regFileID, int regOffset, uint8_t regval);

	static int
	/**
	*@brief
	*
	*@param mode
	*@return int
	*/
	starttx(TX_Mode mode);

	static void
	/**
	*@brief
	*
	*@param buffer
	*@param length
	*@param rxBufferOffset
	*/
	readrxdata(uint8_t *buffer, uint16_t length, uint16_t rxBufferOffset);

	static uint32_t
	/**
	*@brief
	*
	*@param addr
	*@return uint32_t
	*/
	read32bitreg(uint16_t addr);

	static uint16_t
	/**
	*@brief
	*
	*@param regFileID
	*@param regOffset
	*@return uint16_t
	*/
	read16bitoffsetreg(int regFileID,int regOffset);

	static uint32_t
	/**
	*@brief
	*
	*@param regFileID
	*@param regOffset
	*@return uint32_t
	*/
	read32bitoffsetreg(int regFileID, int regOffset);

	static void
	/**
	*@brief
	*
	*@param recordNumber
	*@param index
	*@param length
	*@param buffer
	*/
	writetodevice(uint16_t recordNumber, uint16_t index, uint32_t length , const uint8_t *buffer);

	static void
	/**
	*@brief
	*
	*@param regFileID
	*@param regOffset
	*@param regval
	*/
	write16bitoffsetreg(int regFileID,int regOffset,uint16_t regval);

	static void
	/**
	*@brief
	*
	*@param regFileID
	*@param regOffset
	*@param regval
	*/
	write32bitoffsetreg(int regFileID, int regOffset, uint32_t regval);

	static void
	/**
	*@brief
	*
	*@param x
	*@param y
	*/
	write32bitreg(int x, uint32_t y);

	static void
	/**
	*@brief
*
	*@param recordNumber
	*@param index
	*@param length
	*@param buffer
	*/
	readfromdevice(uint16_t  recordNumber, uint16_t  index, uint32_t  length, uint8_t   *buffer);

	static uint64_t
	/**
	*@brief
	*
	*@return uint64_t
	*/
	get_rx_timestamp_u64(void);

	static uint8_t
	/**
	*@brief
	*
	*@param regFileID
	*@param regOffset
	*@return uint8_t
	*/
	read8bitoffsetreg(int regFileID, int regOffset);



	static void
	/**
	*@brief
	*
	*@param rxDelayTime
	*/
	setrxaftertxdelay(uint32_t rxDelayTime);

	static void
	/**
	*@brief
	*
	*@param registermask
	*/
	setIRQ(uint32_t registermask);

	static void
	/**
	*@brief
	*
	*/
	syncrxbufptrs(void);

	static void
	/**
	*@brief
	*
	*/
	forcetrxoff(void);

	static void
	/**
	*@brief
	*
	*/
	_disablesequencing();

	static void
	/**
	*@brief
	*
	*/
	_aonarrayupload();

	static void
	/**
	*@brief
	*
	*/
	_loaducodefromrom();

	static uint32_t
	/**
	*@brief
	*
	*@param address
	*@return uint32_t
	*/
	_otpread(uint32_t address);

	static void
	/**
	*@brief
	*
	*@param clocks
	*/
	_enableclocks(int clocks);

	static void
	/**
	*@brief
	*
	*@param prfIndex
	*/
	_configlde(int prfIndex);


	/**
	*@brief
	*
	*@return decaIrqStatus_t
	*/
	static decaIrqStatus_t decamutexon();

	static void
	decamutexoff(decaIrqStatus_t s);
};
}
#include "dw1000_impl.hpp"

#endif


