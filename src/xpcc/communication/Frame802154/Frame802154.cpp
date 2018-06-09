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

#ifndef XPCC_FRAME802154_CPP
#define XPCC_FRAME802154_CPP

#endif // XPCC_FRAME802154_CPP

#include "Frame802154.hpp"
using namespace xpcc;

Frame802154::Frame802154()
{
	setControl(0x00UL);
	setSequenceNumber(0);
	frame[payloadend] = 0;
	frame[payloadend + 1] = 0 ;
}

Frame802154::Frame802154(int size, uint8_t data[])

{
	int i;
	for (i=0;i<size;i++)
	{
		frame[i] = data[i];
	}
	setbeginvalues();
	length = size;
	payloadend = size-2;
	payloadlength = payloadend  - payloadbegin;
}


void
Frame802154::setControl (uint16_t control)
{
	frame[0] = uint8_t(control >> 8);
	frame[1] = uint8_t(control);
	setbeginvalues();
	frame[payloadend] = 0;
	frame[payloadend + 1] = 0 ;

}

uint16_t
Frame802154::getControl()
{
	return (((uint16_t)frame[0] << 8) | frame[1]);
}

void
Frame802154::setSequenceNumber(uint8_t number)
{
	frame[2]=number;
}

uint8_t
Frame802154::getSequenceNumber()
{
	return frame[2];
}

void
Frame802154::setDestinationPANAddress(uint16_t panaddress)
{
	if ((getControl() >> Header::DEST_ADDRESSING_MODE) % 4 != 0)

	{
		frame[3] = uint8_t(panaddress);
		frame[4] = uint8_t(panaddress >> 8);
	}
}

uint16_t
Frame802154::getDestinationPANAddress()
{
	if ((getControl() >> Header::DEST_ADDRESSING_MODE) % 4 != 0)
	{
		return (((uint16_t)frame[4] << 8) | frame[3]);
	}
	else
	{
		return 0;
	}
}

void
Frame802154::setDestinationAddress64(uint64_t address)
{
	if (((getControl() >> Header::DEST_ADDRESSING_MODE) & Header::LONG_ADDRESS) == Header::LONG_ADDRESS)
	{
		frame[5]	= uint8_t(address);
		frame[6]	= uint8_t(address >> 8);
		frame[7]	= uint8_t(address >> 16);
		frame[8]	= uint8_t(address >> 24);
		frame[9]	= uint8_t(address >> 32);
		frame[10]	= uint8_t(address >> 40);
		frame[11]	= uint8_t(address >> 48);
		frame[12]	= uint8_t(address >> 56);
	}
	else if (((getControl() >> Header::DEST_ADDRESSING_MODE) & Header::SHORT_ADDRESS) == Header::SHORT_ADDRESS)
	{
		frame[5]	= uint8_t(address);
		frame[6]	= uint8_t(address >> 8);

	}
}


uint64_t
Frame802154::getDestinationAddress64()
{
	if (((getControl() >> Header::DEST_ADDRESSING_MODE) & Header::LONG_ADDRESS )== Header::LONG_ADDRESS)
	{
		return ((uint64_t)frame[12]<<56 |
				(uint64_t)frame[11]<<48 |
				(uint64_t)frame[10]<<40 |
				(uint64_t)frame[9]<<32 |
				(uint32_t)frame[8]<<24 |
				(uint32_t)frame[7]<<16 |
				(uint32_t)frame[6]<<8 |
				frame[5]);
	}
	else if (((getControl() >> Header::DEST_ADDRESSING_MODE) & Header::SHORT_ADDRESS) == Header::SHORT_ADDRESS)
	{
		return ( uint64_t(frame[5]| frame[6]<<8 ));
	}
	else
	{
		return 0;
	}
}

void
Frame802154::setDestinationAddress16(uint16_t address)
{ 	
	if (((getControl() >> Header::DEST_ADDRESSING_MODE) & Header::LONG_ADDRESS) == Header::LONG_ADDRESS)
	{
		frame[5]	= uint8_t(address);
		frame[6]	= uint8_t(address >> 8);
		frame[7]	= uint8_t(0);
		frame[8]	= uint8_t(0);
		frame[9]	= uint8_t(0);
		frame[10]	= uint8_t(0);
		frame[11]	= uint8_t(0);
		frame[12]	= uint8_t(0);
	}
	else if (((getControl() >> Header::DEST_ADDRESSING_MODE) & Header::SHORT_ADDRESS) == Header::SHORT_ADDRESS)
	{
		frame[6]	= uint8_t(address>> 8);
		frame[5]	= uint8_t(address);
	}

}

uint16_t
Frame802154::getDestinationAddress16()
{
	if (((getControl() >> Header::DEST_ADDRESSING_MODE) & Header::LONG_ADDRESS )== Header::LONG_ADDRESS)
	{
		return ((uint32_t)frame[6]<<8 | frame[5]);
	}
	else if (((getControl() >> Header::DEST_ADDRESSING_MODE) & Header::SHORT_ADDRESS) == Header::SHORT_ADDRESS)
	{
		return ( uint64_t(frame[5]| frame[6]<<8 ));
	}
	else
	{
		return 0;
	}
}

void
Frame802154::setSourcePANAddress(uint16_t panaddress)
{
	if (((getControl() >> Header::SOURCE_ADDRESSING_MODE) % 4  != 0) && not(getControl() & (PAN_ID_COMPRESSION) ) )
	{

		frame[SourcePANAddressbegin] = uint8_t(panaddress);
		frame[SourcePANAddressbegin+1] = uint8_t(panaddress >> 8);

	}
	else if (getControl() & PAN_ID_COMPRESSION)
	{
		setDestinationPANAddress(panaddress);
	}
}

uint16_t
Frame802154::getSourcePANAddress()
{
	if (((getControl()<< Header::SOURCE_ADDRESSING_MODE) != 0) && not(getControl() & PAN_ID_COMPRESSION))
	{
		if (((getControl() >> Header::SOURCE_ADDRESSING_MODE) % 4) != NOPAN_AND_NOADRESSFIELD)
		{return ( (uint16_t)frame[SourcePANAddressbegin+1]<<8 | frame[SourcePANAddressbegin]); }
		else
		{
			return 0;
		}
	}
	else if (getControl() & PAN_ID_COMPRESSION)
	{
		return (getDestinationPANAddress());
	}
	else
	{return 0;}
}

void
Frame802154::setSourceAddress64(uint64_t address)
{
	if (((getControl() >> Header::SOURCE_ADDRESSING_MODE) & Header::LONG_ADDRESS)== Header::LONG_ADDRESS)
	{
		frame[SourceAddressbegin +0]	= uint8_t(address);
		frame[SourceAddressbegin +1]	= uint8_t(address >> 8);
		frame[SourceAddressbegin +2]	= uint8_t(address >> 16);
		frame[SourceAddressbegin +3]	= uint8_t(address >> 24);
		frame[SourceAddressbegin +4]	= uint8_t(address >> 32);
		frame[SourceAddressbegin +5]	= uint8_t(address >> 40);
		frame[SourceAddressbegin +6]	= uint8_t(address >> 48);
		frame[SourceAddressbegin +7]	= uint8_t(address >> 56);
	}
	else if (((getControl() >> Header::SOURCE_ADDRESSING_MODE) & Header::SHORT_ADDRESS )== Header::SHORT_ADDRESS)
	{

		frame[SourceAddressbegin+0]	= uint8_t(address);
		frame[SourceAddressbegin+1]	= uint8_t(address >> 8);

	}

}

uint64_t
Frame802154::getSourceAddress64()
{
	if (((getControl() >> Header::SOURCE_ADDRESSING_MODE) & Header::LONG_ADDRESS ) == Header::LONG_ADDRESS )
	{
		return (
					(uint64_t)frame[SourceAddressbegin+7]<<56|
					(uint64_t)frame[SourceAddressbegin+6]<<48|
					(uint64_t)frame[SourceAddressbegin+5]<<40|
					(uint64_t)frame[SourceAddressbegin+4]<<32|
					(uint64_t)frame[SourceAddressbegin+3]<<24|
					(uint64_t)frame[SourceAddressbegin+2]<<16|
					(uint64_t)frame[SourceAddressbegin+1]<<8 |
					(uint64_t)frame[SourceAddressbegin+0]<<0 );
	}
	else if(((getControl() >> Header::SOURCE_ADDRESSING_MODE) & Header::SHORT_ADDRESS )== Header::SHORT_ADDRESS)
	{
		return (uint64_t)frame[SourceAddressbegin+1] <<8 | frame[SourceAddressbegin+0];
	}

	else
	{
		return 0;
	}
}

void
Frame802154::setSourceAddress16(uint16_t address)
{
	if (((getControl() >> Header::SOURCE_ADDRESSING_MODE) & Header::LONG_ADDRESS ) == Header::LONG_ADDRESS )
	{
		frame[SourceAddressbegin +0]	= uint8_t(address);
		frame[SourceAddressbegin +1]	= uint8_t(address >> 8);
		frame[SourceAddressbegin +2]	= uint8_t(0);
		frame[SourceAddressbegin +3]	= uint8_t(0);
		frame[SourceAddressbegin +4]	= uint8_t(0);
		frame[SourceAddressbegin +5]	= uint8_t(0);
		frame[SourceAddressbegin +6]	= uint8_t(0);
		frame[SourceAddressbegin +7]	= uint8_t(0);
	}
	else if (((getControl()>> Header::SOURCE_ADDRESSING_MODE) & Header::SHORT_ADDRESS) == Header::SHORT_ADDRESS)
	{
		frame[SourceAddressbegin+0] = address;
		frame[SourceAddressbegin+1] = address >> 8;
	}
}

uint16_t
Frame802154::getSourceAddress16()
{
	if (((getControl() >> Header::SOURCE_ADDRESSING_MODE) & Header::LONG_ADDRESS ) == Header::LONG_ADDRESS )
	{
		return ((uint64_t)frame[SourceAddressbegin+1]<<8
													   |(uint64_t)frame[SourceAddressbegin+0]<<0
																							   );
	}
	else if ((getControl() >> Header::SOURCE_ADDRESSING_MODE & Header::SHORT_ADDRESS) == Header::SHORT_ADDRESS)
	{
		return ((uint16_t)(frame[SourceAddressbegin+0] ) | frame[SourceAddressbegin+1] << 8);
	}
	else
	{
		return 0;
	}
}

void
Frame802154::setPayload(int size,uint8_t data[])
{
	cleanPayload();
	addPayload(size,data);
}



void
Frame802154::getPayload(int size, uint8_t data[])
{
	int i;
	for (i=0;i<size;i++)
	{
		data[i] = frame[payloadbegin + i];
	}
}

void
Frame802154::debugToString()
{
	XPCC_LOG_DEBUG << "---------IEE802.15.4-2011FRAME------HEADDER--------------"<< xpcc::endl;
	XPCC_LOG_DEBUG.printf("Frame Control= %x \n", getControl());
	XPCC_LOG_DEBUG.printf("SequenceNumber= %d \n",getSequenceNumber());
	XPCC_LOG_DEBUG.printf("DestinationPAN= %x \n", getDestinationPANAddress());
	XPCC_LOG_DEBUG.printf("DestinationAddress= %llx \n", (uint64_t)getDestinationAddress64());
	XPCC_LOG_DEBUG.printf("SourcePAN= %x \n", getSourcePANAddress());
	XPCC_LOG_DEBUG.printf("SourceAddress= %llx \n", (uint64_t)getSourceAddress64());
	XPCC_LOG_DEBUG << "---------PAYLOAD-----------------------------------------"<< xpcc::endl;
	XPCC_LOG_DEBUG << xpcc::endl <<  "Payloadbegin: " << payloadbegin  << " Payloadend: " << payloadend << " Payloadlength: " << payloadlength << xpcc::endl;
	int i;
	for (i=0;i<payloadlength;i++)
	{
		XPCC_LOG_DEBUG.printf("Payload[%d] = %x \n\n",i,frame[payloadbegin + i]);
	}
}

void
Frame802154::loadFrame(int size, uint8_t data[])
{
	int i;
	for (i=0;i<size;i++)
	{
		frame[i] = data[i];
	}
	setbeginvalues();
	length  = size;
	payloadend = size-3;
	payloadlength = payloadend +1 - payloadbegin;
}

void
Frame802154::getFrame(uint8_t buffer[])
{
	int i;
	for (i=0;i<length;i++)
	{
		buffer[i]=frame[i];
	}
}

void
Frame802154::addPayload(uint32_t size, uint8_t data[])
{
	uint32_t i;
	for (i=0;i<size;i++)
	{
		frame[payloadend] = data[i];
		payloadend++;
		payloadlength++;
		length++;
	}
	frame[payloadend] = 0x0;
	frame[payloadend+1] = 0x0;
}

void
Frame802154::cleanPayload()
{
	frame[payloadbegin + 1 ]	=0x00;
	frame[payloadbegin + 0]	    =0x00;
	payloadend = payloadbegin;
	length = payloadend+2;
	payloadlength = payloadend - payloadbegin;

}

void
Frame802154::setbeginvalues()// TODO:: EDIT IF SECURITY COMES IN

{
	uint16_t control = getControl();
	//Set SourcePANAddressBegin
	if(((control >> Header::DEST_ADDRESSING_MODE) % 4) == NOPAN_AND_NOADRESSFIELD)
	{
		SourcePANAddressbegin = 3;
	}
	else if (((control >> Header::DEST_ADDRESSING_MODE) % 4 ) == Header::SHORT_ADDRESS)
	{
		SourcePANAddressbegin = 7;
	}
	else if (((control >> Header::DEST_ADDRESSING_MODE) % 4) == Header::LONG_ADDRESS)
	{
		SourcePANAddressbegin = 13;
	}
	//Set SourceAddressBegin
	if (control & (PAN_ID_COMPRESSION))
	{
		SourceAddressbegin = SourcePANAddressbegin;
	}
	else
	{
		SourceAddressbegin = SourcePANAddressbegin + 2;
	}
	//SetPayloadBegin --- SECURITY BEGIN - PLS CHANGE WHEN SECU COMES IN
	if (((control >> Header::SOURCE_ADDRESSING_MODE) % 4) == NOPAN_AND_NOADRESSFIELD)
	{
		if (control & (PAN_ID_COMPRESSION))
		{
			SourceAddressbegin -= 2;
		}
		payloadbegin = SourcePANAddressbegin;
	}
	else if (((control >> Header::SOURCE_ADDRESSING_MODE) % 4) == Header::SHORT_ADDRESS)
	{
		payloadbegin = SourceAddressbegin + 2;
	}
	else
	{
		payloadbegin = SourceAddressbegin + 8;
	}
	payloadend = payloadbegin;
	payloadlength = 0;
	length = payloadbegin + 2;
}
