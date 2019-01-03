/**
*  Copyright (c) 2018, Marten Junga (Github.com/Maju-Ketchup)
* All Rights Reserved.
*
* The file is part of the xpcc library and is released under the 3-clause BSD
* license. See the file `LICENSE` for the full license governing this code.
*
*
* The headder contains the class implementation of the IEEE standart 802.15.4-2011 Frame
* current max size is 255 bytes but some devices are able to send 1023 bytes
* Set always control first
*
*/

#ifndef RANGING
#define RANGING

#include <xpcc/communication/Frame802154/Frame802154.hpp> 


namespace xpcc {
/**
* @ingroup positioning
* @brief implements ranging Algorithms for IEEE 802.15.4 Devices
*
* IMPLEMENTS single-sided two-way ranging for UWB-transmitters \n
* IMPLEMENTS double-sided two-way ranging for UWB-transmitters \n
*
* This Class gives functions and constants for Single and Doublesided Two Way Ranging for UWB devices
* It currently supports solo the Decawave DW1000
*
*/

template < typename ComDevice >
class Ranging
{
public:

	static constexpr uint32_t
	control = (frame802154::DATA<<frame802154::FRAME_TYPE)
			|(frame802154::PAN_ID_COMPRESSION)
			|(frame802154::SHORT_ADDRESS<<frame802154::DEST_ADDRESSING_MODE)
			|(frame802154::SHORT_ADDRESS<<frame802154::SOURCE_ADDRESSING_MODE)
			; /*< standart control bytes of a ranging frame */

	enum  rangingflag: uint8_t
	{
		SSTWR_INIT   = 0xE0,
		SSTWR_RESP   = 0xE1,
		DSTWR_INIT0  = 0xE2,
		DSTWR_RESP0  = 0xE3,
		DSTWR_INIT1  = 0xE4,
		DSTWR_RESP1  = 0xE5
	};


	/* Delay between frames, in UWB microseconds. */
	static constexpr int POLL_TX_TO_RESP_RX_DLY_UUS = 140;
	/* Receive response timeout. */
	static constexpr int RESP_RX_TIMEOUT_UUS = 1750;

	/* Speed of light in vakuum, in meters per second. */
	static constexpr int SPEED_OF_LIGHT = 299792548;

	/**
	* @brief
	* Sends an SSTWR initiation with the payload{0xE0}
	*/
	static void sendSSTWRinit();

	/**
	* @brief
	* Sends an SSTWR initiation with the payload{0xE0} to the given address
	*/

	static void  sendSSTWRinitAt(uint16_t address);
	/**
	* @brief
	* Sends an SSTWR initiation the payload{0xE2} to the broadcast address (0xFFFF)
	* @param address
	*/

	static void sendDSTWRinit();

	/**
	* @brief
	*  Sends an SSTWR initiation with the payload{0xE2} to the given address
	* @param address
	*/
	static void sendDSTWRinitAt(uint16_t address);

	/**
	* @brief
	* checks if the given Frame Payload begins with an Ranging Flag (0xE0 - 0xE5)
	* @param receiveframe
	* @return bool
	*/
	static bool IsRangingFrame(Frame802154 receiveframe);

	/**
	* @brief
	* if the given frame was an ranging frame this function sends the corrosponding answer
	* @param receiveframe
	*/
	static bool sendAnswer(Frame802154 receiveframe);

	/**
	* @brief
	* Computes the distance with the given frame
	* @param receiveframe
	* @return float
	*/
	static float computeSsTwrDistance(Frame802154 receiveframe);

	/**
	* @brief
	* takes a computed distance in (m) and a frame with the Payloadprefix 0xE5(DSTWR_RESP1) and computes a length out of it
	* @param distance
	* @param receiveframe
	* @return float
	*/
	static float computeDsTwrDistance(float distance,Frame802154 receiveframe);

	/**
	* @brief
	* computes the time of flight with the given rx and tx times returns an int in the timeunits of the micro controler
	* @param init_tx
	* @param init_rx
	* @param resp_rx
	* @param resp_tx
	* @return int
	*/
	static int computeSsTwrTof(int init_tx, int init_rx, int resp_rx , int resp_tx);

	/**
	* @brief
	* Computes the distance with a given time of flight in (Micro Controler time units)
	* @param tof in clocktics
	* @param travelspeed travelspeed in m/s
	* @return float
	*/
	static float computeDistance(float tof, int travelspeed);

	/**
	* @brief
	* gets the time of flight off the given Frame if its payloads begins with (E5)
	* @param receiveframe
	* @return int
	*/
	static int getTof(Frame802154 receiveframe);

private:


	/**
	* @brief
	*
	* @param receiveframe
	*/
	static void sendtof(Frame802154 receiveframe);

	/**
	* @brief
	*
	* @param flag
	* @param receiveframe
	*/
	static bool sendtimestamps(uint8_t flag, Frame802154 receiveframe);

	/**
	* @brief
	*
	* @param timestamp1
	* @param timestamp2
	* @param buffer[]
	*/
	static void gettimestamps(uint32_t &timestamp1, uint32_t &timestamp2, uint8_t buffer[]);

	/**
	* @brief
	*
	* @param buffer[]
	* @return int
	*/
	static int getTof(uint8_t buffer[]);

	/**
	* @brief
	*
	* @param buffer[]
	* @param flag
	* @param ownrx
	* @param owntx
	*/
	static void setanswerpayload(uint8_t buffer [],uint8_t flag, uint64_t ownrx, uint64_t owntx);

	/**
	* @brief
	*
	* @param buffer[]
	* @param tof
	*/
	static void setanswerpayload_tof(uint8_t buffer[], int tof);
};
}
#include "ranging_impl.hpp"
#endif // RANGING
