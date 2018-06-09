/**
 * Copyright (c) 2018, Marten Junga (Github.com/Maju-Ketchup)
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 *
 */
#ifndef _DECA_PARAMS_
#define _DECA_PARAMS_
#include <xpcc/architecture/platform.hpp>
namespace xpcc
{
/*
 *
 * @ingroup driver_radio
 * @defgroup dw1000 DW1000
 */
struct dw1000{
public:
	static constexpr int SUCCESS 	=  0;
	static constexpr int ERROR 		= -1;

	/* UWB microsecond (uus) to Decawave device time unit (dtu, around 15.65 ps) conversion factor.
	 * 1 uus = 512 / 499.2 µs and 1 µs = 499.2 * 128 dtu. */
	static constexpr uint32_t	UUS_TO_TIME_UNIT		= 65536;
	static constexpr float		UUS_TO_MICROS			= (4492e-1/512e0);
	static constexpr float		TIME_UNIT_TO_S			= (1e0/499.2e6/128e0);// *UUS_TO_MICROS;
	static constexpr uint32_t	standardAntennaDelay	= 16436;


	enum class IRQreason : uint8_t{
		RX_Complete = 1,
		TX_Complete = 2,
		RX_Error	= 3,
		Sync		= 4,
		Unknown		= 0
	};

	enum PRF : uint8_t{
		PRF_64M		= 2,
		PRF_16M		= 1
	};
	enum PLEN : uint8_t{
		PLEN_4096  = 0x0C,		//!< Standard preamble length 4096 symbols
		PLEN_2048  = 0x28,		//!< Non-standard preamble length 2048 symbols
		PLEN_1536  = 0x18,		//!< Non-standard preamble length 1536 symbols
		PLEN_1024  = 0x08,		//!< Standard preamble length 1024 symbols
		PLEN_512   = 0x34,		//!< Non-standard preamble length 512 symbols
		PLEN_256   = 0x24,		//!< Non-standard preamble length 256 symbols
		PLEN_128   = 0x14,		//!< Non-standard preamble length 128 symbols
		PLEN_64    = 0x04		//!< Standard preamble length 64 symbols
	};
	enum PAC: uint8_t{
		PAC8	= 0,
		PAC16	= 1,
		PAC32	= 2,
		PAC64	= 3
	};
	enum BR: uint8_t{

		BR_110K		= 0,
		BR_850K		= 1,
		BR_6M8		= 2
	};

	enum PHRMODE: uint8_t{
		PHRMODE_STD		= 0x0,     //< standard PHR mode
		PHRMODE_EXT		= 0x3     //< DW proprietary extended frames PHR mode
	};

	enum Loadcode: uint8_t{
		LOADUCODE	= 0x1,
		LOADNONE	= 0x0
	};

	static constexpr uint16_t SFDTOC_DEF	= 0x1041;

	typedef int decaIrqStatus_t;
	typedef struct
	{
		uint8_t chan ;          //!< channel number {1, 2, 3, 4, 5, 7 }
		PRF prf ;               //!< Pulse Repetition Frequency {PRF_16M or PRF_64M}
		PLEN txPreambLength ;   //!< PLEN_64..PLEN_4096
		PAC rxPAC ;             //!< Acquisition Chunk Size (Relates to RX preamble length)
		uint8_t txCode ;        //!< TX preamble code
		uint8_t rxCode ;        //!< RX preamble code
		uint8_t nsSFD ;         //!< Boolean should we use non-standard SFD for better performance
		BR dataRate ;           //!< Data Rate {BR_110K, BR_850K or BR_6M8}
		PHRMODE phrMode ;       //!< PHR mode {0x0 - standard PHRMODE_STD, 0x3 - extended frames PHRMODE_EXT}
		uint16_t sfdTO ;        //!< SFD timeout value (in symbols)
	} config_t ;

	typedef struct
	{
		uint32_t status;      //<initial value of register as ISR is entered
		uint16_t datalength;  //<length of frame
		uint8_t  fctrl[2];    //<frame control bytes
		uint8_t  rx_flags;    //<RX frame flags, see above

	} cb_data_t;

	// Call-back type for all events
	typedef void (*cb_t)(const cb_data_t *);

	typedef struct
	{
		uint32_t      partID ;            // <IC Part ID - read during initialisation
		uint32_t      lotID ;             // <IC Lot ID - read during initialisation
		uint8_t       longFrames ;        // <Flag in non-standard long frame mode
		uint8_t       otprev ;            // <OTP revision number (read during initialisation)
		uint32_t      txFCTRL ;           // <Keep TX_FCTRL register config
		uint8_t       init_xtrim;         // <initial XTAL trim value read from OTP (or defaulted to mid-range if OTP not programmed)
		uint8_t       dblbuffon;          // <Double RX buffer mode flag
		uint32_t      sysCFGreg ;         // <Local copy of system config register
		uint16_t      sleep_mode;         // <Used for automatic reloading of LDO tune and microcode at wake-up
		uint8_t       wait4resp ;         // <wait4response was set with last TX start command
		cb_data_t	  cbData;	          // <Callback data structure
		cb_t    	  cbTxDone;           // <Callback for TX confirmation event
		cb_t          cbRxOk;			  // <Callback for RX good frame event
		cb_t          cbRxTo;			  // <Callback for RX timeout events
		cb_t          cbRxErr;			  // <Callback for RX error events
	} local_data_t ;

	static local_data_t dw1000local;

private:
#include "./deca_regs.hpp"

	enum TX_Mode{
		START_TX_IMMEDIATE			= 0,
		RESPONSE_EXPECTED			= 2,
		START_TX_DELAYED			= 1
	};
	enum RX_Mode{
		START_RX_IMMEDIATE			= 0,
		START_RX_DELAYED			= 1,   //< Set up delayed RX, if "late" error triggers, then the RX will be enabled immediately
		IDLE_ON_DLY_ERR				= 2,   //< If delayed RX failed due to "late" error then if this
		NO_SYNC_PTRS				= 4    //< Do not try to sync IC side and Host side buffer pointers when enabling RX. This is used to perform manual RX

	};


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




	static constexpr int NUM_BR = 3;
	static constexpr int NUM_PRF= 2;
	static constexpr int NUM_PACS= 4;
	static constexpr int NUM_BW =2;            //<2 bandwidths are supported
	static constexpr int NUM_SFD =2;           //<supported number of SFDs - standard = 0, non-standard = 1
	static constexpr int NUM_CH =6  ;          //<supported channels are 1, 2, 3, 4, 5, 7
	static constexpr int NUM_CH_SUPPORTED =8;  //<supported channels are '0', 1, 2, 3, 4, 5, '6', 7
	static constexpr int PCODES =25;           //<supported preamble codes


	typedef struct {
		uint32_t lo32;
		uint16_t target[NUM_PRF];
	} agc_cfg_struct ;


	//SFD threshold settings for 110k, 850k, 6.8Mb standard and non-standard
	static constexpr float XMLPARAMS_VERSION  = (1.17f);
	static constexpr uint8_t  PEAK_MULTPLIER =  (0x60); //<3 -> (0x3 * 32) & 0x00E0
	static constexpr uint8_t  N_STD_FACTOR   = (13);
	static constexpr uint8_t  LDE_PARAM1     = (PEAK_MULTPLIER | N_STD_FACTOR);

	static constexpr uint16_t  LDE_PARAM3_16 =(0x1607);
	static constexpr uint16_t  LDE_PARAM3_64 =(0x0607);




	//-----------------------------------------
	// map the channel number to the index in the configuration arrays below
	// 0th element is chan 1, 1st is chan 2, 2nd is chan 3, 3rd is chan 4, 4th is chan 5, 5th is chan 7
	static constexpr uint8_t chan_idx[NUM_CH_SUPPORTED] = {0, 0, 1, 2, 3, 4, 0, 5};

	//-----------------------------------------
	static constexpr uint32_t tx_config[NUM_CH] =
	{
		RF_TXCTRL_CH1,
		RF_TXCTRL_CH2,
		RF_TXCTRL_CH3,
		RF_TXCTRL_CH4,
		RF_TXCTRL_CH5,
		RF_TXCTRL_CH7,
	};

	//Frequency Synthesiser - PLL configuration
	static constexpr uint32_t fs_pll_cfg[NUM_CH] =
	{
		FS_PLLCFG_CH1,
		FS_PLLCFG_CH2,
		FS_PLLCFG_CH3,
		FS_PLLCFG_CH4,
		FS_PLLCFG_CH5,
		FS_PLLCFG_CH7
	};

	//Frequency Synthesiser - PLL tuning
	static constexpr uint8_t fs_pll_tune[NUM_CH] =
	{
		FS_PLLTUNE_CH1,
		FS_PLLTUNE_CH2,
		FS_PLLTUNE_CH3,
		FS_PLLTUNE_CH4,
		FS_PLLTUNE_CH5,
		FS_PLLTUNE_CH7
	};

	//bandwidth configuration
	static constexpr uint8_t rx_config[NUM_BW] =
	{
		RF_RXCTRLH_NBW,
		RF_RXCTRLH_WBW
	};


	static constexpr agc_cfg_struct agc_config =
	{
		AGC_TUNE2_VAL,
		{ AGC_TUNE1_16M , AGC_TUNE1_64M }  //adc target
	};

	//DW non-standard SFD length for 110k, 850k and 6.81M
	static constexpr uint8_t dwnsSFDlen[NUM_BR] =
	{
		DW_NS_SFD_LEN_110K,
		DW_NS_SFD_LEN_850K,
		DW_NS_SFD_LEN_6M8
	};

	// SFD Threshold
	static constexpr uint16_t sftsh[NUM_BR][NUM_SFD] =
	{
		{
			DRX_TUNE0b_110K_STD,
			DRX_TUNE0b_110K_NSTD
		},
		{
			DRX_TUNE0b_850K_STD,
			DRX_TUNE0b_850K_NSTD
		},
		{
			DRX_TUNE0b_6M8_STD,
			DRX_TUNE0b_6M8_NSTD
		}
	};

	static constexpr uint16_t dtune1[NUM_PRF] =
	{
		DRX_TUNE1a_PRF16,
		DRX_TUNE1a_PRF64
	};

	static constexpr uint32_t digital_bb_config[NUM_PRF][NUM_PACS] =
	{
		{
			DRX_TUNE2_PRF16_PAC8,
			DRX_TUNE2_PRF16_PAC16,
			DRX_TUNE2_PRF16_PAC32,
			DRX_TUNE2_PRF16_PAC64
		},
		{
			DRX_TUNE2_PRF64_PAC8,
			DRX_TUNE2_PRF64_PAC16,
			DRX_TUNE2_PRF64_PAC32,
			DRX_TUNE2_PRF64_PAC64
		}
	};

	static constexpr uint16_t lde_replicaCoeff[PCODES] =
	{
		0, // No preamble code 0
		LDE_REPC_PCODE_1,
		LDE_REPC_PCODE_2,
		LDE_REPC_PCODE_3,
		LDE_REPC_PCODE_4,
		LDE_REPC_PCODE_5,
		LDE_REPC_PCODE_6,
		LDE_REPC_PCODE_7,
		LDE_REPC_PCODE_8,
		LDE_REPC_PCODE_9,
		LDE_REPC_PCODE_10,
		LDE_REPC_PCODE_11,
		LDE_REPC_PCODE_12,
		LDE_REPC_PCODE_13,
		LDE_REPC_PCODE_14,
		LDE_REPC_PCODE_15,
		LDE_REPC_PCODE_16,
		LDE_REPC_PCODE_17,
		LDE_REPC_PCODE_18,
		LDE_REPC_PCODE_19,
		LDE_REPC_PCODE_20,
		LDE_REPC_PCODE_21,
		LDE_REPC_PCODE_22,
		LDE_REPC_PCODE_23,
		LDE_REPC_PCODE_24
	};
};
}
#endif
