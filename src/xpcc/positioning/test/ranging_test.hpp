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
#include <unittest/testsuite.hpp>

class RangingTest : public unittest::TestSuite
{
public:
	void
	testRangingMath();
	void
	testRangingLogic();
};
