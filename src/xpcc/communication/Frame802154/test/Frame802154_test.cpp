// coding: utf-8
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
// ----------------------------------------------------------------------------

#include "Frame802154_test.hpp"
#include "../Frame802154.hpp"

void Frame802154Test::testConstruction()
{
	uint8_t buffer[255] = {0x00,0x00,0x42,'T','E','S','T',0x54,0x0};
	uint8_t emptybuffer[255] = {0,0,0,0,0};
	uint8_t testbuffer[255];
	xpcc::Frame802154 testframe = xpcc::Frame802154();
	TEST_ASSERT_EQUALS(testframe.getControl(),0U);
	TEST_ASSERT_EQUALS(testframe.getSequenceNumber(),0U);
	TEST_ASSERT_EQUALS(testframe.getDestinationPANAddress(),0U);
#if not defined(XPCC__CPU_AVR) 
	TEST_ASSERT_EQUALS(testframe.getDestinationAddress64(),0UL);
#endif
	TEST_ASSERT_EQUALS(testframe.getDestinationAddress16(),0U);
	TEST_ASSERT_EQUALS(testframe.getSourcePANAddress(),0U);
#if not defined(XPCC__CPU_AVR) 
	TEST_ASSERT_EQUALS(testframe.getSourceAddress64(),0UL);
#endif
	TEST_ASSERT_EQUALS(testframe.getSourceAddress16(),0U);
	TEST_ASSERT_EQUALS(testframe.payloadlength,0);
	TEST_ASSERT_EQUALS(testframe.length,5);
	testframe.getFrame(testbuffer);
	TEST_ASSERT_EQUALS_ARRAY(emptybuffer,testbuffer,5);
	
	testframe = xpcc::Frame802154(9,buffer);
	TEST_ASSERT_EQUALS(testframe.getControl(),00U);
	TEST_ASSERT_EQUALS(testframe.getSequenceNumber(),0x42U);
	TEST_ASSERT_EQUALS(testframe.getDestinationPANAddress(),0U);
#if not defined(XPCC__CPU_AVR) 
	TEST_ASSERT_EQUALS(testframe.getDestinationAddress64(),0UL);
#endif
	TEST_ASSERT_EQUALS(testframe.getDestinationAddress16(),0U);
	TEST_ASSERT_EQUALS(testframe.getSourcePANAddress(),0U);
#if not defined(XPCC__CPU_AVR) 
	TEST_ASSERT_EQUALS(testframe.getSourceAddress64(),0UL);
#endif
	TEST_ASSERT_EQUALS(testframe.getSourceAddress16(),0U);
	TEST_ASSERT_EQUALS(testframe.payloadlength,4);
	TEST_ASSERT_EQUALS(testframe.length,9);
	testframe.getFrame(testbuffer);
	TEST_ASSERT_EQUALS_ARRAY(buffer,testbuffer,9);
}
void Frame802154Test::testGetterandSetter()
{
	uint8_t payload[] = {'T','E','S','T'};
	uint8_t testarray[] = {0,0,42,'T','E','S','T',0,0};
	xpcc::Frame802154 testframe = xpcc::Frame802154();
	uint8_t testbuffer[255];

	testframe.setControl(0x0000);
	testframe.setSequenceNumber(42);
	testframe.setDestinationPANAddress(0x0123);
	testframe.setDestinationAddress16(0x4567);
	testframe.setSourcePANAddress(0x89AB);
	testframe.setSourceAddress16(0xCDEF);
	testframe.setPayload (4,payload);
	TEST_ASSERT_EQUALS(testframe.getControl(),00U);
	TEST_ASSERT_EQUALS(testframe.getSequenceNumber(),42);
	TEST_ASSERT_EQUALS(testframe.getDestinationPANAddress(),0U);
#if not defined(XPCC__CPU_AVR) 
	TEST_ASSERT_EQUALS(testframe.getDestinationAddress64(),0UL);
#endif
	TEST_ASSERT_EQUALS(testframe.getDestinationAddress16(),0U);
	TEST_ASSERT_EQUALS(testframe.getSourcePANAddress(),0U);
#if not defined(XPCC__CPU_AVR) 
	TEST_ASSERT_EQUALS(testframe.getSourceAddress64(),0UL);
#endif
	TEST_ASSERT_EQUALS(testframe.getSourceAddress16(),0U);
	TEST_ASSERT_EQUALS(testframe.payloadlength,4);
	TEST_ASSERT_EQUALS(testframe.length,9);
	testframe.getFrame(testbuffer);
	TEST_ASSERT_EQUALS_ARRAY(testarray,testbuffer,9);
	testframe.setDestinationAddress64(0x12345678);
	testframe.setSourceAddress64(0x90ABCDEF);
#if not defined(XPCC__CPU_AVR) 
	TEST_ASSERT_EQUALS(testframe.getDestinationAddress64(),0UL);
#endif
	TEST_ASSERT_EQUALS(testframe.getDestinationAddress16(),0U);
#if not defined(XPCC__CPU_AVR) 
	TEST_ASSERT_EQUALS(testframe.getSourceAddress64(),0UL);
#endif
	TEST_ASSERT_EQUALS(testframe.getSourceAddress16(),0U);

	testframe = xpcc::Frame802154();
	testframe.setControl(0x0800);
	testframe.setSequenceNumber(42);
	testframe.setDestinationPANAddress(0x0123);
	testframe.setDestinationAddress16(0x4567);
	testframe.setSourcePANAddress(0x89AB);
	testframe.setSourceAddress16(0xCDEF);
	testframe.setPayload (4,payload);
	TEST_ASSERT_EQUALS(testframe.getControl(),0x0800U);
	TEST_ASSERT_EQUALS(testframe.getSequenceNumber(),42);
	TEST_ASSERT_EQUALS(testframe.getDestinationPANAddress(),0x0123U);
#if not defined(XPCC__CPU_AVR) 
	TEST_ASSERT_EQUALS(testframe.getDestinationAddress64(),0x4567UL);
#endif
	TEST_ASSERT_EQUALS(testframe.getDestinationAddress16(),0x4567U);
	TEST_ASSERT_EQUALS(testframe.getSourcePANAddress(),0x0U);
#if not defined(XPCC__CPU_AVR) 
	TEST_ASSERT_EQUALS(testframe.getSourceAddress64(),0UL);
#endif
	TEST_ASSERT_EQUALS(testframe.getSourceAddress16(),0U);
	TEST_ASSERT_EQUALS(testframe.payloadlength,4);
	TEST_ASSERT_EQUALS(testframe.length,13);
	testframe.getPayload(4,testbuffer);
	TEST_ASSERT_EQUALS_ARRAY(payload,testbuffer,4);
	uint8_t testarray1[] = {0x8,0x0,42,0x23,0x1,0x67,0x45,'T','E','S','T',0,0};
	testframe.getFrame(testbuffer);
	TEST_ASSERT_EQUALS_ARRAY(testarray1,testbuffer,13);
	testframe.setDestinationAddress64(0x12345678);
	testframe.setSourceAddress64(0x90ABCDEF);
#if not defined(XPCC__CPU_AVR) 
	TEST_ASSERT_EQUALS(testframe.getDestinationAddress64(),0x5678UL);
#endif
	TEST_ASSERT_EQUALS(testframe.getDestinationAddress16(),0x5678U);
#if not defined(XPCC__CPU_AVR) 
	TEST_ASSERT_EQUALS(testframe.getSourceAddress64(),0UL);
#endif
	TEST_ASSERT_EQUALS(testframe.getSourceAddress16(),0U);

	testframe = xpcc::Frame802154();
	testframe.setControl(0x8800);
	testframe.setSequenceNumber(42);
	testframe.setDestinationPANAddress(0x0123);
	testframe.setDestinationAddress16(0x4567);
	testframe.setSourcePANAddress(0x89AB);
	testframe.setSourceAddress16(0xCDEF);
	testframe.setPayload (4,payload);
	TEST_ASSERT_EQUALS(testframe.getControl(),0x8800U);
	TEST_ASSERT_EQUALS(testframe.getSequenceNumber(),42);
	TEST_ASSERT_EQUALS(testframe.getDestinationPANAddress(),0x0123U);
#if not defined(XPCC__CPU_AVR) 
	TEST_ASSERT_EQUALS(testframe.getDestinationAddress64(),0x4567UL);
#endif
	TEST_ASSERT_EQUALS(testframe.getDestinationAddress16(),0x4567U);
	TEST_ASSERT_EQUALS(testframe.getSourcePANAddress(),0x89ABU);
#if not defined(XPCC__CPU_AVR) 
	TEST_ASSERT_EQUALS(testframe.getSourceAddress64(),0xCDEFUL);
#endif
	TEST_ASSERT_EQUALS(testframe.getSourceAddress16(),0xCDEFU);
	TEST_ASSERT_EQUALS(testframe.payloadlength,4);
	testframe.getPayload(4,testbuffer);
	TEST_ASSERT_EQUALS_ARRAY(payload,testbuffer,4);
	TEST_ASSERT_EQUALS(testframe.length,17);
	testframe.setDestinationAddress64(0x12345678);
	testframe.setSourceAddress64(0x90ABCDEF);
#if not defined(XPCC__CPU_AVR) 
	TEST_ASSERT_EQUALS(testframe.getDestinationAddress64(),0x5678UL);
#endif
	TEST_ASSERT_EQUALS(testframe.getDestinationAddress16(),0x5678U);
#if not defined(XPCC__CPU_AVR) 
	TEST_ASSERT_EQUALS(testframe.getSourceAddress64(),0xCDEFUL);
#endif
	TEST_ASSERT_EQUALS(testframe.getSourceAddress16(),0xCDEFU);

	testframe = xpcc::Frame802154();
	testframe.setControl(0xCC00);
	testframe.setSequenceNumber(42);
	testframe.setDestinationPANAddress(0x0123);
	testframe.setDestinationAddress16(0x4567);
	testframe.setSourcePANAddress(0x89AB);
	testframe.setSourceAddress16(0xCDEFU);
	testframe.setPayload (4,payload);
	TEST_ASSERT_EQUALS(testframe.getControl(),0xCC00U);
	TEST_ASSERT_EQUALS(testframe.getSequenceNumber(),42);
	TEST_ASSERT_EQUALS(testframe.getDestinationPANAddress(),0x0123U);
#if not defined(XPCC__CPU_AVR) 
	TEST_ASSERT_EQUALS(testframe.getDestinationAddress64(),0x4567UL);
#endif
	TEST_ASSERT_EQUALS(testframe.getDestinationAddress16(),0x4567U);
	TEST_ASSERT_EQUALS(testframe.getSourcePANAddress(),0x89ABU);
#if not defined(XPCC__CPU_AVR) 
	TEST_ASSERT_EQUALS(testframe.getSourceAddress64(),0xCDEFUL);
#endif
	TEST_ASSERT_EQUALS(testframe.getSourceAddress16(),0xCDEFU);
	TEST_ASSERT_EQUALS(testframe.payloadlength,4);
	testframe.getPayload(4,testbuffer);
	TEST_ASSERT_EQUALS_ARRAY(payload,testbuffer,4);
	TEST_ASSERT_EQUALS(testframe.length,29);
	testframe.setDestinationAddress64(0x12345678);
	testframe.setSourceAddress64(0x90ABCDEF);
#if not defined(XPCC__CPU_AVR) 
	TEST_ASSERT_EQUALS(testframe.getDestinationAddress64(),0x12345678UL);
#endif
	TEST_ASSERT_EQUALS(testframe.getDestinationAddress16(),0x5678U);
#if not defined(XPCC__CPU_AVR) 
	TEST_ASSERT_EQUALS(testframe.getSourceAddress64(),0x90ABCDEFUL);
#endif
	TEST_ASSERT_EQUALS(testframe.getSourceAddress16(),0xCDEFU);

	testframe = xpcc::Frame802154();
	testframe.setControl(0x8840);
	testframe.setSequenceNumber(42);
	testframe.setDestinationPANAddress(0x0123);
	testframe.setDestinationAddress16(0x4567);
	testframe.setSourcePANAddress(0x89AB);
	testframe.setSourceAddress16(0xCDEF);
	testframe.setPayload (4,payload);
	TEST_ASSERT_EQUALS(testframe.getControl(),0x8840U);
	TEST_ASSERT_EQUALS(testframe.getSequenceNumber(),42);
	TEST_ASSERT_EQUALS(testframe.getDestinationPANAddress(),0x89ABU);
#if not defined(XPCC__CPU_AVR) 
	TEST_ASSERT_EQUALS(testframe.getDestinationAddress64(),0x4567UL);
#endif
	TEST_ASSERT_EQUALS(testframe.getDestinationAddress16(),0x4567U);
	TEST_ASSERT_EQUALS(testframe.getSourcePANAddress(),0x89ABU);
#if not defined(XPCC__CPU_AVR) 
	TEST_ASSERT_EQUALS(testframe.getSourceAddress64(),0xCDEFUL);
#endif
	TEST_ASSERT_EQUALS(testframe.getSourceAddress16(),0xCDEFU);
	TEST_ASSERT_EQUALS(testframe.payloadlength,4);
	testframe.getPayload(4,testbuffer);
	TEST_ASSERT_EQUALS_ARRAY(payload,testbuffer,4);
	TEST_ASSERT_EQUALS(testframe.length,15);
	testframe.setDestinationAddress64(0x12345678);
	testframe.setSourceAddress64(0x90ABCDEF);
#if not defined(XPCC__CPU_AVR) 
	TEST_ASSERT_EQUALS(testframe.getDestinationAddress64(),0x5678UL);
#endif
	TEST_ASSERT_EQUALS(testframe.getDestinationAddress16(),0x5678U);
#if not defined(XPCC__CPU_AVR) 
	TEST_ASSERT_EQUALS(testframe.getSourceAddress64(),0xCDEFUL);
#endif
	TEST_ASSERT_EQUALS(testframe.getSourceAddress16(),0xCDEFU);

	testframe = xpcc::Frame802154();
	testframe.setControl(0xCC40);
	testframe.setSequenceNumber(42);
	testframe.setDestinationPANAddress(0x0123);
	testframe.setDestinationAddress16(0x4567);
	testframe.setSourcePANAddress(0x89AB);
	testframe.setSourceAddress16(0xCDEFU);
	testframe.setPayload (4,payload);
	TEST_ASSERT_EQUALS(testframe.getControl(),0xCC40U);
	TEST_ASSERT_EQUALS(testframe.getSequenceNumber(),42);
	TEST_ASSERT_EQUALS(testframe.getDestinationPANAddress(),0x89ABU);
#if not defined(XPCC__CPU_AVR) 
	TEST_ASSERT_EQUALS(testframe.getDestinationAddress64(),0x4567UL);
#endif
	TEST_ASSERT_EQUALS(testframe.getDestinationAddress16(),0x4567U);
	TEST_ASSERT_EQUALS(testframe.getSourcePANAddress(),0x89ABU);
#if not defined(XPCC__CPU_AVR) 
	TEST_ASSERT_EQUALS(testframe.getSourceAddress64(),0xCDEFUL);
#endif
	TEST_ASSERT_EQUALS(testframe.getSourceAddress16(),0xCDEFU);
	TEST_ASSERT_EQUALS(testframe.payloadlength,4);
	testframe.getPayload(4,testbuffer);
	TEST_ASSERT_EQUALS_ARRAY(payload,testbuffer,4);
	TEST_ASSERT_EQUALS(testframe.length,27);
	testframe.setDestinationAddress64(0x12345678);
	testframe.setSourceAddress64(0x90ABCDEF);
#if not defined(XPCC__CPU_AVR) 
	TEST_ASSERT_EQUALS(testframe.getDestinationAddress64(),0x12345678UL);
#endif
	TEST_ASSERT_EQUALS(testframe.getDestinationAddress16(),0x5678U);
#if not defined(XPCC__CPU_AVR) 
	TEST_ASSERT_EQUALS(testframe.getSourceAddress64(),0x90ABCDEFUL);
#endif
	TEST_ASSERT_EQUALS(testframe.getSourceAddress16(),0xCDEFU);

	testframe = xpcc::Frame802154();
	testframe.setControl(0x8C40);
	testframe.setSequenceNumber(42);
	testframe.setDestinationPANAddress(0x0123);
	testframe.setDestinationAddress16(0x4567);
	testframe.setSourcePANAddress(0x89AB);
	testframe.setSourceAddress16(0xCDEFU);
	testframe.setPayload (4,payload);
	TEST_ASSERT_EQUALS(testframe.getControl(),0x8C40);
	TEST_ASSERT_EQUALS(testframe.getSequenceNumber(),42);
	TEST_ASSERT_EQUALS(testframe.getDestinationPANAddress(),0x89ABU);
#if not defined(XPCC__CPU_AVR) 
	TEST_ASSERT_EQUALS(testframe.getDestinationAddress64(),0x4567UL);
#endif
	TEST_ASSERT_EQUALS(testframe.getDestinationAddress16(),0x4567U);
	TEST_ASSERT_EQUALS(testframe.getSourcePANAddress(),0x89ABU);
#if not defined(XPCC__CPU_AVR) 
	TEST_ASSERT_EQUALS(testframe.getSourceAddress64(),0xCDEFUL);
#endif
	TEST_ASSERT_EQUALS(testframe.getSourceAddress16(),0xCDEFU);
	TEST_ASSERT_EQUALS(testframe.payloadlength,4);
	testframe.getPayload(4,testbuffer);
	TEST_ASSERT_EQUALS_ARRAY(payload,testbuffer,4);
	TEST_ASSERT_EQUALS(testframe.length,21);
	testframe.setDestinationAddress64(0x12345678);
	testframe.setSourceAddress64(0x90ABCDEF);
#if not defined(XPCC__CPU_AVR) 
	TEST_ASSERT_EQUALS(testframe.getDestinationAddress64(),0x12345678UL);
#endif
	TEST_ASSERT_EQUALS(testframe.getDestinationAddress16(),0x5678U);
#if not defined(XPCC__CPU_AVR) 
	TEST_ASSERT_EQUALS(testframe.getSourceAddress64(),0xCDEFUL);
#endif
	TEST_ASSERT_EQUALS(testframe.getSourceAddress16(),0xCDEFU);

	testframe = xpcc::Frame802154();
	testframe.setControl(0xC840);
	testframe.setSequenceNumber(42);
	testframe.setDestinationPANAddress(0x0123);
	testframe.setDestinationAddress16(0x4567);
	testframe.setSourcePANAddress(0x89AB);
	testframe.setSourceAddress16(0xCDEFU);
	testframe.setPayload (4,payload);
	TEST_ASSERT_EQUALS(testframe.getControl(),0xC840);
	TEST_ASSERT_EQUALS(testframe.getSequenceNumber(),42);
	TEST_ASSERT_EQUALS(testframe.getDestinationPANAddress(),0x89ABU);
#if not defined(XPCC__CPU_AVR) 
	TEST_ASSERT_EQUALS(testframe.getDestinationAddress64(),0x4567UL);
#endif
	TEST_ASSERT_EQUALS(testframe.getDestinationAddress16(),0x4567U);
	TEST_ASSERT_EQUALS(testframe.getSourcePANAddress(),0x89ABU);
#if not defined(XPCC__CPU_AVR) 
	TEST_ASSERT_EQUALS(testframe.getSourceAddress64(),0xCDEFUL);
#endif
	TEST_ASSERT_EQUALS(testframe.getSourceAddress16(),0xCDEFU);
	TEST_ASSERT_EQUALS(testframe.payloadlength,4);
	testframe.getPayload(4,testbuffer);
	TEST_ASSERT_EQUALS_ARRAY(payload,testbuffer,4);
	TEST_ASSERT_EQUALS(testframe.length,21);
	testframe.setDestinationAddress64(0x12345678);
	testframe.setSourceAddress64(0x90ABCDEF);
#if not defined(XPCC__CPU_AVR) 
	TEST_ASSERT_EQUALS(testframe.getDestinationAddress64(),0x5678UL);
#endif
	TEST_ASSERT_EQUALS(testframe.getDestinationAddress16(),0x5678U);
#if not defined(XPCC__CPU_AVR) 
	TEST_ASSERT_EQUALS(testframe.getSourceAddress64(),0x90ABCDEFUL);
#endif
	TEST_ASSERT_EQUALS(testframe.getSourceAddress16(),0xCDEFU);
}

void Frame802154Test::testPayload()
{
	xpcc::Frame802154 testframe = xpcc::Frame802154();
	uint8_t  payload[] = {'T','E','S','T'};
	
	uint8_t buffer[255];


	testframe = xpcc::Frame802154();
	testframe.setControl(0xCC00);
	testframe.setSequenceNumber(0x42);
	testframe.setDestinationPANAddress(0x0123);
	testframe.setSourcePANAddress(0x89AB);
	testframe.setDestinationAddress64(0x1234567890ABCDEF);
	testframe.setSourceAddress64(0xFEDCBA0987654321);
	TEST_ASSERT_EQUALS(testframe.payloadlength,0);
	TEST_ASSERT_EQUALS(testframe.length,25);
	testframe.getFrame(buffer);
	uint8_t testbuffer[] = {0xCC,0x00,0x42,0x23,0x01,0xEF,0xCD,0xAB,0x90,0x78,0x56,0x34,0x12,0xAB,0x89,0x21,0x43,0x65,0x87,0x09,0xBA,0xDC,0xFE,0x0,0x0};
	TEST_ASSERT_EQUALS_ARRAY(buffer,testbuffer,testframe.length);
	
	testframe.setPayload(4,payload);
	TEST_ASSERT_EQUALS(testframe.payloadlength,4);
	TEST_ASSERT_EQUALS(testframe.length,29);
	uint8_t testbuffer2[] = {0xCC,0x00,0x42,0x23,0x01,0xEF,0xCD,0xAB,0x90,0x78,0x56,0x34,0x12,0xAB,0x89,0x21,0x43,0x65,0x87,0x09,0xBA,0xDC,0xFE,'T','E','S','T',0x0,0x0};
	testframe.getFrame(buffer);
	TEST_ASSERT_EQUALS_ARRAY(buffer,testbuffer2,testframe.length);
	testframe.getPayload(4,buffer);
	TEST_ASSERT_EQUALS_ARRAY(buffer,payload,testframe.payloadlength);
	
	testframe.addPayload(4,payload);
	TEST_ASSERT_EQUALS(testframe.payloadlength,8);
	TEST_ASSERT_EQUALS(testframe.length,33);
	uint8_t testbuffer3[] = {0xCC,0x00,0x42,0x23,0x01,0xEF,0xCD,0xAB,0x90,0x78,0x56,0x34,0x12,0xAB,0x89,0x21,0x43,0x65,0x87,0x09,0xBA,0xDC,0xFE,'T','E','S','T','T','E','S','T',0x0,0x0};
	testframe.getFrame(buffer);
	TEST_ASSERT_EQUALS_ARRAY(buffer,testbuffer3,testframe.length);

	testframe.addPayload(0,payload);
	TEST_ASSERT_EQUALS(testframe.payloadlength,8);
	TEST_ASSERT_EQUALS(testframe.length,33);
	testframe.getFrame(buffer);
	TEST_ASSERT_EQUALS_ARRAY(buffer,testbuffer3,testframe.length);
	
	
	testframe.setPayload(0,payload);
	TEST_ASSERT_EQUALS(testframe.payloadlength,0);
	TEST_ASSERT_EQUALS(testframe.length,25);
	testframe.getFrame(buffer);
	TEST_ASSERT_EQUALS_ARRAY(buffer,testbuffer,testframe.length);

	testframe.addPayload(0,payload);
	TEST_ASSERT_EQUALS(testframe.payloadlength,0);
	TEST_ASSERT_EQUALS(testframe.length,25);
	testframe.getFrame(buffer);
	TEST_ASSERT_EQUALS_ARRAY(buffer,testbuffer,testframe.length);

	testframe.addPayload(4,payload);
	TEST_ASSERT_EQUALS(testframe.payloadlength,4);
	TEST_ASSERT_EQUALS(testframe.length,29);
	testframe.getFrame(buffer);
	TEST_ASSERT_EQUALS_ARRAY(buffer,testbuffer2,testframe.length);
	testframe.getPayload(4,buffer);
	TEST_ASSERT_EQUALS_ARRAY(buffer,payload,testframe.payloadlength);

	testframe.cleanPayload();
	TEST_ASSERT_EQUALS(testframe.payloadlength,0);
	TEST_ASSERT_EQUALS(testframe.length,25);
	testframe.getFrame(buffer);
	TEST_ASSERT_EQUALS_ARRAY(buffer,testbuffer,testframe.length);
}

