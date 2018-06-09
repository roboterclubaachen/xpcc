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
#ifndef XPCC_Frame802154_HPP
#define XPCC_Frame802154_HPP

#include <xpcc/debug/logger.hpp>

namespace xpcc {

struct frame802154{
public:
	enum
			Header : uint8_t
	{
		//--------------FRAME CONTROL FIELD CONSTANTS--------------------------
		//OFFSETS
		/** 3 Bits see FrameTypes for Types  */
		FRAME_TYPE				= 0x0,
		/** 2 Bits see AddressModes for Modes */
		DEST_ADDRESSING_MODE	= 0xA,
		/** 2 Bits mostly 00 */
		FRAME_VERSION			= 0xC,
		/** 2 bits see AddressModes for Modes */
		SOURCE_ADDRESSING_MODE	= 0xE,

		//FLAGS
		/**if enabled security is present ---- not implanted*/
		SECURITY_ENABLED		= 0x8,
		/** if sender has more data to send turn this flag to 1 */
		FRAME_PENDING			= 0x10,
		/** Acknowledgment Request Flag */
		AR						= 0x20,
		/** IF set to 1 only destination PAN-address is available */
		PAN_ID_COMPRESSION		= 0x40,


		//FRAME TYPES
		BEACON					= 0b000,
		DATA					= 0b001,
		ACKNOWLEDGMENT			= 0b010,
		MAC_COMMAND				= 0b011,

		//ADRESSING MODES

		NOPAN_AND_NOADRESSFIELD	= 0b00,
		SHORT_ADDRESS			= 0b10,
		LONG_ADDRESS			= 0b11


	};
	/** 16Bit Broadcastaddress */
	static constexpr uint16_t broadcast16bitAddress = 0xFFFF;
	/** 64Bit Broadcastaddress */
	static constexpr uint64_t broadcast64bitAddress = 0xFFFFFFFFFFFFFFFF;
};




/**
 *
 * @ingroup 802154Frame
 * @brief
 * Copyright (c) 2017, Marten Junga (Github.com/Maju-Ketchup)
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 *
 *
 * The headder contains the class implementation of the IEEE standart 802.15.4-2011 MAC-frame \n
 * Current max size is 255 bytes \n
 * !The security header is not implemented at the moment! \n
 *
 * Set always control first \n
 *
 * Frame build up (First Byte on the left):
 * For all fields applies LSB (Least Significant Byte) first!
 *
 * |   2 Octets       |     1 Octet     |    0/2 Octets   |    0/2/8 Octets     |    0/2 Octets   |    0/2/8 Octets     | 0/5/6/10/14 Octets  |      variable       |    2 Octets     |
 * |------------------|-----------------|-----------------|---------------------|-----------------|---------------------|---------------------|---------------------|-----------------|
 * | Control Header   | Sequence Number | Destination PAN | Destination Address |   Source PAN    |    Source Address   |   Security Header   |      Payload        |      FSC        |
 *
 *
 * Control Header:
 *
 * |Bit:    |       0 + 1 + 2         |       3         |    4        |5 |    6             |   7   +    8   +   9   |    10  +   11              |    12  -   13  |   14   -  15      |
 * |--------|-------------------------|-----------------|-------------|--|------------------|------------------------|----------------------------|----------------|-------------------|
 * |Content:|        Frame Type       |Security Enabled |Frame Pending|AR|PAN ID Compression|        Reserved        |  Destination Address Mode  | Frame Version  | Sorce Address Mode|
 *
 *
 *
 * Frame Types: \n
 * 0b000 Beacon \n
 * 0b001 Data \n
 * 0b010 Acknowledgement \n
 * 0b011 MAC command \n
 * 0b100-111  Reserved \n \n
 *
 * Addressing Modes:\n
 * 0b00 PAN identifier and address fields are not present\n
 * 0b01 Reserved\n
 * 0b10 Address field contains a short address (16 bit)\n
 * 0b11 Address field contains an extended address (64 bit)\n
 *
 *
 * EXAMPLE USAGE:\n
 *  Frame802154 sendframe;\n
 *	uint8_t payload = 0xE0;\n
 *	static uint8_t buffer[256];\n
 *
 *	control = (frame802154::DATA<<frame802154::FRAME_TYPE)
 *		|(frame802154::PAN_ID_COMPRESSION)
 *		|(frame802154::SHORT_ADDRESS<<frame802154::DEST_ADDRESSING_MODE)
 *		|(frame802154::SHORT_ADDRESS<<frame802154::SOURCE_ADDRESSING_MODE)
 *		;// 16Bit Addresses short PAN DATA \n
 *	sendframe.setControl(control); \n
 *	sendframe.setSequenceNumber(ComDevice::frame_seq_nb); //Set SequenceNumber \n
 *	sendframe.setDestinationPANAddress(0xDECA); //Set PANAddress \n
 *	sendframe.setDestinationAddress16(0xFFFF);	//Set destinationaddress \n
 *	sendframe.setSourceAddress16(ComDevice::hostaddress);	//Set Sourceaddress to own address \n
 *	sendframe.setPayload(1,&payload); \n
 *	sendframe.getFrame(buffer); \n
 *
 *
 *
 *
 */
class Frame802154:frame802154
{
public:

	int length; /**< The length of the whole package in byte (includes 2 Byte checksum at the end) */
	int payloadlength; /**< The length of the payload */

	/**
	* @brief
	* This constructor creates an empty Frame
	* @return Frame802154
	*/
	Frame802154();

	/**
	* @brief
	*This constructor creates a package out of the given uint8_t array along with the given size
	*
	* @param size
	*  size should be max 255
	* @param data[]
	* An uint8_t array including the frame in Raw Bytes
	* @return Frame802154
	*/
	Frame802154(int size, uint8_t data[]);

	/**
	* @brief
	* This function loads the Rawbytes into this frame format
	*
	* @param size
	*  size should be max 255
	* @param data[]
	* An uint8_t array including the frame in Raw Bytes
	*/
	void loadFrame(int size, uint8_t data[]);

	/**
	* @brief
	*  This function loads the whole frame Rawdata into the given Buffer __ watch out for array overfloats
	* @param buffer[]
	*/
	void getFrame(uint8_t buffer[]);

	/**
	* @brief
	*  Sets the controlflags to the given double bytes
	* @param uint16_t
	*/
	void setControl(uint16_t);

	/**
	* @brief
	* returns the control bytes as uint16_t
	* @return uint16_t
	*/
	uint16_t getControl();


	/**
	* @brief
	* sets the sequence number 0_255(uint8_t)
	* @param number
	*/
	void setSequenceNumber(uint8_t number);
	/**
	* @brief
	* returns the sequence number of the given frame
	* @return uint8_t
	*/
	uint8_t getSequenceNumber();

	/**
	* @brief
	*
	* @param panaddress
	*/
	void setDestinationPANAddress(uint16_t panaddress);
	/**
	* @brief
	*
	* @return uint16_t
	*/
	uint16_t getDestinationPANAddress();

	/**
	* @brief
	*
	* @param address
	*/
	void setDestinationAddress64(uint64_t address);
	/**
	* @brief
	*
	* @return uint64_t
	*/
	uint64_t getDestinationAddress64();

	/**
	* @brief
	*
	* @param address
	*/
	void setDestinationAddress16(uint16_t address);
	/**
	* @brief
	*
	* @return uint16_t
	*/
	uint16_t getDestinationAddress16();

	/**
	* @brief
	*
	* @param panaddress
	*/
	void setSourcePANAddress(uint16_t panaddress);
	/**
	* @brief
	*
	* @return uint16_t
	*/
	uint16_t getSourcePANAddress();

	/**
	* @brief
	*
	* @param address
	*/
	void setSourceAddress64(uint64_t address);
	/**
	* @brief
	*
	* @return uint64_t
	*/
	uint64_t getSourceAddress64();

	/**
	* @brief
	*
	* @param address
	*/
	void setSourceAddress16(uint16_t address);
	/**
	* @brief
	*
	* @return uint16_t
	*/
	uint16_t getSourceAddress16();


	/**
	* @brief
	*
	* @param size
	* @param data[]
	*/
	void setPayload(int size,uint8_t data[]);
	/**
	* @brief
	*
	* @param size
	* @param data[]
	*/
	void getPayload(int size,uint8_t data[]);
	/**
	* @brief
	* adds payload of the length [size] and adds behind it to the actual payload
	* @param size
	* @param data[]
	*/
	void addPayload(uint32_t size, uint8_t data[]);
	/**
	* @brief
	* deletes the whole payload
	*/
	void cleanPayload();

	/**
	* @brief
	* prints the package Information into the Log stream
	*/
	void debugToString();


private:
	void setbeginvalues();

	//	void setbeginvalues();

	uint8_t	frame[255];				/**< includes the bytes of the frame */
	uint32_t	payloadbegin;			/**< the beginning byte of the payload */
	uint32_t	payloadend;				/**< The ending Byte of the Payload */
	uint32_t	SourcePANAddressbegin; 	/**The begin of the Source PANAddress*/
	uint32_t	SourceAddressbegin;		/**< The begin of the Source Address*/


};
}
#endif // Frame802154_HPP
