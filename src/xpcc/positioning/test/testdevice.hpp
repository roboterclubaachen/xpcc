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

class testdevice{
public:
	static constexpr uint32_t UUS_TO_TIME_UNIT = 65536;
	static constexpr float UUS_TO_MICROS = (4492e-1/512e0);
	static constexpr float TIME_UNIT_TO_S =  (1e0/499.2e6/128e0);
	static uint8_t frame_seq_nb;
	static uint64_t hostaddress;
	static bool send(int length,uint16_t buffer[]) {
		int a = length;
		a = a;
		buffer[0] = buffer[0];
		return true;}
	static bool sendAt(int length,uint16_t buffer[], uint32_t time) {
		int a = length;
		uint32_t b = time;
                a=a;
                b=b;
		buffer[0] = buffer[0];
		return true;}
	static uint64_t readTXTimestamp64(){return 1;}
	static uint64_t readRXTimestamp64(){return 3;}

};
#include "testdevice_impl.hpp"
