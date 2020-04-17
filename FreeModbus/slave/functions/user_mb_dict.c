#include "mbframe.h"
#include "user_mb_dict.h"
#include "app_val.h"

/***********************************************************************
                               OBJECT DICTIONARY
************************************************************************/

sMBSlaveRegData Slave_Protocol0_RegHoldBuf[]=             //保持寄存器字典
{
    {0, uint16,  0,  65535, 0, RO,  (void*)&CHID         },   
    {1, uint16,  0,  65535, 0, RO,  (void*)&CHProtocolVer},
	{3, uint16, 85,    170, 0, RW,  (void*)&CHCommRunCtrl},
	{4, uint16,  1,      6, 0, RW,  (void*)&CHRunMode    },
	{5, uint16, 50,    150, 0, RW,  (void*)&CHCHWOTSP    },   //5
	
    {9,   uint16,    0,     1,  0, RO, (void*)&CHRemoteEn       },   
	{23,  uint16,   20,    50,  0, RO, (void*)&CWAntiFreezeT    },
	{27,  uint16,    3,    30,  0, RO, (void*)&MinStartStopInv  },
	{46,   int16, -200,   500,  0, RO, (void*)&FullLoadDesEvapT },
    {180,  int16, -300,  1050,  0, RO, (void*)&CHCHWIT          },
	
    {181,  int16, -300, 1050, 0, RO, (void*)&CHCHWOT   },
	{182,  int16, -300, 1050, 0, RO, (void*)&CHCWIT    },
	{183,  int16, -300, 1050, 0, RO, (void*)&CHCWOT    },
    {186, uint16,    0, 1200, 0, RO, (void*)&CompDisT  },       
    {187,  int16, -300, 1500, 0, RO, (void*)&CompOilT  },      //15
	
    {192, uint16,     0,  2000, 0, RO, (void*)&MotorWindT},
	{194,  int16, -1100,  2000, 0, RO, (void*)&CHCondT   },
	{195,  int16, -1100,  2000, 0, RO, (void*)&CHEvapT   },
	{196, uint16,     0,  2500, 0, RO, (void*)&CHCondP   },    //20		
	{197, uint16,     0,  1600, 0, RO, (void*)&CHEvapP   }, 
	
    {202,  int16, -2500, 2500, 0, RO, (void*)&OilPDif   },
	{204, uint16,     0, 1000, 0, RO, (void*)&GVaneAg   },
	{212, uint16,     0, 5000, 0, RO, (void*)&CHLoad    },
	{220, uint16,     0,    6, 0, RO, (void*)&CHState   },     //25
    {240, uint16,     0,10000, 0, RO, (void*)&CHWhV     },
	
    {246, uint16,  0, 25000, 0, RO, (void*)&CHWhC_U     }, 
	{247, uint16,  0, 25000, 0, RO, (void*)&CHWhC_V     },
	{248, uint16,  0, 25000, 0, RO, (void*)&CHWhC_W     },
	{316, uint16,  0, 65535, 0, RO, (void*)&CHRuntime   },
	{319, uint16,  0, 65535, 0, RO, (void*)&CHStartTimes},     //30
    
    {342, uint16,   0, 65535, 0, RO, (void*)&CHWhP   },
    {343, uint16,   0, 65535, 0, RO, (void*)&CHWhW   },	
	{800, uint16,   0, 65535, 0, RO, (void*)&CHID    }, 
};

sMBSlaveBitData Slave_Protocol0_CoilBuf[]=                //线圈字典
{
    {17,  RO,  (UCHAR*)&CHFaultStateClear },
	{33,  RO,  (UCHAR*)&CHCommHMIFault    },
	{37,  RO,  (UCHAR*)&CHWSwState        },
	{38,  RO,  (UCHAR*)&CWSwState         },
    {43,  RO,  (UCHAR*)&CHRunState        },
	
	{44,  RO,  (UCHAR*)&CHCom1RunState},
	{47,  RO,  (UCHAR*)&CHProtectFlag },   
	{76,  RO,  (UCHAR*)&CHRYWPState   },
};

sMBSlaveCPNData Slave_Protocol0_CPNBuf[]=             //CPN变量字典
{
	{0x200,  uint8, CPN_UINT32, 0,       1, RO,  1, (void*)&CHRunState   },
	{0x202, uint16, CPN_UINT32, 0,       1, RO,  1, (void*)&CHRemoteEn   },
	{0x206, uint16, CPN_UINT32, 0, 3932100, RO, 60, (void*)&CHRuntime    },
	{0x207, uint16, CPN_UINT32, 0,   65535, RO,  1, (void*)&CHStartTimes },
	{0x208,  uint8, CPN_UINT32, 0,       1, RW,  1, (void*)&CHControl    },  
	
	{0x210, int16,  CPN_FLOAT,  -1100, 2000, RO, 10, (void*)&CHEvapT  },
	{0x211, int16,  CPN_FLOAT,  -1100, 2000, RO, 10, (void*)&CHCondT  },
	{0x214, int16,  CPN_FLOAT,   -300, 1050, RO, 10, (void*)&CHCHWOT  },
	{0x215, int16,  CPN_FLOAT,   -300, 1050, RO, 10, (void*)&CHCHWIT  },
	{0x216, int16,  CPN_FLOAT,   -300, 1050, RO, 10, (void*)&CHCWOT   },   //10
	
	{0x217,   int16,  CPN_FLOAT,  -300,  1050, RO, 10, (void*)&CHCWIT   },
	{0x218,  uint16,  CPN_FLOAT,    0,  1600,  RO,  1, (void*)&CHCHWIP_B },
	{0x219,  uint16,  CPN_FLOAT,    0,  1600,  RO,  1, (void*)&CHCHWOP   },
	{0x21A,  uint16,  CPN_FLOAT,    0,  1600,  RO,  1, (void*)&CHCWIP_B  },
	{0x21B,  uint16,  CPN_FLOAT,    0,  1600,  RO,  1, (void*)&CHCWOP    },
	
	{0x21C,  int16,  CPN_FLOAT,    50,   150,  RW,  10, (void*)&CHCHWOTSP  },
	{0x220, uint16,  CPN_FLOAT,     0, 65535,  RO,   1, (void*)&CHWhP      },
	{0x221, uint16,  CPN_FLOAT,     0, 65535,  RO,   1, (void*)&CHCoolCap  },
    {0x224, uint32,  CPN_FLOAT,     0,  2^32,  RO,   1, (void*)&CHWhW      },
    {0x228, uint16,  CPN_FLOAT,     0,  5000,  RO,  10, (void*)&CHWhCRatio }, //20
	
    {0x230, uint16, CPN_UINT32,   0, 32, RO, 1, (void*)&CHFaultState },  
	{0x242,  uint8, CPN_UINT32,   0,  1, RO, 1, (void*)&CHWV_OpenedState },
	{0x243,  uint8, CPN_UINT32,   0,  1, RO, 1, (void*)&CWV_OpenedState  },
    {0x244,  uint8, CPN_UINT32,   0,  1, RO, 1, (void*)&CHWSwState       },
    {0x245,  uint8, CPN_UINT32,   0,  1, RO, 1, (void*)&CWSwState        },
	
    {0x246,  uint8, CPN_UINT32,  0,    1, RO, 1, (void*)&CHWV_RemoteEn   },
	{0x247,  uint8, CPN_UINT32,  0,    1, RO, 1, (void*)&CWV_RemoteEn    },
	{0x24A,  uint8, CPN_UINT32,  0,    1, RW, 1, (void*)&CHWV_Control    },
	{0x24B,  uint8, CPN_UINT32,  0,    1, RW, 1, (void*)&CWV_Control     },
	{0x270, uint16,  CPN_FLOAT,  0, 1600, RO, 1, (void*)&CHCHWIP_F        },   //30
	
	{0x271,  uint16,  CPN_FLOAT,    0,  1600, RO, 1, (void*)&CHCWIP_F    },           	
	{0x280,  uint16,  CPN_FLOAT,    0,  1600, RO, 1, (void*)&CHEvapP     },
	{0x281,  uint16,  CPN_FLOAT,    0,  2500, RO, 1, (void*)&CHCondP     },      
	{0x283,  uint16,  CPN_FLOAT,    0,  1200, RO, 10, (void*)&CompDisT   },
	{0x284,   int16,  CPN_FLOAT, -300,  1500, RO, 10, (void*)&CompOilT   },
	
	{0x286,  uint16,  CPN_FLOAT,     0,  2000, RO, 10, (void*)&MotorWindT   },
    {0x288,  uint16,  CPN_FLOAT,     0, 65535, RO,  1, (void*)&CHWhV        },
    {0x289,  uint16,  CPN_FLOAT,     0, 25000, RO, 10, (void*)&CHWhC        },
	{0x28A,   int16,  CPN_FLOAT, -2500,  2500, RO,  1, (void*)&OilPDif      },    
	{0x28B,  uint16,  CPN_FLOAT,     0,  1000, RO, 10, (void*)&GVaneAg      },   //40
	
	{0x311,  uint16,  CPN_FLOAT,   100, 10000, RO,  1, (void*)&CHRateCoolCap     },  
    {0x313,   int16,  CPN_FLOAT,  -200,   500, RO, 10, (void*)&FullLoadDesEvapT  },
	{0x319,  uint16,  CPN_FLOAT,    20,    50, RW, 10, (void*)&CWAntiFreezeT     }, 
    {0x31A,  uint16,  CPN_FLOAT,   180,  1800, RO, 60, (void*)&MinStartStopInv   },
	{0x31D,  uint16,  CPN_FLOAT,     0,  2000, RO,  1, (void*)&CompRatedCurrent  },
	
//	{544,    uint16,      0,   65535, 0, RW, (void*)&SysC, NULL},
};


#if MB_FUNC_READ_INPUT_ENABLED > 0

const sMBSlaveDataTable psSRegInTable [1];

#endif

#if MB_FUNC_WRITE_HOLDING_ENABLED > 0 || MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED > 0 \
    || MB_FUNC_READ_HOLDING_ENABLED > 0 || MB_FUNC_READWRITE_HOLDING_ENABLED > 0

const sMBSlaveDataTable psSRegHoldTable [1] = 
{
	{(sMBSlaveRegData*)Slave_Protocol0_RegHoldBuf, 0, 800, sizeof(Slave_Protocol0_RegHoldBuf)/sizeof(sMBSlaveRegData)}
};

#endif

#if MB_FUNC_READ_COILS_ENABLED > 0 || MB_FUNC_WRITE_COIL_ENABLED > 0 || MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED > 0

const sMBSlaveDataTable psSCoilTable [1] =
{
	{(sMBSlaveBitData*)Slave_Protocol0_CoilBuf, 17, 76, sizeof(Slave_Protocol0_CoilBuf)/sizeof(sMBSlaveBitData)}
};

#endif

#if MB_FUNC_READ_DISCRETE_INPUTS_ENABLED > 0

const sMBSlaveDataTable psSDiscInTable [1];

#endif

#if MB_SLAVE_CPN_ENABLED > 0 

const sMBSlaveDataTable psSCPNTable [1]=
{
	{(sMBSlaveCPNData*)Slave_Protocol0_CPNBuf, 0x200, 0x31D, sizeof(Slave_Protocol0_CPNBuf)/sizeof(sMBSlaveCPNData)}
};

#endif