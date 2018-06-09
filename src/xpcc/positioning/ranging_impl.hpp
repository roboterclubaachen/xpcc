/**
*  Copyright (c) 2018, Marten Junga (Github.com/Maju-Ketchup)
* All Rights Reserved.
*
* The file is part of the xpcc library and is released under the 3-clause BSD
* license. See the file `LICENSE` for the full license governing this code.
*
*
* The headder contains the class implementation of the IEEE standart 802.15.4-2011 Frame
* current max size is 255 bytes
* Set always control first
*
*/
#ifndef RANGING_IMPL_HPP
#define RANGING_IMPL_HPP


#endif // RANGING_IMPL_HPP

template < typename ComDevice >
float
xpcc::Ranging<ComDevice>::computeDsTwrDistance(float distance,xpcc::Frame802154 receiveframe)
{

	uint8_t buffer[5];
	receiveframe.getPayload(5,buffer);
	//receiveframe.debugToString();
	if (buffer[0] == DSTWR_RESP1)
	{
		float tof;
		tof = getTof(buffer);
		//XPCC_LOG_DEBUG.printf("foreign tof %f \n",tof);
		tof = tof*ComDevice::TIME_UNIT_TO_S;

		return ((computeDistance(tof,SPEED_OF_LIGHT)+ distance)/2);
	}
	else
	{
		return 0;
	}
}

template < typename ComDevice >
void
xpcc::Ranging<ComDevice>::sendSSTWRinit()
{
	xpcc::Frame802154 sendframe;
	uint8_t payload = SSTWR_INIT;
	static uint8_t buffer[256];

	sendframe.setControl(control); // 16Bit Addresses short PAN DATA
	sendframe.setSequenceNumber(ComDevice::frame_seq_nb); //Set SequenceNumber
	sendframe.setDestinationPANAddress(0xDECA); //Set PANAddress
	sendframe.setDestinationAddress16(frame802154::broadcast16bitAddress);	//Set destinationaddress
	sendframe.setSourceAddress16(ComDevice::hostaddress);	//Set Sourceaddress to own address
	sendframe.setPayload(1,&payload);
	sendframe.getFrame(buffer);
	ComDevice::send(sendframe.length, buffer);
	ComDevice::frame_seq_nb++;
}

template < typename ComDevice >
void
xpcc::Ranging<ComDevice>::sendSSTWRinitAt(uint16_t address)
{
	xpcc::Frame802154 sendframe;
	uint8_t payload = SSTWR_INIT;
	static uint8_t buffer[256];

	sendframe.setControl(control); // 16Bit Addresses short PAN DATA
	sendframe.setSequenceNumber(ComDevice::frame_seq_nb); //Set SequenceNumber
	sendframe.setDestinationPANAddress(0xDECA); //Set PANAddress
	sendframe.setDestinationAddress16(address);	//Set destinationaddress
	sendframe.setSourceAddress16(ComDevice::hostaddress);	//Set Sourceaddress to own address
	sendframe.setPayload(1,&payload);
	sendframe.getFrame(buffer);
	ComDevice::send(sendframe.length, buffer);
	ComDevice::frame_seq_nb++;
}

template < typename ComDevice >
void
xpcc::Ranging<ComDevice>::sendDSTWRinit()
{
	xpcc::Frame802154 sendframe;
	uint8_t payload = DSTWR_INIT0;
	static uint8_t buffer[256];

	sendframe.setControl(control); // 16Bit Addresses short PAN DATA
	sendframe.setSequenceNumber(ComDevice::frame_seq_nb); //Set SequenceNumber
	sendframe.setDestinationPANAddress(0xDECA); //Set PANAddress
	sendframe.setDestinationAddress16(frame802154::broadcast16bitAddress);	//Set destinationaddress
	sendframe.setSourceAddress16(ComDevice::hostaddress);	//Set Sourceaddress to own address
	sendframe.setPayload(1,&payload);
	sendframe.getFrame(buffer);
	ComDevice::send(sendframe.length, buffer);
	//sendframe.debugToString();
	ComDevice::frame_seq_nb++;
}



template < typename ComDevice >
void
xpcc::Ranging<ComDevice>::sendDSTWRinitAt(uint16_t address)
{
	xpcc::Frame802154 sendframe;
	uint8_t payload = DSTWR_INIT0;
	static uint8_t buffer[256];

	sendframe.setControl(control); // 16Bit Addresses short PAN DATA
	sendframe.setSequenceNumber(ComDevice::frame_seq_nb); //Set SequenceNumber
	sendframe.setDestinationPANAddress(0xDECA); //Set PANAddress
	sendframe.setDestinationAddress16(address);	//Set destinationaddress
	sendframe.setSourceAddress16(ComDevice::hostaddress);	//Set Sourceaddress to own address
	sendframe.setPayload(1,&payload);
	sendframe.getFrame(buffer);
	ComDevice::send(sendframe.length, buffer);
	//sendframe.debugToString();
	ComDevice::frame_seq_nb++;
}

template < typename ComDevice >
bool
xpcc::Ranging<ComDevice>::sendAnswer(xpcc::Frame802154 receiveframe)
{

	static uint8_t buffer[256];

	receiveframe.getPayload(receiveframe.payloadlength,buffer);
	if (buffer[0] == SSTWR_INIT){
		return(sendtimestamps(SSTWR_RESP,receiveframe));
	}
	else if (buffer[0] == DSTWR_INIT0){
		return(sendtimestamps(DSTWR_RESP0,receiveframe));
	}
	else if (buffer[0] == DSTWR_RESP0){
		return(sendtimestamps(DSTWR_INIT1,receiveframe));
	}
	else if (buffer[0] == DSTWR_INIT1){
		sendtof(receiveframe);
		return true;
	}
	return false;
}

template < typename ComDevice >
bool
xpcc::Ranging<ComDevice>::IsRangingFrame(xpcc::Frame802154 receiveframe)
{
	uint8_t buffer[256];
	receiveframe.getPayload(receiveframe.payloadlength,buffer);
	return (buffer[0] == SSTWR_INIT || buffer[0] == SSTWR_RESP || buffer[0] == DSTWR_INIT0 || buffer[0] == DSTWR_RESP0 || buffer[0] == DSTWR_INIT1 || buffer[0] == DSTWR_RESP1);
}


template < typename ComDevice >
float
xpcc::Ranging<ComDevice>::computeSsTwrDistance(xpcc::Frame802154 receiveframe)
{

	uint64_t owntx,ownrx;
	uint32_t responserx = 0;
	uint32_t responsetx = 0;
	float tof;
	float distance = 0;
	static uint8_t buffer[256];

	receiveframe.getPayload(receiveframe.payloadlength,buffer);
	if (buffer[0] == SSTWR_RESP || buffer[0] == DSTWR_RESP0)
	{
		//timeout.restart(0);
		gettimestamps(responserx,responsetx,buffer);
		owntx = ComDevice::readTXTimestamp64();
		ownrx = ComDevice::readRXTimestamp64();

		tof = computeSsTwrTof(ownrx,owntx,responsetx,responserx)*ComDevice::TIME_UNIT_TO_S;
		distance = computeDistance(tof,SPEED_OF_LIGHT);
		ComDevice::frame_seq_nb = receiveframe.getSequenceNumber() + 1 ;

	}
	else
	{
		XPCC_LOG_ERROR << "EXPECTET RANGING FRAME GOT:" << xpcc::endl ;
		receiveframe.debugToString();
		XPCC_LOG_ERROR << xpcc::endl << xpcc::endl ;

	}
	return (distance);
}

template < typename ComDevice >
int
xpcc::Ranging<ComDevice>::computeSsTwrTof(int init_rx, int init_tx, int resp_tx , int resp_rx)
{
	int returnv = (((init_rx-init_tx)-(resp_tx-resp_rx))/2);
	return (returnv);
}

template < typename ComDevice >
float
xpcc::Ranging<ComDevice>::computeDistance(float tof, int travelspeed)
{
	return (tof*travelspeed);
}

template < typename ComDevice >
void
xpcc::Ranging<ComDevice>::sendtof(xpcc::Frame802154 receiveframe){
	xpcc::Frame802154 send;

	static uint8_t buffer[256];

	receiveframe.getPayload(receiveframe.payloadlength,buffer);

	if (buffer[0] == DSTWR_INIT1)
	{
		int tof = 0;
		tof= getTof(receiveframe);
		//SET ANSWER
		send.setControl(control); // 16Bit Addresses; one PAN; DATA
		send.setDestinationPANAddress(receiveframe.getDestinationPANAddress()); //Set PANAddress
		send.setSourceAddress16(uint16_t(ComDevice::hostaddress));	//Set Sourceaddress to own address
		setanswerpayload_tof(buffer,tof);
		ComDevice::frame_seq_nb = receiveframe.getSequenceNumber() + 1 ;
		send.setSequenceNumber(ComDevice::frame_seq_nb);
		send.setDestinationAddress16(receiveframe.getSourceAddress16());
		send.setPayload(6,buffer);
		send.getFrame(buffer);
		//SEND ANSWER
		ComDevice::send(send.length,buffer);
		//XPCC_LOG_DEBUG.printf("owntx = %llu,ownrx = %llu,responserx = %lu, responsetx= %lu, tof: %d \n", owntx, ownrx,responserx, responsetx, tof);
		//receiveframe.debugToString();
		//send.debugToString();
	}
}

template < typename ComDevice >
bool
xpcc::Ranging<ComDevice>::sendtimestamps(uint8_t flag, xpcc::Frame802154 receiveframe)
{
	xpcc::Frame802154 send;
	uint32_t sendtime;
	uint64_t owntx,ownrx;
	static uint8_t buffer[256];
	//PREPAIR ANSWER
	send.setControl(control); // 16Bit Addresses; one PAN; DATA
	send.setDestinationPANAddress(receiveframe.getDestinationPANAddress()); //Set PANAddress
	send.setSourceAddress16(uint16_t(ComDevice::hostaddress));	//Set Sourceaddress to own address
	ownrx = ComDevice::readRXTimestamp64();
	sendtime = ((ownrx + (RESP_RX_TIMEOUT_UUS * (ComDevice::UUS_TO_TIME_UNIT))) >> 8);
	owntx = ((((uint64_t)(sendtime & 0xFFFFFFEUL) << 8)  )) + ComDevice::TX_ANT_DLY;
	// set starttime
	setanswerpayload(buffer, flag , ownrx, owntx);
	(ComDevice::frame_seq_nb) = receiveframe.getSequenceNumber() + 1 ;
	//SET ANSWER
	send.setSequenceNumber(ComDevice::frame_seq_nb);
	send.setDestinationAddress16(receiveframe.getSourceAddress16());
	send.setPayload(9,buffer);
	send.getFrame(buffer);
	//SEND ANSWER
	ComDevice::frame_seq_nb ++;
	return(ComDevice::sendAt(send.length,buffer,sendtime));
}

template < typename ComDevice >
void
xpcc::Ranging<ComDevice>::gettimestamps(uint32_t &timestamp1,uint32_t &timestamp2 , uint8_t buffer[])
{
	int i;
	timestamp1 = 0;
	timestamp2 = 0;
	for (i=4;i>0;i--)
	{
		timestamp1 = (timestamp1<<8)|buffer[i];
	}
	for (i=9;i>4;i--)
	{
		timestamp2 = (timestamp2<<8)|buffer[i];
	}
	//XPCC_LOG_DEBUG.printf("timestamp1: %lu , timestamp2: %lu \n",timestamp1,timestamp2);
	return;
}

template < typename ComDevice >
int
xpcc::Ranging<ComDevice>::getTof(uint8_t buffer[])
{

	int i;
	int tof = 0;
	for (i=1;i<5;i++){
		tof |= (uint32_t)buffer[i]<<(i-1)*8;}
	if (buffer[5] == 0xFF){
		tof = 0- tof;	}
	return tof;
}

template < typename ComDevice >
void
xpcc::Ranging<ComDevice>::setanswerpayload(uint8_t buffer [],uint8_t flag, uint64_t ownrx, uint64_t owntx)
{
	buffer[0] = flag;
	int i;
	for (i=1;i<5;i++)
	{
		buffer[i] = ownrx >> ((i-1)*8);
	}
	for (i=5;i<9;i++)
	{
		buffer[i] = owntx >> ((i-5)*8);
	}
}

template < typename ComDevice >
void
xpcc::Ranging<ComDevice>::setanswerpayload_tof(uint8_t buffer[], int tof)
{
	buffer[0] = DSTWR_RESP1;
	if (tof < 0){
		buffer[5] = 0xFF;
	}
	else{
		buffer[5] = 0x00;
	}
	if (tof < 0) {
		tof = tof - (2*tof);
	}

	uint32_t tof2 = tof;
	int i;
	for (i=1;i<5;i++)
	{
		buffer[i] = tof2 >> ((i-1)*8);
	}
}

template < typename ComDevice >
int
xpcc::Ranging<ComDevice>:: getTof(xpcc::Frame802154 receiveframe)
{

	uint64_t owntx,ownrx;
	uint32_t responserx;
	uint32_t responsetx = 0;
	int tof;
	static uint8_t buffer[256];

	receiveframe.getPayload(receiveframe.payloadlength,buffer);
	if (buffer[0] == DSTWR_RESP0 ||buffer[0] == DSTWR_INIT1)
	{
		//timeout.restart(0);
		gettimestamps(responserx,responsetx,buffer);
		owntx = ComDevice::readTXTimestamp64();
		ownrx = ComDevice::readRXTimestamp64();
		tof = computeSsTwrTof(ownrx,owntx,responsetx,responserx);

	}
	else if (buffer[0] == DSTWR_RESP1)
	{
		tof = getTof(buffer);
	}
	else
	{
		XPCC_LOG_ERROR << "EXPECTET RANGING FRAME WITH HEADER E3|E4|E5 GOT:" << xpcc::endl ;
		receiveframe.debugToString();
		XPCC_LOG_ERROR << xpcc::endl << xpcc::endl ;
		tof = 0;

	}
	return tof;
}



