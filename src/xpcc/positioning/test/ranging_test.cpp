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

#include "ranging_test.hpp"
#include "../ranging.hpp"
#include "testdevice.hpp"
#include <xpcc/communication/Frame802154/Frame802154.hpp>
using ranging = xpcc::Ranging <testdevice>;

void RangingTest::testRangingMath()
{
	TEST_ASSERT_EQUALS(ranging::computeSsTwrTof(0,0,0,0),0);
	TEST_ASSERT_EQUALS(ranging::computeSsTwrTof(3,0,1,0),1);
	TEST_ASSERT_EQUALS(ranging::computeSsTwrTof(3,0,5,0),-1);
	TEST_ASSERT_EQUALS_FLOAT(ranging::computeDistance(0,ranging::SPEED_OF_LIGHT),0);
	TEST_ASSERT_EQUALS_FLOAT(ranging::computeDistance(1,ranging::SPEED_OF_LIGHT),float(ranging::SPEED_OF_LIGHT));
	TEST_ASSERT_EQUALS_FLOAT(ranging::computeDistance(0.5,ranging::SPEED_OF_LIGHT),float(ranging::SPEED_OF_LIGHT)/2.0);

	xpcc::Frame802154 testframe;
	testframe.setControl(0x8841);
	uint8_t payload[] = {0xE1,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	testframe.setPayload(9,payload);
	TEST_ASSERT_EQUALS_FLOAT(ranging::computeSsTwrDistance(testframe),1.0*(1e0/499.2e6/128e0)*ranging::SPEED_OF_LIGHT);
	payload[0] = 0xE5;
	payload[1] = 0x1;
	testframe.setPayload(9,payload);
	TEST_ASSERT_EQUALS_FLOAT(ranging::computeDsTwrDistance(0.0,testframe),0.5*(1e0/499.2e6/128e0)*ranging::SPEED_OF_LIGHT);
	TEST_ASSERT_EQUALS_FLOAT(ranging::computeDsTwrDistance(1.0,testframe),0.5*(1+(1e0/499.2e6/128e0)*ranging::SPEED_OF_LIGHT));
	payload[1] = 0x0;
	testframe.setPayload(9,payload);
	TEST_ASSERT_EQUALS_FLOAT(ranging::computeDsTwrDistance(1.0,testframe),0.5*(1+0));
	TEST_ASSERT_EQUALS_FLOAT(ranging::computeDsTwrDistance(0,testframe),0);
}

void RangingTest::testRangingLogic()
{
	xpcc::Frame802154 testframe;
	testframe.setControl(0x8841);
	uint8_t payload[] = {0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	testframe.setPayload(9,payload);
	TEST_ASSERT_EQUALS(ranging::IsRangingFrame(testframe),true);
	payload[0] = 0xE1;
	testframe.setPayload(9,payload);
	TEST_ASSERT_EQUALS(ranging::IsRangingFrame(testframe),true);
	payload[0] = 0xE2;
	testframe.setPayload(9,payload);
	TEST_ASSERT_EQUALS(ranging::IsRangingFrame(testframe),true);
	payload[0] = 0xE3;
	testframe.setPayload(9,payload);
	TEST_ASSERT_EQUALS(ranging::IsRangingFrame(testframe),true);
	payload[0] = 0xE4;
	testframe.setPayload(9,payload);
	TEST_ASSERT_EQUALS(ranging::IsRangingFrame(testframe),true);
	payload[0] = 0xE5;
	testframe.setPayload(9,payload);
	TEST_ASSERT_EQUALS(ranging::IsRangingFrame(testframe),true);
	payload[0] = 0xE6;
	testframe.setPayload(9,payload);
	TEST_ASSERT_EQUALS(ranging::IsRangingFrame(testframe),false);
	payload[0] = 0xE3;
	testframe.setPayload(9,payload);
	TEST_ASSERT_EQUALS(ranging::getTof(testframe),1);
	payload[5] = 0x2;
	testframe.setPayload(9,payload);
	TEST_ASSERT_EQUALS(ranging::getTof(testframe),0);
	payload[1] = 0x2;
	payload[5] = 0x6;
	testframe.setPayload(9,payload);
	TEST_ASSERT_EQUALS(ranging::getTof(testframe),-1);
	payload[0] = 0xE5;
	testframe.setPayload(9,payload);
	TEST_ASSERT_EQUALS(ranging::getTof(testframe),2);
	payload[5] = 0xFF;
	testframe.setPayload(9,payload);
	TEST_ASSERT_EQUALS(ranging::getTof(testframe),-2);

}
