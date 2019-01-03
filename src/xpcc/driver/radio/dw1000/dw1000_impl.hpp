/**
 *  Copyright (c) 2017, Marten Junga (Github.com/Maju-Ketchup)
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


//Lot of Parts copied out of the DWM1000 API added some 'easier to use functions' *mja*
//"Don't include this file directly, use 'dw1000.hpp' instead!"



//--------------------------------------------------------------XPCC-Functions-Puplic------------------------------------------------------------------------



template < typename Spi, typename Cs, typename Reset, typename Irq >
uint8_t
xpcc::Dw1000< Spi, Cs, Reset, Irq >::frame_seq_nb = 0;

template < typename Spi, typename Cs, typename Reset, typename Irq >
uint64_t
xpcc::Dw1000< Spi, Cs, Reset, Irq >::hostaddress = 0xFFFFFFFFFFFFFFFF;

template < typename Spi, typename Cs, typename Reset, typename Irq >
uint32_t
xpcc::Dw1000< Spi, Cs, Reset, Irq >::TX_ANT_DLY = 16436;

template < typename Spi, typename Cs, typename Reset, typename Irq >
uint32_t
xpcc::Dw1000< Spi, Cs, Reset, Irq >::RX_ANT_DLY = 16436;

template < typename Spi, typename Cs, typename Reset, typename Irq >
bool
xpcc::Dw1000< Spi, Cs, Reset, Irq >::isChannelFree()
{
	const uint32_t rxEvent = (SYS_STATUS_RXPRD |
							  SYS_STATUS_RXSFDD |
							  SYS_STATUS_LDEDONE |
							  SYS_STATUS_RXPHD |
							  SYS_STATUS_RXPHE |
							  SYS_STATUS_RXDFR |
							  SYS_STATUS_RXFCG |
							  SYS_STATUS_RXFCE);
	write32bitreg(SYS_STATUS_ID,rxEvent);
	rxEnable();
	xpcc::delayMicroseconds(1807); // time of two transmissions
	trxdisable();
	if (readStatusRegister()&rxEvent)
	{
		rxreset();
		XPCC_LOG_INFO.printf("\n\nChannel busy\n\n\n");
		return false;

	}
	else
	{
		rxreset();
		return true;
	}
}

template < typename Spi, typename Cs, typename Reset, typename Irq >
xpcc::dw1000::IRQreason
xpcc::Dw1000< Spi, Cs, Reset, Irq >::getIRQReason() // TODO
{
	uint32_t systatus = readStatusRegister();
	systatus &= read32bitreg(SYS_MASK_ID);

	if (systatus &  SYS_STATUS_RXDFR)
	{
		write32bitreg(SYS_STATUS_ID, SYS_STATUS_RXDFR);
		return xpcc::dw1000::IRQreason::RX_Complete; //Frame received

	}
	else if (systatus & SYS_STATUS_TXFRS )
	{
		write32bitreg(SYS_STATUS_ID, SYS_STATUS_TXFRS);
		return xpcc::dw1000::IRQreason::TX_Complete; //Frame send
	}
	else if (systatus &  SYS_STATUS_ALL_RX_ERR)
	{write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_RX_ERR);
		return xpcc::dw1000::IRQreason::RX_Error; //Receive Error
	}
	else if (systatus &  SYS_MASK_MESYNCR )
	{
		write32bitreg(SYS_STATUS_ID, SYS_MASK_MESYNCR);
		return xpcc::dw1000::IRQreason::Sync; //Sync
	}
	else{
		return xpcc::dw1000::IRQreason::Unknown;
	}
}




template < typename Spi, typename Cs, typename Reset, typename Irq >
void
xpcc::Dw1000< Spi, Cs, Reset, Irq >::toggleReceiveIRQ()
{
	setIRQ( readStatusMRegister() ^ (SYS_STATUS_RXDFR | SYS_STATUS_ALL_RX_ERR));
}

template < typename Spi, typename Cs, typename Reset, typename Irq >
void
xpcc::Dw1000< Spi, Cs, Reset, Irq >::toggleSendIRQ()
{
	setIRQ(readStatusMRegister() ^ SYS_MASK_MTXFRS);
}

template < typename Spi, typename Cs, typename Reset, typename Irq >
void
xpcc::Dw1000< Spi, Cs, Reset, Irq >::toggleSyncIRQ()
{
	setIRQ(readStatusMRegister() ^ SYS_MASK_MESYNCR);
}

template < typename Spi, typename Cs, typename Reset, typename Irq >
uint32_t
xpcc::Dw1000< Spi, Cs, Reset, Irq >::readStatusRegister()
{
	return read32bitreg(SYS_STATUS_ID);
}

template < typename Spi, typename Cs, typename Reset, typename Irq >
uint32_t
xpcc::Dw1000< Spi, Cs, Reset, Irq >::readStatusMRegister()
{
	return read32bitreg(SYS_MASK_ID);
}

template < typename Spi, typename Cs, typename Reset, typename Irq >
void
xpcc::Dw1000< Spi, Cs, Reset, Irq >::setIRQ(uint32_t registermask)
{
	write32bitreg(SYS_MASK_ID, registermask);
}


template < typename Spi, typename Cs, typename Reset, typename Irq >
uint64_t
xpcc::Dw1000< Spi, Cs, Reset, Irq >::readTXTimestamp64()
{
	uint8_t ts_tab[5];
	uint64_t ts = 0;
	readTXTimestamp(ts_tab);
	for (int i = 0; i < 5; i++)
	{
		ts = ts << 8;
		ts |= ts_tab[4-i];
	}
	return ts;
}


template < typename Spi, typename Cs, typename Reset, typename Irq >
uint64_t
xpcc::Dw1000< Spi, Cs, Reset, Irq >::readRXTimestamp64()
{
	uint8_t ts_tab[5];
	uint64_t ts = 0;
	readRXTimestamp(ts_tab);
	for (int i = 0; i < 5; i++)
	{
		ts = ts << 8;
		ts |= ts_tab[4-i];
	}
	return ts;
}

template < typename Spi, typename Cs, typename Reset, typename Irq >
void
xpcc::Dw1000< Spi, Cs, Reset, Irq >::enableExternalSync()
{
	write32bitreg(EXT_SYNC_ID,EC_CTRL_OSTRM| 33 << 3);
}
template < typename Spi, typename Cs, typename Reset, typename Irq >
void
xpcc::Dw1000< Spi, Cs, Reset, Irq >::setCrystaltrim(uint8_t trim)
{
	if (trim < 0b100000){
		write8bitoffsetreg(0x2B,0x0E,0b01100000 | trim);}
}



template < typename Spi, typename Cs, typename Reset, typename Irq >
bool
xpcc::Dw1000< Spi, Cs, Reset, Irq >::sendAt(int size, uint8_t data[], uint32_t time)
{
	setdelayedtrxtime(time);
	write32bitreg(SYS_STATUS_ID, SYS_STATUS_TXFRS);
	writetxdata(size, data, 0); // writes TX DATA to DWM1000
	writetxfctrl(size, 0, 1); //write control
	if (starttx(START_TX_DELAYED) == SUCCESS)//send data direct
	{
		//XPCC_LOG_DEBUG << "DATA WAS SENT DELAYED" << xpcc::endl;
		return true;
	}
	else
	{
		XPCC_LOG_ERROR << "DWM1000: Could not send because delayed time < systemtime" << xpcc::endl;
		XPCC_LOG_DEBUG << int (readsystimestamphi32() - time) << xpcc::endl;
		return false;
	}
}


template < typename Spi, typename Cs, typename Reset, typename Irq >
bool
xpcc::Dw1000< Spi, Cs, Reset, Irq >::send(int size, uint8_t data[])
{
	if (size < 3 || size > 1023)
	{return false;}
	write32bitreg(SYS_STATUS_ID, SYS_STATUS_TXFRS);
	writetxdata(size, data, 0); // writes TX DATA to DWM1000
	writetxfctrl(size, 0, 0); //write control
	if (starttx(START_TX_IMMEDIATE) == SUCCESS)//send data direct
	{
		return true;
	}
	else
	{
		return false;
	}
}

template < typename Spi, typename Cs, typename Reset, typename Irq >
bool
xpcc::Dw1000< Spi, Cs, Reset, Irq >::checkForRX()
{
	uint32_t status_reg = read32bitreg(SYS_STATUS_ID);
	if (status_reg & SYS_STATUS_ALL_RX_TO)
	{XPCC_LOG_ERROR << "!!!DWM1000: RECEIVE TIME OUT!!!" << xpcc::endl;}
	if (status_reg & SYS_STATUS_ALL_RX_ERR)
	{XPCC_LOG_ERROR << "!!!DWM1000: RECEIVE ERROR OCCURRED!!!" << xpcc::endl;}
	return (status_reg & (SYS_STATUS_RXFCG));
}



template < typename Spi, typename Cs, typename Reset, typename Irq >
void
xpcc::Dw1000< Spi, Cs, Reset, Irq >::readrx(uint32_t size, uint8_t buffer[])
{
	if (read32bitreg(SYS_STATUS_ID & SYS_STATUS_RXFCG)) {
		write32bitreg(SYS_STATUS_ID, SYS_STATUS_RXFCG);
		readrxdata(buffer, size, 0);
	} else {
		XPCC_LOG_ERROR << "!!!DWM1000: ERROR NO FRAME TO READ!!!" << xpcc::endl;
	}
}

template < typename Spi, typename Cs, typename Reset, typename Irq >
uint16_t
xpcc::Dw1000< Spi, Cs, Reset, Irq >::rxlength()
{
	return((uint16_t)read32bitreg(RX_FINFO_ID) & RX_FINFO_RXFL_MASK_1023);
}

template < typename Spi, typename Cs, typename Reset, typename Irq >
void
xpcc::Dw1000< Spi, Cs, Reset, Irq >::displayMaskRegister ()
{
	uint32_t maskregister;
	maskregister = read32bitreg(SYS_MASK_ID);
	XPCC_LOG_DEBUG.printf ("DWM1000 StatusMaskregister: %lx \n",maskregister);
}

template < typename Spi, typename Cs, typename Reset, typename Irq >
void
xpcc::Dw1000< Spi, Cs, Reset, Irq >::displayStatusRegister ()
{
	uint32_t statusregister;
	statusregister = read32bitreg(SYS_STATUS_ID);
	XPCC_LOG_DEBUG.printf ("DWM1000 Statusregister: %lx \n", statusregister);
}

template < typename Spi, typename Cs, typename Reset, typename Irq >
bool
xpcc::Dw1000< Spi, Cs, Reset, Irq >::isFrameSent()
{
	return (read32bitreg(SYS_STATUS_ID)&SYS_STATUS_TXFRS);
}

template < typename Spi, typename Cs, typename Reset, typename Irq >
void
xpcc::Dw1000< Spi, Cs, Reset, Irq >::trxdisable()
{
	write32bitreg(SYS_CTRL_ID,SYS_CTRL_TRXOFF);
}

template < typename Spi, typename Cs, typename Reset, typename Irq >
bool
xpcc::Dw1000< Spi, Cs, Reset, Irq >::checkForRXError()
{
	return (read32bitreg(SYS_STATUS_ID) & SYS_STATUS_ALL_RX_ERR);
}

template < typename Spi, typename Cs, typename Reset, typename Irq >
void
xpcc::Dw1000< Spi, Cs, Reset, Irq >::rxdisable()
{
	trxdisable();
	rxreset();
}

//--------------------------------------------------------DWM1000-API----------------------------------------------------------------------------------

template < typename Spi, typename Cs, typename Reset, typename Irq >
xpcc::dw1000::local_data_t
xpcc::Dw1000< Spi, Cs, Reset, Irq >::dw1000local;

template < typename Spi, typename Cs, typename Reset, typename Irq >
int
xpcc::Dw1000< Spi, Cs, Reset, Irq >::writetospi(
		uint16_t headerLength,
		const uint8_t *headerBuffer,
		uint32_t bodylength,
		const uint8_t *bodyBuffer)
{
	// XPCC_LOG_DEBUG.printf("write\n");
	Cs::reset();

	uint8_t r_buffer[256] = {0};

	Spi::transferBlocking(
				(uint8_t*)headerBuffer /* w_buffer */,
				r_buffer /* r_buffer */,
				headerLength/* length */);
	Spi::transferBlocking((uint8_t*)bodyBuffer   /* w_buffer */,
						  r_buffer /* r_buffer */, bodylength/* length */);

	Cs::set();

	return SUCCESS;
}

template < typename Spi, typename Cs, typename Reset, typename Irq >
int
xpcc::Dw1000< Spi, Cs, Reset, Irq >::readfromspi(uint16_t headerLength, const uint8_t *headerBuffer, uint32_t readlength, uint8_t *readBuffer)
{
	// XPCC_LOG_DEBUG.printf("read\n");
	Cs::reset();

	uint8_t r_buffer[256] = {0};
	uint8_t w_buffer[256] = {0};

	Spi::transferBlocking(
				(uint8_t*)headerBuffer /* w_buffer */,
				r_buffer /* r_buffer */,
				headerLength/* length */);

	Spi::transferBlocking(
				(uint8_t*)w_buffer   /* w_buffer = dummy write*/,
				readBuffer /* r_buffer */, readlength/* length */);

	Cs::set();

	return SUCCESS;
}

template < typename Spi, typename Cs, typename Reset, typename Irq >
uint32_t
xpcc::Dw1000< Spi, Cs, Reset, Irq >::readdevid()
{
	return read32bitoffsetreg(DEV_ID_ID,0);
}

template < typename Spi, typename Cs, typename Reset, typename Irq >
void
xpcc::Dw1000< Spi, Cs, Reset, Irq >::setxtaltrim(uint8_t value)
{
	uint8_t reg_val = (3 << 5) | (value & FS_XTALT_MASK);
	write8bitoffsetreg(FS_CTRL_ID, FS_XTALT_OFFSET, reg_val);
}

template < typename Spi, typename Cs, typename Reset, typename Irq >
void
xpcc::Dw1000< Spi, Cs, Reset, Irq >::_disablesequencing()
{
	_enableclocks(FORCE_SYS_XTI); // Set system clock to XTI

	write16bitoffsetreg(PMSC_ID, PMSC_CTRL1_OFFSET, PMSC_CTRL1_PKTSEQ_DISABLE); // Disable PMSC ctrl of RF and RX clk blocks
}



template < typename Spi, typename Cs, typename Reset, typename Irq >
void
xpcc::Dw1000< Spi, Cs, Reset, Irq >::_aonarrayupload()
{
	write8bitoffsetreg(AON_ID, AON_CTRL_OFFSET, 0x00); // Clear the register
	write8bitoffsetreg(AON_ID, AON_CTRL_OFFSET, AON_CTRL_SAVE);
}

template < typename Spi, typename Cs, typename Reset, typename Irq >
void
xpcc::Dw1000< Spi, Cs, Reset, Irq >::softreset()
{
	_disablesequencing();

	// Clear any AON auto download bits (as reset will trigger AON download)
	write16bitoffsetreg(AON_ID, AON_WCFG_OFFSET, 0x00);
	// Clear the wake-up configuration
	write8bitoffsetreg(AON_ID, AON_CFG0_OFFSET, 0x00);
	// Upload the new configuration
	_aonarrayupload();

	// Reset HIF, TX, RX and PMSC
	write8bitoffsetreg(PMSC_ID, PMSC_CTRL0_SOFTRESET_OFFSET, PMSC_CTRL0_RESET_ALL);

	// DW1000 needs a 10us sleep to let clk PLL lock after reset - the PLL will automatically lock after the reset
	// Could also have polled the PLL lock flag, but then the SPI needs to be < 3MHz !! So a simple delay is easier
	//deca_sleep(1);
	xpcc::delayMilliseconds(1);

	// Clear reset
	write8bitoffsetreg(PMSC_ID, PMSC_CTRL0_SOFTRESET_OFFSET, PMSC_CTRL0_RESET_CLEAR);

	dw1000local.wait4resp = 0;
}


template < typename Spi, typename Cs, typename Reset, typename Irq >
void
xpcc::Dw1000< Spi, Cs, Reset, Irq >::_loaducodefromrom()
{
	_enableclocks(FORCE_LDE);

	// Kick off the LDE load
	write16bitoffsetreg(OTP_IF_ID, OTP_CTRL, OTP_CTRL_LDELOAD); // Set load LDE kick bit

	//deca_sleep(1); // Allow time for code to upload (should take up to 120 us)
	xpcc::delayMilliseconds(1);
	// Default clocks (ENABLE_ALL_SEQ)
	_enableclocks(ENABLE_ALL_SEQ); // Enable clocks for sequencing
}

template < typename Spi, typename Cs, typename Reset, typename Irq >
uint32_t
xpcc::Dw1000< Spi, Cs, Reset, Irq >::_otpread(uint32_t address)
{
	uint32_t ret_data;

	// Write the address
	write16bitoffsetreg(OTP_IF_ID, OTP_ADDR, address);

	// Perform OTP Read - Manual read mode has to be set
	write8bitoffsetreg(OTP_IF_ID, OTP_CTRL, OTP_CTRL_OTPREAD | OTP_CTRL_OTPRDEN);
	write8bitoffsetreg(OTP_IF_ID, OTP_CTRL, 0x00); // OTPREAD is self clearing but OTPRDEN is not

	// Read read data, available 40ns after rising edge of OTP_READ
	ret_data = read32bitoffsetreg(OTP_IF_ID, OTP_RDAT);

	// Return the 32bit of read data
	return ret_data;
}


template < typename Spi, typename Cs, typename Reset, typename Irq >
void
xpcc::Dw1000< Spi, Cs, Reset, Irq >::_enableclocks(int clocks)
{
	uint8_t reg[2];

	readfromdevice(PMSC_ID, PMSC_CTRL0_OFFSET, 2, reg);
	switch(clocks)
	{
	case ENABLE_ALL_SEQ:
	{
		reg[0] = 0x00 ;
		reg[1] = reg[1] & 0xfe;
	}
		break;
	case FORCE_SYS_XTI:
	{
		// System and RX
		reg[0] = 0x01 | (reg[0] & 0xfc);
	}
		break;
	case FORCE_SYS_PLL:
	{
		// System
		reg[0] = 0x02 | (reg[0] & 0xfc);
	}
		break;
	case READ_ACC_ON:
	{
		reg[0] = 0x48 | (reg[0] & 0xb3);
		reg[1] = 0x80 | reg[1];
	}
		break;
	case READ_ACC_OFF:
	{
		reg[0] = reg[0] & 0xb3;
		reg[1] = 0x7f & reg[1];
	}
		break;
	case FORCE_OTP_ON:
	{
		reg[1] = 0x02 | reg[1];
	}
		break;
	case FORCE_OTP_OFF:
	{
		reg[1] = reg[1] & 0xfd;
	}
		break;
	case FORCE_TX_PLL:
	{
		reg[0] = 0x20 | (reg[0] & 0xcf);
	}
		break;
	case FORCE_LDE:
	{
		reg[0] = 0x01;
		reg[1] = 0x03;
	}
		break;
	default:
		break;
	}

	// Need to write lower byte separately before setting the higher byte(s)
	writetodevice(PMSC_ID, PMSC_CTRL0_OFFSET, 1, &reg[0]);
	writetodevice(PMSC_ID, 0x1, 1, &reg[1]);
}


template < typename Spi, typename Cs, typename Reset, typename Irq >
void
xpcc::Dw1000< Spi, Cs, Reset, Irq >::_configlde(int prfIndex)
{
	write8bitoffsetreg(LDE_IF_ID, LDE_CFG1_OFFSET, LDE_PARAM1); // 8-bit configuration register

	if(prfIndex)
	{
		write16bitoffsetreg( LDE_IF_ID, LDE_CFG2_OFFSET, (uint16_t) LDE_PARAM3_64); // 16-bit LDE configuration tuning register
	}
	else
	{
		write16bitoffsetreg( LDE_IF_ID, LDE_CFG2_OFFSET, (uint16_t) LDE_PARAM3_16);
	}
}

template < typename Spi, typename Cs, typename Reset, typename Irq >
void
xpcc::Dw1000< Spi, Cs, Reset, Irq >::configure(config_t *conf)
{

	uint8_t nsSfd_result  = 0;
	uint8_t useDWnsSFD = 0;
	uint8_t chan = conf->chan ;
	uint32_t regval ;
	uint16_t reg16 = lde_replicaCoeff[conf->rxCode];
	uint8_t prfIndex = conf->prf - PRF_16M;
	uint8_t bw = ((chan == 4) || (chan == 7)) ? 1 : 0 ; // Select wide or narrow band

	// For 110 kbps we need a special setup
	if(BR_110K == conf->dataRate)
	{
		dw1000local.sysCFGreg |= SYS_CFG_RXM110K ;
		reg16 >>= 3; // lde_replicaCoeff must be divided by 8
	}
	else
	{
		dw1000local.sysCFGreg &= (~SYS_CFG_RXM110K) ;
	}

	dw1000local.longFrames = conf->phrMode ;

	dw1000local.sysCFGreg &= ~SYS_CFG_PHR_MODE_11;
	dw1000local.sysCFGreg |= (SYS_CFG_PHR_MODE_11 & (conf->phrMode << SYS_CFG_PHR_MODE_SHFT));

	write32bitreg(SYS_CFG_ID,dw1000local.sysCFGreg) ;
	// Set the lde_replicaCoeff
	write16bitoffsetreg(LDE_IF_ID, LDE_REPC_OFFSET, reg16) ;

	_configlde(prfIndex);

	// Configure PLL2/RF PLL block CFG/TUNE (for a given channel)
	write32bitoffsetreg(FS_CTRL_ID, FS_PLLCFG_OFFSET, fs_pll_cfg[chan_idx[chan]]);
	write8bitoffsetreg(FS_CTRL_ID, FS_PLLTUNE_OFFSET, fs_pll_tune[chan_idx[chan]]);

	// Configure RF RX blocks (for specified channel/bandwidth)
	write8bitoffsetreg(RF_CONF_ID, RF_RXCTRLH_OFFSET, rx_config[bw]);

	// Configure RF TX blocks (for specified channel and PRF)
	// Configure RF TX control
	write32bitoffsetreg(RF_CONF_ID, RF_TXCTRL_OFFSET, tx_config[chan_idx[chan]]);

	// Configure the baseband parameters (for specified PRF, bit rate, PAC, and SFD settings)
	// DTUNE0
	write16bitoffsetreg(DRX_CONF_ID, DRX_TUNE0b_OFFSET, sftsh[conf->dataRate][conf->nsSFD]);

	// DTUNE1
	write16bitoffsetreg(DRX_CONF_ID, DRX_TUNE1a_OFFSET, dtune1[prfIndex]);

	if(conf->dataRate == BR_110K)
	{
		write16bitoffsetreg(DRX_CONF_ID, DRX_TUNE1b_OFFSET, DRX_TUNE1b_110K);
	}
	else
	{
		if(conf->txPreambLength == PLEN_64)
		{
			write16bitoffsetreg(DRX_CONF_ID, DRX_TUNE1b_OFFSET, DRX_TUNE1b_6M8_PRE64);
			write8bitoffsetreg(DRX_CONF_ID, DRX_TUNE4H_OFFSET, DRX_TUNE4H_PRE64);
		}
		else
		{
			write16bitoffsetreg(DRX_CONF_ID, DRX_TUNE1b_OFFSET, DRX_TUNE1b_850K_6M8);
			write8bitoffsetreg(DRX_CONF_ID, DRX_TUNE4H_OFFSET, DRX_TUNE4H_PRE128PLUS);
		}
	}

	// DTUNE2
	write32bitoffsetreg(DRX_CONF_ID, DRX_TUNE2_OFFSET, digital_bb_config[prfIndex][conf->rxPAC]);

	// DTUNE3 (SFD timeout)
	// Don't allow 0 - SFD timeout will always be enabled
	if(conf->sfdTO == 0)
	{
		conf->sfdTO = SFDTOC_DEF;
	}
	write16bitoffsetreg(DRX_CONF_ID, DRX_SFDTOC_OFFSET, conf->sfdTO);

	// Configure AGC parameters
	write32bitoffsetreg( AGC_CFG_STS_ID, 0xC, agc_config.lo32);
	write16bitoffsetreg( AGC_CFG_STS_ID, 0x4, agc_config.target[prfIndex]);

	// Set (non-standard) user SFD for improved performance,
	if(conf->nsSFD)
	{
		// Write non standard (DW) SFD length
		write8bitoffsetreg(USR_SFD_ID, 0x00, dwnsSFDlen[conf->dataRate]);
		nsSfd_result = 3 ;
		useDWnsSFD = 1 ;
	}
	regval =  (CHAN_CTRL_TX_CHAN_MASK & (chan << CHAN_CTRL_TX_CHAN_SHIFT)) | // Transmit Channel
			(CHAN_CTRL_RX_CHAN_MASK & (chan << CHAN_CTRL_RX_CHAN_SHIFT)) | // Receive Channel
			(CHAN_CTRL_RXFPRF_MASK & (conf->prf << CHAN_CTRL_RXFPRF_SHIFT)) | // RX PRF
			((CHAN_CTRL_TNSSFD|CHAN_CTRL_RNSSFD) & (nsSfd_result << CHAN_CTRL_TNSSFD_SHIFT)) | // nsSFD enable RX&TX
			(CHAN_CTRL_DWSFD & (useDWnsSFD << CHAN_CTRL_DWSFD_SHIFT)) | // Use DW nsSFD
			(CHAN_CTRL_TX_PCOD_MASK & (conf->txCode << CHAN_CTRL_TX_PCOD_SHIFT)) | // TX Preamble Code
			(CHAN_CTRL_RX_PCOD_MASK & (conf->rxCode << CHAN_CTRL_RX_PCOD_SHIFT)) ; // RX Preamble Code

	write32bitreg(CHAN_CTRL_ID,regval) ;

	// Set up TX Preamble Size, PRF and Data Rate
	dw1000local.txFCTRL = ((conf->txPreambLength | conf->prf) << TX_FCTRL_TXPRF_SHFT) | (conf->dataRate << TX_FCTRL_TXBR_SHFT);
	write32bitreg(TX_FCTRL_ID, dw1000local.txFCTRL);

	// The SFD transmit pattern is initialised by the DW1000 upon a user TX request, but (due to an IC issue) it is not done for an auto-ACK TX. The
	// SYS_CTRL write below works around this issue, by simultaneously initiating and aborting a transmission, which correctly initialises the SFD
	// after its configuration or reconfiguration.
	// This issue is not documented at the time of writing this code. It should be in next release of DW1000 User Manual (v2.09, from July 2016).
	write8bitoffsetreg(SYS_CTRL_ID, SYS_CTRL_OFFSET, SYS_CTRL_TXSTRT | SYS_CTRL_TRXOFF); // Request TX start and TRX off at the same time
}

template < typename Spi, typename Cs, typename Reset, typename Irq >
int
xpcc::Dw1000< Spi, Cs, Reset, Irq >::initialise(uint16_t conf)
{
	uint16_t otp_addr = 0;
	uint32_t ldo_tune = 0;

	dw1000local.dblbuffon = 0; // Double buffer mode off by default
	dw1000local.wait4resp = 0;
	dw1000local.sleep_mode = 0;

	dw1000local.cbTxDone = NULL;
	dw1000local.cbRxOk = NULL;
	dw1000local.cbRxTo = NULL;
	dw1000local.cbRxErr = NULL;

	// Read and validate device ID return -1 if not recognised
	if (DEVICE_ID != readdevid()) // MP IC ONLY (i.e. DW1000) FOR THIS CODE
	{
		return ERROR ;
	}

	// Make sure the device is completely reset before starting initialisation
	softreset();
	//....................................
	_enableclocks(FORCE_SYS_XTI); //searchno: 18 // NOTE: set system clock to XTI - this is necessary to make sure the values read by _otpread are reliable

	// Configure the CPLL lock detect
	write8bitoffsetreg(EXT_SYNC_ID, EC_CTRL_OFFSET, EC_CTRL_PLLLCK);

	// Read OTP revision number
	otp_addr = _otpread(XTRIM_ADDRESS) & 0xffff;        // Read 32 bit value, XTAL trim val is in low octet-0 (5 bits)
	dw1000local.otprev = (otp_addr >> 8) & 0xff;            // OTP revision is next byte

	// Load LDO tune from OTP and kick it if there is a value actually programmed.
	ldo_tune = _otpread(LDOTUNE_ADDRESS);
	if((ldo_tune & 0xFF) != 0)
	{
		// Kick LDO tune
		write8bitoffsetreg(OTP_IF_ID, OTP_SF, OTP_SF_LDO_KICK); // Set load LDE kick bit
		dw1000local.sleep_mode |= AON_WCFG_ONW_LLDO; // LDO tune must be kicked at wake-up
	}

	// Load Part and Lot ID from OTP
	dw1000local.partID = _otpread(PARTID_ADDRESS);
	dw1000local.lotID = _otpread(LOTID_ADDRESS);

	// XTAL trim value is set in OTP for DW1000 module and EVK/TREK boards but that might not be the case in a custom design
	dw1000local.init_xtrim = otp_addr & 0x1F;
	if (!dw1000local.init_xtrim) // A value of 0 means that the crystal has not been trimmed
	{
		dw1000local.init_xtrim = FS_XTALT_MIDRANGE ; // Set to mid-range if no calibration value inside
	}
	// Configure XTAL trim
	setxtaltrim(dw1000local.init_xtrim);

	// Load leading edge detect code
	if(conf & LOADUCODE)
	{
		_loaducodefromrom();
		dw1000local.sleep_mode |= AON_WCFG_ONW_LLDE; // microcode must be loaded at wake-up
	}
	else // Should disable the LDERUN enable bit in 0x36, 0x4
	{
		uint16_t rega = read16bitoffsetreg(PMSC_ID, PMSC_CTRL1_OFFSET+1) ;
		rega &= 0xFDFF ; // Clear LDERUN bit
		write16bitoffsetreg(PMSC_ID, PMSC_CTRL1_OFFSET+1, rega) ;
	}

	_enableclocks(ENABLE_ALL_SEQ); // Enable clocks for sequencing

	// The 3 bits in AON CFG1 register must be cleared to ensure proper operation of the DW1000 in DEEPSLEEP mode.
	write8bitoffsetreg(AON_ID, AON_CFG1_OFFSET, 0x00);

	// Read system register / store local copy
	dw1000local.sysCFGreg = read32bitreg(SYS_CFG_ID) ; // Read sysconfig register

	return SUCCESS ;
}

template < typename Spi, typename Cs, typename Reset, typename Irq >
int
xpcc::Dw1000< Spi, Cs, Reset, Irq >::writetxdata(uint16_t txFrameLength, uint8_t *txFrameBytes, uint16_t txBufferOffset)
{
	if ((txBufferOffset + txFrameLength) <= 1024)
	{
		// Write the data to the IC TX buffer, (-2 bytes for auto generated CRC)
		writetodevice( TX_BUFFER_ID, txBufferOffset, txFrameLength-2, txFrameBytes);
		return SUCCESS;
	}
	else
	{
		return ERROR;
	}
}



template < typename Spi, typename Cs, typename Reset, typename Irq >
void
xpcc::Dw1000< Spi, Cs, Reset, Irq >::writetxfctrl(uint16_t txFrameLength, uint16_t txBufferOffset, int ranging)
{
	uint32_t reg32 = dw1000local.txFCTRL | txFrameLength | (txBufferOffset << TX_FCTRL_TXBOFFS_SHFT) | (ranging << TX_FCTRL_TR_SHFT);
	write32bitreg(TX_FCTRL_ID, reg32);
}

template < typename Spi, typename Cs, typename Reset, typename Irq >
void
xpcc::Dw1000< Spi, Cs, Reset, Irq >::write8bitoffsetreg(int regFileID, int regOffset, uint8_t regval)
{
	writetodevice(regFileID, regOffset, 1, &regval);
}

template < typename Spi, typename Cs, typename Reset, typename Irq >
int
xpcc::Dw1000< Spi, Cs, Reset, Irq >::starttx(TX_Mode mode)
{
	/*int retval = SUCCESS ;
	uint8_t temp  = 0x00;

	temp |= (uint8_t)SYS_CTRL_TXSTRT ;
	write8bitoffsetreg(SYS_CTRL_ID, SYS_CTRL_OFFSET, temp);


	return retval;*/
	int retval = SUCCESS ;
	uint8_t temp  = 0x00;
	uint16_t checkTxOK = 0 ;

	if(mode & RESPONSE_EXPECTED)
	{
		temp = (uint8_t)SYS_CTRL_WAIT4RESP ; // Set wait4response bit
		write8bitoffsetreg(SYS_CTRL_ID, SYS_CTRL_OFFSET, temp);
		//dw1000local.wait4resp = 1;
	}

	if (mode & START_TX_DELAYED)
	{
		// Both SYS_CTRL_TXSTRT and SYS_CTRL_TXDLYS to correctly enable TX
		temp |= (uint8_t)(SYS_CTRL_TXDLYS | SYS_CTRL_TXSTRT) ;
		write8bitoffsetreg(SYS_CTRL_ID, SYS_CTRL_OFFSET, temp);
		checkTxOK = read16bitoffsetreg(SYS_STATUS_ID, 3); // Read at offset 3 to get the upper 2 bytes out of 5
		if ((checkTxOK & SYS_STATUS_TXERR) == 0) // Transmit Delayed Send set over Half a Period away or Power Up error (there is enough time to send but not to power up individual blocks).
		{
			retval = SUCCESS ; // All okay
		}
		else
		{
			// I am taking DSHP set to Indicate that the TXDLYS was set too late for the specified DX_TIME.
			// Remedial Action - (a) cancel delayed send
			temp = (uint8_t)SYS_CTRL_TRXOFF; // This assumes the bit is in the lowest byte
			write8bitoffsetreg(SYS_CTRL_ID, SYS_CTRL_OFFSET, temp);
			// Note event Delayed TX Time too Late
			// Could fall through to start a normal send (below) just sending late.....
			// ... instead return and assume return value of 1 will be used to detect and recover from the issue.
			//dw1000local.wait4resp = 0;
			retval = ERROR ; // Failed !
		}
	}
	else
	{
		temp |= (uint8_t)SYS_CTRL_TXSTRT ;
		write8bitoffsetreg(SYS_CTRL_ID, SYS_CTRL_OFFSET, temp);
	}

	return retval;
}

template < typename Spi, typename Cs, typename Reset, typename Irq >
void
xpcc::Dw1000< Spi, Cs, Reset, Irq >::rxEnable()
{
	rxenable(START_RX_IMMEDIATE);
}


template < typename Spi, typename Cs, typename Reset, typename Irq >
bool
xpcc::Dw1000< Spi, Cs, Reset, Irq >::rxEnableAt(uint32_t time)
{
	setdelayedtrxtime(time);
	int result;
	result  = rxenable(RX_Mode::START_RX_DELAYED);
	if (result == SUCCESS){
		return true;
	}
	else {
		return false;
	}

}

template < typename Spi, typename Cs, typename Reset, typename Irq >
int
xpcc::Dw1000< Spi, Cs, Reset, Irq >::rxenable(RX_Mode mode)
{
	uint16_t temp ;
	uint8_t temp1 ;

	if ((mode & NO_SYNC_PTRS) == 0)
	{
		syncrxbufptrs();
	}

	temp = (uint16_t)SYS_CTRL_RXENAB ;

	if (mode & START_RX_DELAYED)
	{
		temp |= (uint16_t)SYS_CTRL_RXDLYE ;
	}

	write16bitoffsetreg(SYS_CTRL_ID, SYS_CTRL_OFFSET, temp);

	if (mode & START_RX_DELAYED) // check for errors
	{
		temp1 = read8bitoffsetreg(SYS_STATUS_ID, 3); // Read 1 byte at offset 3 to get the 4th byte out of 5
		if ((temp1 & (SYS_STATUS_HPDWARN >> 24)) != 0) // if delay has passed do immediate RX on unless IDLE_ON_DLY_ERR is true
		{
			forcetrxoff(); // turn the delayed receive off

			if((mode & IDLE_ON_DLY_ERR) == 0) // if IDLE_ON_DLY_ERR not set then re-enable receiver
			{
				write16bitoffsetreg(SYS_CTRL_ID, SYS_CTRL_OFFSET, SYS_CTRL_RXENAB);
			}
			return ERROR; // return warning indication
		}
	}

	return SUCCESS;
} // end rxenable()


template < typename Spi, typename Cs, typename Reset, typename Irq >
void
xpcc::Dw1000< Spi, Cs, Reset, Irq >::readrxdata(uint8_t *buffer, uint16_t length, uint16_t rxBufferOffset)
{
	readfromdevice(RX_BUFFER_ID,rxBufferOffset,length,buffer) ;
}

template < typename Spi, typename Cs, typename Reset, typename Irq >
uint32_t
xpcc::Dw1000< Spi, Cs, Reset, Irq >::read32bitreg(uint16_t addr)
{
	return read32bitoffsetreg(addr, 0);
}

template < typename Spi, typename Cs, typename Reset, typename Irq >
uint16_t
xpcc::Dw1000< Spi, Cs, Reset, Irq >::read16bitoffsetreg(int regFileID,int regOffset)
{
	uint16_t  regval = 0 ;
	uint8_t   buffer[2] ;

	readfromdevice(regFileID,regOffset,2,buffer); // Read 2 bytes (16-bits) register into buffer

	regval = (buffer[1] << 8) + buffer[0] ;
	return regval ;
}

template < typename Spi, typename Cs, typename Reset, typename Irq >
uint32_t
xpcc::Dw1000< Spi, Cs, Reset, Irq >::read32bitoffsetreg(int regFileID, int regOffset)
{
	uint32_t  regval = 0 ;
	int     j ;
	uint8_t   buffer[4] ;

	readfromdevice(regFileID,regOffset,4,buffer); // Read 4 bytes (32-bits) register into buffer

	for (j = 3 ; j >= 0 ; j --)
	{
		regval = (regval << 8) + buffer[j] ;
	}
	return regval ;

}

template < typename Spi, typename Cs, typename Reset, typename Irq >
void
xpcc::Dw1000< Spi, Cs, Reset, Irq >::writetodevice(uint16_t recordNumber, uint16_t index, uint32_t length , const uint8_t *buffer)
{
	uint8_t header[3];
	int cnt = 0;

	if (index == 0)
	{
		header[cnt++] = 0x80 | recordNumber ;
	}
	else
	{
		header[cnt++] = 0xC0 | recordNumber ;
		if (index <= 127)
		{
			header[cnt++] = (uint8_t) index;
		}
		else
		{
			header[cnt++] = 0x80 | (uint8_t)(index);
			header[cnt++] = (uint8_t) (index >> 7);
		}
	}
	writetospi(cnt, header, length, buffer);
}

template < typename Spi, typename Cs, typename Reset, typename Irq >
void
xpcc::Dw1000< Spi, Cs, Reset, Irq >::write16bitoffsetreg(int regFileID,int regOffset,uint16_t regval)
{
	uint8_t buffer[2];
	buffer[0] = regval & 0xFF;
	buffer[1] = regval >> 8;
	writetodevice(regFileID, regOffset, 2, buffer);
}

template < typename Spi, typename Cs, typename Reset, typename Irq >
void
xpcc::Dw1000< Spi, Cs, Reset, Irq >::write32bitoffsetreg(int regFileID, int regOffset, uint32_t regval)
{
	int j;
	uint8_t buffer[4];

	for( j = 0 ; j < 4 ; j++)
	{
		buffer[j] = regval & 0xff ;
		regval >>= 8;
	}

	writetodevice(regFileID, regOffset,4, buffer);
}

template < typename Spi, typename Cs, typename Reset, typename Irq >
void
xpcc::Dw1000< Spi, Cs, Reset, Irq >::write32bitreg(int x, uint32_t y)
{
	write32bitoffsetreg(x,0,y);
}

template < typename Spi, typename Cs, typename Reset, typename Irq >
void
xpcc::Dw1000< Spi, Cs, Reset, Irq >::readfromdevice(uint16_t  recordNumber, uint16_t  index, uint32_t  length, uint8_t buffer[])
{
	uint8_t header[3] ; // Buffer to compose header in
	uint16_t  cnt = 0; // Counter for length of header

	// Write message header selecting READ operation and addresses as appropriate (this is one to three bytes long)
	if (index == 0) // For index of 0, no sub-index is required
	{
		header[cnt++] = (uint8_t) recordNumber ; // Bit-7 zero is READ operation, bit-6 zero=NO sub-addressing, bits 5-0 is reg file id
	}
	else
	{
		header[cnt++] = (uint8_t)(0x40 | recordNumber) ; // Bit-7 zero is READ operation, bit-6 one=sub-address follows, bits 5-0 is reg file id

		if (index <= 127) // For non-zero index < 127, just a single sub-index byte is required
		{
			header[cnt++] = (uint8_t) index ; // Bit-7 zero means no extension, bits 6-0 is index.
		}
		else
		{
			header[cnt++] = 0x80 | (uint8_t)(index) ; // Bit-7 one means extended index, bits 6-0 is low seven bits of index.
			header[cnt++] =  (uint8_t) (index >> 7) ; // 8-bit value = high eight bits of index.
		}
	}

	// Do the read from the SPI
	readfromspi(cnt, header, length, buffer);  // result is stored in the buffer
}

template < typename Spi, typename Cs, typename Reset, typename Irq >
bool
xpcc::Dw1000< Spi, Cs, Reset, Irq >::init(config_t &conf){
	Reset::configure(Reset::OutputType::OpenDrain);
	Reset::setOutput();
	Reset::reset();

	xpcc::delayMilliseconds(5);
	Reset::set();
	xpcc::delayMilliseconds(1);

	if (initialise(LOADUCODE) == ERROR)
	{
		XPCC_LOG_DEBUG   << "INIT FAILED\n" ;
		return false;
	}
	else{
		configure(&conf);
		return true;
	}

}

template < typename Spi, typename Cs, typename Reset, typename Irq >
uint32_t
xpcc::Dw1000< Spi, Cs, Reset, Irq >::readTXTimestamplo32(){
	return read32bitreg(TX_TIME_ID);
}

template < typename Spi, typename Cs, typename Reset, typename Irq >
uint32_t
xpcc::Dw1000< Spi, Cs, Reset, Irq >::readTXTimestamphi32(){
	return read32bitoffsetreg(TX_TIME_ID,1);
}



template < typename Spi, typename Cs, typename Reset, typename Irq >
uint8_t
xpcc::Dw1000< Spi, Cs, Reset, Irq >::read8bitoffsetreg(int regFileID, int regOffset){

	uint8_t regval;
	readfromdevice(regFileID, regOffset, 1, &regval);
	return regval ;
}

template < typename Spi, typename Cs, typename Reset, typename Irq >
void
xpcc::Dw1000< Spi, Cs, Reset, Irq >::readsystime(uint8_t timestamp[])
{
	readfromdevice(SYS_TIME_ID, SYS_TIME_OFFSET, SYS_TIME_LEN, timestamp);
}

template < typename Spi, typename Cs, typename Reset, typename Irq >
uint32_t
xpcc::Dw1000< Spi, Cs, Reset, Irq >::readsystimestamphi32(void)
{
	return read32bitoffsetreg(SYS_TIME_ID, 1); // Offset is 1 to get the 4 upper bytes out of 5
}

template < typename Spi, typename Cs, typename Reset, typename Irq >
uint32_t
xpcc::Dw1000< Spi, Cs, Reset, Irq >::readsystimestamplo32(void)
{
	return read32bitoffsetreg(SYS_TIME_ID, 0);
}


template < typename Spi, typename Cs, typename Reset, typename Irq >
uint32_t
xpcc::Dw1000< Spi, Cs, Reset, Irq >::readRXTimestamplo32(void)
{
	return read32bitreg(RX_TIME_ID); // Read RX TIME as a 32-bit register to get the 4 lower bytes out of 5
}

template < typename Spi, typename Cs, typename Reset, typename Irq >
uint32_t
xpcc::Dw1000< Spi, Cs, Reset, Irq >::readRXTimestamphi32(void)
{
	return read32bitoffsetreg(RX_TIME_ID,1); // Read RX TIME as a 32-bit register to get the 4 higher bytes out of 5
}


template < typename Spi, typename Cs, typename Reset, typename Irq >
void
xpcc::Dw1000< Spi, Cs, Reset, Irq >::setTXAntennaDelay(uint16_t txDelay)
{
	TX_ANT_DLY = txDelay;
	write16bitoffsetreg(TX_ANTD_ID, TX_ANTD_OFFSET, txDelay);
}

template < typename Spi, typename Cs, typename Reset, typename Irq >
void
xpcc::Dw1000< Spi, Cs, Reset, Irq >::setRXAntennaDelay(uint16_t rxDelay)
{
	// Set the RX antenna delay for auto TX timestamp adjustment
	RX_ANT_DLY = rxDelay;
	write16bitoffsetreg(LDE_IF_ID, LDE_RXANTD_OFFSET, rxDelay);
}


template < typename Spi, typename Cs, typename Reset, typename Irq >
void
xpcc::Dw1000< Spi, Cs, Reset, Irq >::setrxaftertxdelay(uint32_t rxDelayTime)
{
	uint32_t val = read32bitreg(ACK_RESP_T_ID) ; // Read ACK_RESP_T_ID register

	val &= ~(ACK_RESP_T_W4R_TIM_MASK) ; // Clear the timer (19:0)

	val |= (rxDelayTime & ACK_RESP_T_W4R_TIM_MASK) ; // In UWB microseconds (e.g. turn the receiver on 20uus after TX)

	write32bitreg(ACK_RESP_T_ID, val) ;
}

template < typename Spi, typename Cs, typename Reset, typename Irq >
void
xpcc::Dw1000< Spi, Cs, Reset, Irq >::setrxtimeout(uint16_t time)
{
	uint8_t temp ;

	temp = read8bitoffsetreg(SYS_CFG_ID, 3); // Read at offset 3 to get the upper byte only

	if(time > 0)
	{
		write16bitoffsetreg(RX_FWTO_ID, RX_FWTO_OFFSET, time) ;

		temp |= (uint8_t)(SYS_CFG_RXWTOE>>24); // Shift RXWTOE mask as we read the upper byte only
		// OR in 32bit value (1 bit set), I know this is in high byte.
		dw1000local.sysCFGreg |= SYS_CFG_RXWTOE;

		write8bitoffsetreg(SYS_CFG_ID, 3, temp); // Write at offset 3 to write the upper byte only
	}
	else
	{
		temp &= ~((uint8_t)(SYS_CFG_RXWTOE>>24)); // Shift RXWTOE mask as we read the upper byte only
		// AND in inverted 32bit value (1 bit clear), I know this is in high byte.
		dw1000local.sysCFGreg &= ~(SYS_CFG_RXWTOE);

		write8bitoffsetreg(SYS_CFG_ID, 3, temp); // Write at offset 3 to write the upper byte only
	}

}

template < typename Spi, typename Cs, typename Reset, typename Irq >
void
xpcc::Dw1000< Spi, Cs, Reset, Irq >::rxreset(void)
{
	// Set RX reset
	write8bitoffsetreg(PMSC_ID, PMSC_CTRL0_SOFTRESET_OFFSET, PMSC_CTRL0_RESET_RX);

	// Clear RX reset
	write8bitoffsetreg(PMSC_ID, PMSC_CTRL0_SOFTRESET_OFFSET, PMSC_CTRL0_RESET_CLEAR);
}


template < typename Spi, typename Cs, typename Reset, typename Irq >
void
xpcc::Dw1000< Spi, Cs, Reset, Irq >::readRXTimestamp(uint8_t timestamp[])
{
	readfromdevice(RX_TIME_ID, RX_TIME_RX_STAMP_OFFSET, RX_TIME_RX_STAMP_LEN, timestamp) ; // Get the adjusted time of arrival
}

template < typename Spi, typename Cs, typename Reset, typename Irq >
void
xpcc::Dw1000< Spi, Cs, Reset, Irq >::readTXTimestamp(uint8_t timestamp[])
{
	readfromdevice(TX_TIME_ID, TX_TIME_TX_STAMP_OFFSET, TX_TIME_TX_STAMP_LEN, timestamp) ; // Get the adjusted time of arrival
}

template < typename Spi, typename Cs, typename Reset, typename Irq >
void
xpcc::Dw1000< Spi, Cs, Reset, Irq >::setdelayedtrxtime(uint32_t starttime)
{
	write32bitoffsetreg(DX_TIME_ID, 1, starttime); // Write at offset 1 as the lower 9 bits of this register are ignored

}

template < typename Spi, typename Cs, typename Reset, typename Irq >
void
xpcc::Dw1000< Spi, Cs, Reset, Irq >::syncrxbufptrs(void)
{
	uint8_t  buff ;
	// Need to make sure that the host/IC buffer pointers are aligned before starting RX
	buff = read8bitoffsetreg(SYS_STATUS_ID, 3); // Read 1 byte at offset 3 to get the 4th byte out of 5

	if((buff & (SYS_STATUS_ICRBP >> 24)) !=     // IC side Receive Buffer Pointer
			((buff & (SYS_STATUS_HSRBP>>24)) << 1) ) // Host Side Receive Buffer Pointer
	{
		write8bitoffsetreg(SYS_CTRL_ID, SYS_CTRL_HRBT_OFFSET , 0x01) ; // We need to swap RX buffer status reg (write one to toggle internally)

	}
}

template < typename Spi, typename Cs, typename Reset, typename Irq >
void
xpcc::Dw1000< Spi, Cs, Reset, Irq >::forcetrxoff(void)
{
	decaIrqStatus_t stat ;
	uint32_t mask;
	mask = read32bitreg(SYS_MASK_ID) ; // Read set interrupt mask
	// Need to beware of interrupts occurring in the middle of following read modify write cycle
	// We can disable the radio, but before the status is cleared an interrupt can be set (e.g. the
	// event has just happened before the radio was disabled)
	// thus we need to disable interrupt during this operation
	stat = decamutexon() ;

	write32bitreg(SYS_MASK_ID, 0) ; // Clear interrupt mask - so we don't get any unwanted events

	write8bitoffsetreg(SYS_CTRL_ID, SYS_CTRL_OFFSET, (uint8_t)SYS_CTRL_TRXOFF) ; // Disable the radio

	// Forcing Transceiver off - so we do not want to see any new events that may have happened
	write32bitreg(SYS_STATUS_ID, (SYS_STATUS_ALL_TX | SYS_STATUS_ALL_RX_ERR | SYS_STATUS_ALL_RX_TO | SYS_STATUS_ALL_RX_GOOD));

	syncrxbufptrs();

	write32bitreg(SYS_MASK_ID, mask) ; // Set interrupt mask to what it was

	// Enable/restore interrupts again...
	decamutexoff(stat) ;
	//dw1000local.wait4resp = 0;

}

template < typename Spi, typename Cs, typename Reset, typename Irq >
uint64_t
xpcc::Dw1000< Spi, Cs, Reset, Irq >::get_rx_timestamp_u64(void)
{
	uint8_t ts_tab[5];
	uint64_t ts = 0;
	int i;
	readRXTimestamp(ts_tab);
	for (i = 4; i >= 0; i--)
	{
		ts <<= 8;
		ts |= ts_tab[i];
	}
	return ts;
}

template < typename Spi, typename Cs, typename Reset, typename Irq >
xpcc::dw1000::decaIrqStatus_t
xpcc::Dw1000< Spi, Cs, Reset, Irq >::decamutexon()
{
	Irq::disableExternalInterrupt();
	return 1;
}

template < typename Spi, typename Cs, typename Reset, typename Irq >
void
xpcc::Dw1000< Spi, Cs, Reset, Irq >::decamutexoff(decaIrqStatus_t s)
{
	if (s == 0) {
		Irq::enableExternalInterrupt();
	}
}

