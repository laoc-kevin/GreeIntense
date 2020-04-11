#ifndef APPDATAH
#define APPDATAH

#include "includes.h"
#include "MasterSlave.h"

typedef struct
{
	int16_t min;
	int16_t max;
}AIRange;

typedef struct
{
	uint8_t SysCOVersion;
	uint8_t SysIDNum;
	uint8_t SysRunState;
	uint8_t SysModeEn;
	uint8_t SysCtrlLogicEn;
	uint8_t SysLoad;
	uint8_t SysType;
	uint8_t SysMode;
	uint8_t SysCtrlLogic;
	uint16_t SysTimeYear;
	uint8_t SysTimeMonth;
	uint8_t SysTimeDay;
	uint8_t SysTimeWeek;
	uint8_t SysTimeHour;
	uint8_t SysTimeMin;
	uint8_t SysTimeSec;
	uint8_t MonTmrOnHour1;
	uint8_t MonTmrOnMin1;
	uint8_t MonTmrOffHour1;
	uint8_t MonTmrOffMin1;
	uint8_t MonTmrOnHour2;
	uint8_t MonTmrOnMin2;
	uint8_t MonTmrOffHour2;
	uint8_t MonTmrOffMin2;
	uint8_t TueTmrOnHour1;
	uint8_t TueTmrOnMin1;
	uint8_t TueTmrOffHour1;
	uint8_t TueTmrOffMin1;
	uint8_t TueTmrOnHour2;
	uint8_t TueTmrOnMin2;
	uint8_t TueTmrOffHour2;
	uint8_t TueTmrOffMin2;
	uint8_t WedTmrOnHour1;
	uint8_t WedTmrOnMin1;
	uint8_t WedTmrOffHour1;
	uint8_t WedTmrOffMin1;
	uint8_t WedTmrOnHour2;
	uint8_t WedTmrOnMin2;
	uint8_t WedTmrOffHour2;
	uint8_t WedTmrOffMin2;
	uint8_t ThursTmrOnHour1;
	uint8_t ThursTmrOnMin1;
	uint8_t ThursTmrOffHour1;
	uint8_t ThursTmrOffMin1;
	uint8_t ThursTmrOnHour2;
	uint8_t ThursTmrOnMin2;
	uint8_t ThursTmrOffHour2;
	uint8_t ThursTmrOffMin2;
	uint8_t FriTmrOnHour1;
	uint8_t FriTmrOnMin1;
	uint8_t FriTmrOffHour1;
	uint8_t FriTmrOffMin1;
	uint8_t FriTmrOnHour2;
	uint8_t FriTmrOnMin2;
	uint8_t FriTmrOffHour2;
	uint8_t FriTmrOffMin2;
	uint8_t SatTmrOnHour1;
	uint8_t SatTmrOnMin1;
	uint8_t SatTmrOffHour1;
	uint8_t SatTmrOffMin1;
	uint8_t SatTmrOnHour2;
	uint8_t SatTmrOnMin2;
	uint8_t SatTmrOffHour2;
	uint8_t SatTmrOffMin2;
	uint8_t SunTmrOnHour1;
	uint8_t SunTmrOnMin1;
	uint8_t SunTmrOffHour1;
	uint8_t SunTmrOffMin1;
	uint8_t SunTmrOnHour2;
	uint8_t SunTmrOnMin2;
	uint8_t SunTmrOffHour2;
	uint8_t SunTmrOffMin2;
	uint8_t TmrEnWeekSel;
	uint8_t TmrEnMonAndTue;
	uint8_t TmrEnWedAndThurs;
	uint8_t TmrEnFriAndSat;
	uint8_t TmrEnSun;
	int16_t SensorCHWST;
	int16_t SensorCHWRT;
	int16_t SensorCHWSP;
	int16_t SensorCHWRP;
	int16_t SensorCWST;
	int16_t SensorCWRT;
	int16_t SensorCHWFL;
	int16_t SensorCWFL;
	int16_t SensorOutT;
	int16_t SensorOutH;
	uint16_t EnergyDayW;
	uint16_t EnergyTotalP;
	uint16_t EnergyDayEERs;
	uint16_t EnergyMonthEERs;
	uint16_t EnergyYearEERs;
	uint16_t EnergyRealEERs;
	uint16_t EnergyTotalEERs;
	uint16_t EnergyCHWF;
	uint32_t EnergyMonthW;
	uint32_t EnergyYearW;
	uint32_t EnergyTotalW;
	uint32_t EnergyRealE;
	uint32_t EnergyDayE;
	uint32_t EnergyMonthE;
	uint32_t EnergyYearE;
	uint32_t EnergyTotalE;
	uint16_t EnergyCWF;
	uint8_t RunCntCHReal;
	uint8_t RunCntCHPlan;
	uint8_t RunCntCHRemain;
	uint8_t RunCntCHWPReal;
	uint8_t RunCntCHWPPlan;
	uint8_t RunCntCHWPRemain;
	uint8_t RunCntCWPReal;
	uint8_t RunCntCWPPlan;
	uint8_t RunCntCWPRemain;
	uint8_t RunCntCTFReal;
	uint8_t RunCntCTFPlan;
	uint8_t RunCntCTFRemain;
	uint8_t RunCntCHWV;
	uint8_t RunCntCWV;
	uint8_t RunCntCTV;
	uint8_t SysFlagss;
	uint16_t CtrlBoard1;
	uint16_t CtrlBoard2;
	uint16_t CtrlBoard3;
	uint16_t CtrlBoard4;
	uint16_t SetCHWPFq;
	uint16_t SetCWPFq;
	uint16_t SetCTFFq;
	uint16_t SetCHWBPVOC;
	uint16_t SetCWBPVOC;
	uint8_t MakerLogic;
	uint8_t MakerCHWOTSP;
	uint16_t MakerCHWDPSP;
	uint8_t MakerBLoadSet;
	uint8_t MakerSLoadSet;
	uint8_t MakerCHWDTSP;
	uint8_t MakerCWDTSP;
	uint8_t MakerCWTDownRate;
	uint8_t MakerCWTUpRate;
	uint8_t MakerCHWTDownRate;
	uint8_t MakerCHWDPOffset;
	uint8_t MakerCHAddLoad;
	uint8_t MakerCHWBPVCtrlDeg;
	uint8_t MakerCHWBPVMax;
	uint8_t MakerCHWBPVMin;
	uint16_t MakerCHWBPVCtrlDly;
	uint8_t MakerCWBPVCtrlDeg;
	uint8_t MakerCWBPVMax;
	uint8_t MakerCWBPVMin;
	uint16_t MakerCWBPVCtrlDly;
	uint8_t MakerCWBPTOffset;
	uint16_t MakerCWRTProLimit;
	uint16_t MakerCHStartTemp;
	uint16_t MakerCHAddTimSlot;
	uint16_t MakerCHSubTimSlot;
	uint8_t MakerCHAdd1Load;
	uint8_t MakerCHAdd2Load;
	uint8_t MakerCHAdd3Load;
	uint8_t MakerCHSub2Load;
	uint8_t MakerCHSub3Load;
	uint8_t MakerCHSub4Load;
	int8_t MakerCHAddTOffset;
	int8_t MakerCHSubTOffset;
	uint16_t MakerSubCHWOffDly;
	uint16_t MakerSubCWOffDly;
	uint8_t MakerCHWPTOffset1;
	uint8_t MakerCHWPTOffset2;
	uint8_t MakerCHWPStartFq;
	uint8_t MakerCHWPMaxFq;
	uint8_t MakerCHWPMinFq;
	uint16_t MakerCHWPFqCtrlDly;
	uint16_t MakerCHWPStopDly;
	uint16_t MakerCHWPStartDly;
	uint8_t MakerCWPTOffset1;
	uint8_t MakerCWPTOffset2;
	uint8_t MakerCWPStartFq;
	uint8_t MakerCWPMaxFq;
	uint8_t MakerCWPMinFq;
	uint16_t MakerCWPFqCtrlDly;
	uint16_t MakerCWPStopDly;
	uint16_t MakerCWPStartDly;
	uint8_t MakerCHWPAddFqDeg1;
	uint8_t MakerCHWPSubFqDeg1;
	uint8_t MakerCWPAddFqDeg1;
	uint8_t MakerCWPSubFqDeg1;
	uint8_t MakerTFStartFq;
	uint8_t MakerTFMaxFq;
	uint8_t MakerTFMinFq;
	uint16_t MakerTFSubFqTemp1;
	uint16_t MakerTFSubFqTemp2;
	uint16_t MakerTFAddFqTemp1;
	uint16_t MakerTFAddFqTemp2;
	uint8_t MakerTFAddFqDeg1;
	uint8_t MakerTFAddFqDeg2;
	uint8_t MakerTFSubFqDeg1;
	uint8_t MakerTFSubFqDeg2;
	uint16_t MakerTFStartTemp;
	uint16_t MakerTFAddDly;
	uint16_t MakerTFSubDly;
	uint16_t MakerTFAddFqDly;
	uint16_t MakerTFSubFqDly;
	uint16_t MakerTFStopDly;
	uint16_t MakerTFStartDly;
	uint16_t MakerTFAddRoTimSlot;
	uint16_t MakerTFSubRoTimSlot;
	uint16_t MakerTFAddRoTemp;
	uint16_t MakerTFSubRoTemp;
	uint16_t MakerVFaultDly;
	uint16_t MakerVCloseDly;
	uint8_t MakerEHeatStopTemp;
	uint8_t MakerEHeatStartTemp;
	uint8_t MakerEHeatTOffset;
	uint16_t MakerEHeatStartDly;
	uint8_t MakerWhCnt;
	uint16_t MakerCHWDn;
	uint16_t MakerCWDn;
	uint8_t MakerHmCnt;
	uint16_t MakerCWSTProLimit;
	uint16_t B1StateCHLogic;
	uint8_t B1StateCHCtrlMode;
	uint8_t B1StateCHCHWTSP;
	int16_t B1StateCHCHWIT;
	int16_t B1StateCHCHWOT;
	int16_t B1StateCHCWIT;
	int16_t B1StateCHCWOT;
	uint32_t B1StateCHRuntime;
	uint16_t B1StateCHLoad;
	uint8_t B1StateCHWP1Logic;
	uint8_t B1StateCHWP2Logic;
	uint8_t B1StateCWP1Logic;
	uint8_t B1StateCWP2Logic;
	uint8_t B1StateTFLogic;
	uint8_t B1StateCHID;
	uint16_t B1StateCHWP1FqSet;
	uint16_t B1StateCHWP2FqSet;
	uint16_t B1StateCWP1FqSet;
	uint16_t B1StateCWP2FqSet;
	uint16_t B1StateTFFqSet;
	uint8_t B1StateCHWV;
	uint8_t B1StateCWV;
	uint8_t B1StateTFV;
	uint8_t B1StateCHWBPVOSP;
	uint8_t B1StateCWBPVOSP;
	uint8_t B1StateElseDevices;
	uint32_t B1StateCHWP1Runtime;
	uint32_t B1StateCHWP2Runtime;
	uint32_t B1StateCWP1Runtime;
	uint32_t B1StateCWP2Runtime;
	uint32_t B1StateTFRuntime;
	uint32_t B1StateEHeatRuntime;
	int16_t B1StateCHWST;
	int16_t B1StateCHWRT;
	int16_t B1StateCHWSP;
	int16_t B1StateCHWRP;
	int16_t B1StateCWST;
	int16_t B1StateCWRT;
	int16_t B1StateCHWRFL;
	int16_t B1StateCWRFL;
	uint16_t B2StateCHLogic;
	uint8_t B2StateCHCtrlMode;
	uint8_t B2StateCHCHWTSP;
	int16_t B2StateCHCHWIT;
	int16_t B2StateCHCHWOT;
	int16_t B2StateCHCWIT;
	int16_t B2StateCHCWOT;
	uint32_t B2StateCHRuntime;
	uint16_t B2StateCHLoad;
	uint8_t B2StateCHWP1Logic;
	uint8_t B2StateCHWP2Logic;
	uint8_t B2StateCWP1Logic;
	uint8_t B2StateCWP2Logic;
	uint8_t B2StateTFLogic;
	uint8_t B2StateCHID;
	uint16_t B2StateCHWP1FqSet;
	uint16_t B2StateCHWP2FqSet;
	uint16_t B2StateCWP1FqSet;
	uint16_t B2StateCWP2FqSet;
	uint16_t B2StateTFFqSet;
	uint8_t B2StateCHWV;
	uint8_t B2StateCWV;
	uint8_t B2StateTFV;
	uint8_t B2StateCHWBPVOSP;
	uint8_t B2StateCWBPVOSP;
	uint8_t B2StateElseDevices;
	uint32_t B2StateCHWP1Runtime;
	uint32_t B2StateCHWP2Runtime;
	uint32_t B2StateCWP1Runtime;
	uint32_t B2StateCWP2Runtime;
	uint32_t B2StateTFRuntime;
	uint32_t B2StateEHeatRuntime;
	int16_t B2StateCHWST;
	int16_t B2StateCHWRT;
	int16_t B2StateCHWSP;
	int16_t B2StateCHWRP;
	int16_t B2StateCWST;
	int16_t B2StateCWRT;
	int16_t B2StateCHWRFL;
	int16_t B2StateCWRFL;
	uint16_t B3StateCHLogic;
	uint8_t B3StateCHCtrlMode;
	uint8_t B3StateCHCHWTSP;
	int16_t B3StateCHCHWIT;
	int16_t B3StateCHCHWOT;
	int16_t B3StateCHCWIT;
	int16_t B3StateCHCWOT;
	uint32_t B3StateCHRuntime;
	uint16_t B3StateCHLoad;
	uint8_t B3StateCHWP1Logic;
	uint8_t B3StateCHWP2Logic;
	uint8_t B3StateCWP1Logic;
	uint8_t B3StateCWP2Logic;
	uint8_t B3StateTFLogic;
	uint8_t B3StateCHID;
	uint16_t B3StateCHWP1FqSet;
	uint16_t B3StateCHWP2FqSet;
	uint16_t B3StateCWP1FqSet;
	uint16_t B3StateCWP2FqSet;
	uint16_t B3StateTFFqSet;
	uint8_t B3StateCHWV;
	uint8_t B3StateCWV;
	uint8_t B3StateTFV;
	uint8_t B3StateCHWBPVOSP;
	uint8_t B3StateCWBPVOSP;
	uint8_t B3StateElseDevices;
	uint32_t B3StateCHWP1Runtime;
	uint32_t B3StateCHWP2Runtime;
	uint32_t B3StateCWP1Runtime;
	uint32_t B3StateCWP2Runtime;
	uint32_t B3StateTFRuntime;
	uint32_t B3StateEHeatRuntime;
	int16_t B3StateCHWST;
	int16_t B3StateCHWRT;
	int16_t B3StateCHWSP;
	int16_t B3StateCHWRP;
	int16_t B3StateCWST;
	int16_t B3StateCWRT;
	int16_t B3StateCHWRFL;
	int16_t B3StateCWRFL;
	uint16_t B4StateCHLogic;
	uint8_t B4StateCHCtrlMode;
	uint8_t B4StateCHCHWTSP;
	int16_t B4StateCHCHWIT;
	int16_t B4StateCHCHWOT;
	int16_t B4StateCHCWIT;
	int16_t B4StateCHCWOT;
	uint32_t B4StateCHRuntime;
	uint16_t B4StateCHLoad;
	uint8_t B4StateCHWP1Logic;
	uint8_t B4StateCHWP2Logic;
	uint8_t B4StateCWP1Logic;
	uint8_t B4StateCWP2Logic;
	uint8_t B4StateTFLogic;
	uint8_t B4StateCHID;
	uint16_t B4StateCHWP1FqSet;
	uint16_t B4StateCHWP2FqSet;
	uint16_t B4StateCWP1FqSet;
	uint16_t B4StateCWP2FqSet;
	uint16_t B4StateTFFqSet;
	uint8_t B4StateCHWV;
	uint8_t B4StateCWV;
	uint8_t B4StateTFV;
	uint8_t B4StateCHWBPVOSP;
	uint8_t B4StateCWBPVOSP;
	uint8_t B4StateElseDevices;
	uint32_t B4StateCHWP1Runtime;
	uint32_t B4StateCHWP2Runtime;
	uint32_t B4StateCWP1Runtime;
	uint32_t B4StateCWP2Runtime;
	uint32_t B4StateTFRuntime;
	uint32_t B4StateEHeatRuntime;
	int16_t B4StateCHWST;
	int16_t B4StateCHWRT;
	int16_t B4StateCHWSP;
	int16_t B4StateCHWRP;
	int16_t B4StateCWST;
	int16_t B4StateCWRT;
	int16_t B4StateCHWRFL;
	int16_t B4StateCWRFL;
	uint16_t BAStateLogic;
	uint16_t BAStateOutT;
	uint16_t BAStateOutH;
	uint16_t BAStateCHWDn;
	uint16_t BAStateCWDn;
	uint16_t BAStateWh1P;
	uint16_t BAStateWh2P;
	uint16_t BAStateWh3P;
	uint16_t BAStateWh4P;
	uint16_t BAStateWh5P;
	uint16_t BAStateWh6P;
	uint16_t BAStateWh7P;
	uint16_t BAStateWh8P;
	uint16_t BAStateWh9P;
	uint16_t BAStateWh10P;
	uint16_t BAStateWh11P;
	uint16_t BAStateWh12P;
	uint16_t BAStateWh13P;
	uint16_t BAStateWh14P;
	uint16_t BAStateWh15P;
	uint16_t BAStateWh16P;
	uint16_t BAStateWh17P;
	uint16_t BAStateWh18P;
	uint16_t BAStateWh19P;
	uint16_t BAStateWh20P;
	uint32_t BAStateWh1W;
	uint32_t BAStateWh2W;
	uint32_t BAStateWh3W;
	uint32_t BAStateWh4W;
	uint32_t BAStateWh5W;
	uint32_t BAStateWh6W;
	uint32_t BAStateWh7W;
	uint32_t BAStateWh8W;
	uint32_t BAStateWh9W;
	uint32_t BAStateWh10W;
	uint32_t BAStateWh11W;
	uint32_t BAStateWh12W;
	uint32_t BAStateWh13W;
	uint32_t BAStateWh14W;
	uint32_t BAStateWh15W;
	uint32_t BAStateWh16W;
	uint32_t BAStateWh17W;
	uint32_t BAStateWh18W;
	uint32_t BAStateWh19W;
	uint32_t BAStateWh20W;
	uint16_t BAStateHm1RealC;
	uint16_t BAStateHm1RealF;
	uint16_t BAStateHm2RealC;
	uint16_t BAStateHm2RealF;
	uint16_t BAStateHm3RealC;
	uint16_t BAStateHm3RealF;
	uint16_t BAStateHm4RealC;
	uint16_t BAStateHm4RealF;
	uint32_t BAStateHm1TotalC;
	uint32_t BAStateHm1TotalF;
	uint32_t BAStateHm2TotalC;
	uint32_t BAStateHm2TotalF;
	uint32_t BAStateHm3TotalC;
	uint32_t BAStateHm3TotalF;
	uint32_t BAStateHm4TotalC;
	uint32_t BAStateHm4TotalF;
	uint8_t B1MakerSysType;
	uint16_t B1MakerCHSize;
	uint8_t B1MakerCHAddr;
	uint8_t B1MakerCHWhAddr;
	uint8_t B1MakerBPVType;
	uint8_t B1MakerCHWP1Type;
	uint8_t B1MakerCHWP1WhAddr;
	uint8_t B1MakerCHWP2Type;
	uint8_t B1MakerCHWP2WhAddr;
	uint8_t B1MakerCWP1Type;
	uint8_t B1MakerCWP1WhAddr;
	uint8_t B1MakerCWP2Type;
	uint8_t B1MakerCWP2WhAddr;
	uint8_t B1MakerCTFType;
	uint8_t B1MakerCTFWhAddr;
	uint16_t B1MakerLogic;
	uint8_t B1MakerAI1Def;
	uint8_t B1MakerAI2Def;
	uint8_t B1MakerAI3Def;
	uint8_t B1MakerAI4Def;
	int16_t B1MakerAI1Max;
	int16_t B1MakerAI1Min;
	int16_t B1MakerAI2Max;
	int16_t B1MakerAI2Min;
	int16_t B1MakerAI3Max;
	int16_t B1MakerAI3Min;
	int16_t B1MakerAI4Max;
	int16_t B1MakerAI4Min;
	uint8_t B2MakerSysType;
	uint16_t B2MakerCHSize;
	uint8_t B2MakerCHAddr;
	uint8_t B2MakerCHWhAddr;
	uint8_t B2MakerBPVType;
	uint8_t B2MakerCHWP1Type;
	uint8_t B2MakerCHWP1WhAddr;
	uint8_t B2MakerCHWP2Type;
	uint8_t B2MakerCHWP2WhAddr;
	uint8_t B2MakerCWP1Type;
	uint8_t B2MakerCWP1WhAddr;
	uint8_t B2MakerCWP2Type;
	uint8_t B2MakerCWP2WhAddr;
	uint8_t B2MakerCTFType;
	uint8_t B2MakerCTFWhAddr;
	uint16_t B2MakerLogic;
	uint8_t B2MakerAI1Def;
	uint8_t B2MakerAI2Def;
	uint8_t B2MakerAI3Def;
	uint8_t B2MakerAI4Def;
	int16_t B2MakerAI1Max;
	int16_t B2MakerAI1Min;
	int16_t B2MakerAI2Max;
	int16_t B2MakerAI2Min;
	int16_t B2MakerAI3Max;
	int16_t B2MakerAI3Min;
	int16_t B2MakerAI4Max;
	int16_t B2MakerAI4Min;
	uint8_t B3MakerSysType;
	uint16_t B3MakerCHSize;
	uint8_t B3MakerCHAddr;
	uint8_t B3MakerCHWhAddr;
	uint8_t B3MakerBPVType;
	uint8_t B3MakerCHWP1Type;
	uint8_t B3MakerCHWP1WhAddr;
	uint8_t B3MakerCHWP2Type;
	uint8_t B3MakerCHWP2WhAddr;
	uint8_t B3MakerCWP1Type;
	uint8_t B3MakerCWP1WhAddr;
	uint8_t B3MakerCWP2Type;
	uint8_t B3MakerCWP2WhAddr;
	uint8_t B3MakerCTFType;
	uint8_t B3MakerCTFWhAddr;
	uint16_t B3MakerLogic;
	uint8_t B3MakerAI1Def;
	uint8_t B3MakerAI2Def;
	uint8_t B3MakerAI3Def;
	uint8_t B3MakerAI4Def;
	int16_t B3MakerAI1Max;
	int16_t B3MakerAI1Min;
	int16_t B3MakerAI2Max;
	int16_t B3MakerAI2Min;
	int16_t B3MakerAI3Max;
	int16_t B3MakerAI3Min;
	int16_t B3MakerAI4Max;
	int16_t B3MakerAI4Min;
	uint8_t B4MakerSysType;
	uint16_t B4MakerCHSize;
	uint8_t B4MakerCHAddr;
	uint8_t B4MakerCHWhAddr;
	uint8_t B4MakerBPVType;
	uint8_t B4MakerCHWP1Type;
	uint8_t B4MakerCHWP1WhAddr;
	uint8_t B4MakerCHWP2Type;
	uint8_t B4MakerCHWP2WhAddr;
	uint8_t B4MakerCWP1Type;
	uint8_t B4MakerCWP1WhAddr;
	uint8_t B4MakerCWP2Type;
	uint8_t B4MakerCWP2WhAddr;
	uint8_t B4MakerCTFType;
	uint8_t B4MakerCTFWhAddr;
	uint16_t B4MakerLogic;
	uint8_t B4MakerAI1Def;
	uint8_t B4MakerAI2Def;
	uint8_t B4MakerAI3Def;
	uint8_t B4MakerAI4Def;
	int16_t B4MakerAI1Max;
	int16_t B4MakerAI1Min;
	int16_t B4MakerAI2Max;
	int16_t B4MakerAI2Min;
	int16_t B4MakerAI3Max;
	int16_t B4MakerAI3Min;
	int16_t B4MakerAI4Max;
	int16_t B4MakerAI4Min;
}ODDataMap;

void syncData(ODDataMap *data)
{
	data->SysCOVersion			=	Sys_COVersion;
	data->SysIDNum				=	Sys_IDNum;
	data->SysRunState			=	Sys_RunState;
	data->SysModeEn				=	Sys_ModeEn;
	data->SysCtrlLogicEn		=	Sys_CtrlLogicEn;
	data->SysLoad				=	Sys_Load;
	data->SysType				=	Sys_Type;
	data->SysMode				=	Sys_Mode;
	data->SysCtrlLogic			=	Sys_CtrlLogic;
	data->SysTimeYear			=	SysTime_Year;
	data->SysTimeMonth			=	SysTime_Month;
	data->SysTimeDay			=	SysTime_Day;
	data->SysTimeWeek			=	SysTime_Week;
	data->SysTimeHour			=	SysTime_Hour;
	data->SysTimeMin			=	SysTime_Min;
	data->SysTimeSec			=	SysTime_Sec;
	data->MonTmrOnHour1			=	MonTmr_OnHour1;
	data->MonTmrOnMin1			=	MonTmr_OnMin1;
	data->MonTmrOffHour1		=	MonTmr_OffHour1;
	data->MonTmrOffMin1			=	MonTmr_OffMin1;
	data->MonTmrOnHour2			=	MonTmr_OnHour2;
	data->MonTmrOnMin2			=	MonTmr_OnMin2;
	data->MonTmrOffHour2		=	MonTmr_OffHour2;
	data->MonTmrOffMin2			=	MonTmr_OffMin2;
	data->TueTmrOnHour1			=	TueTmr_OnHour1;
	data->TueTmrOnMin1			=	TueTmr_OnMin1;
	data->TueTmrOffHour1		=	TueTmr_OffHour1;
	data->TueTmrOffMin1			=	TueTmr_OffMin1;
	data->TueTmrOnHour2			=	TueTmr_OnHour2;
	data->TueTmrOnMin2			=	TueTmr_OnMin2;
	data->TueTmrOffHour2		=	TueTmr_OffHour2;
	data->TueTmrOffMin2			=	TueTmr_OffMin2;
	data->WedTmrOnHour1			=	WedTmr_OnHour1;
	data->WedTmrOnMin1			=	WedTmr_OnMin1;
	data->WedTmrOffHour1		=	WedTmr_OffHour1;
	data->WedTmrOffMin1			=	WedTmr_OffMin1;
	data->WedTmrOnHour2			=	WedTmr_OnHour2;
	data->WedTmrOnMin2			=	WedTmr_OnMin2;
	data->WedTmrOffHour2		=	WedTmr_OffHour2;
	data->WedTmrOffMin2			=	WedTmr_OffMin2;
	data->ThursTmrOnHour1		=	ThursTmr_OnHour1;
	data->ThursTmrOnMin1		=	ThursTmr_OnMin1;
	data->ThursTmrOffHour1		=	ThursTmr_OffHour1;
	data->ThursTmrOffMin1		=	ThursTmr_OffMin1;
	data->ThursTmrOnHour2		=	ThursTmr_OnHour2;
	data->ThursTmrOnMin2		=	ThursTmr_OnMin2;
	data->ThursTmrOffHour2		=	ThursTmr_OffHour2;
	data->ThursTmrOffMin2		=	ThursTmr_OffMin2;
	data->FriTmrOnHour1			=	FriTmr_OnHour1;
	data->FriTmrOnMin1			=	FriTmr_OnMin1;
	data->FriTmrOffHour1		=	FriTmr_OffHour1;
	data->FriTmrOffMin1			=	FriTmr_OffMin1;
	data->FriTmrOnHour2			=	FriTmr_OnHour2;
	data->FriTmrOnMin2			=	FriTmr_OnMin2;
	data->FriTmrOffHour2		=	FriTmr_OffHour2;
	data->FriTmrOffMin2			=	FriTmr_OffMin2;
	data->SatTmrOnHour1			=	SatTmr_OnHour1;
	data->SatTmrOnMin1			=	SatTmr_OnMin1;
	data->SatTmrOffHour1		=	SatTmr_OffHour1;
	data->SatTmrOffMin1			=	SatTmr_OffMin1;
	data->SatTmrOnHour2			=	SatTmr_OnHour2;
	data->SatTmrOnMin2			=	SatTmr_OnMin2;
	data->SatTmrOffHour2		=	SatTmr_OffHour2;
	data->SatTmrOffMin2			=	SatTmr_OffMin2;
	data->SunTmrOnHour1			=	SunTmr_OnHour1;
	data->SunTmrOnMin1			=	SunTmr_OnMin1;
	data->SunTmrOffHour1		=	SunTmr_OffHour1;
	data->SunTmrOffMin1			=	SunTmr_OffMin1;
	data->SunTmrOnHour2			=	SunTmr_OnHour2;
	data->SunTmrOnMin2			=	SunTmr_OnMin2;
	data->SunTmrOffHour2		=	SunTmr_OffHour2;
	data->SunTmrOffMin2			=	SunTmr_OffMin2;
	data->TmrEnWeekSel			=	TmrEn_WeekSel;
	data->TmrEnMonAndTue		=	TmrEn_MonAndTue;
	data->TmrEnWedAndThurs		=	TmrEn_WedAndThurs;
	data->TmrEnFriAndSat		=	TmrEn_FriAndSat;
	data->TmrEnSun				=	TmrEn_Sun;
	data->SensorCHWST			=	Sensor_CHWST;
	data->SensorCHWRT			=	Sensor_CHWRT;
	data->SensorCHWSP			=	Sensor_CHWSP;
	data->SensorCHWRP			=	Sensor_CHWRP;
	data->SensorCWST			=	Sensor_CWST;
	data->SensorCWRT			=	Sensor_CWRT;
	data->SensorCHWFL			=	Sensor_CHWFL;
	data->SensorCWFL			=	Sensor_CWFL;
	data->SensorOutT			=	Sensor_OutT;
	data->SensorOutH			=	Sensor_OutH;
	data->EnergyDayW			=	Energy_DayW;
	data->EnergyTotalP			=	Energy_TotalP;
	data->EnergyDayEERs			=	Energy_DayEERs;
	data->EnergyMonthEERs		=	Energy_MonthEERs;
	data->EnergyYearEERs		=	Energy_YearEERs;
	data->EnergyRealEERs		=	Energy_RealEERs;
	data->EnergyTotalEERs		=	Energy_TotalEERs;
	data->EnergyCHWF			=	Energy_CHWF;
	data->EnergyMonthW			=	Energy_MonthW;
	data->EnergyYearW			=	Energy_YearW;
	data->EnergyTotalW			=	Energy_TotalW;
	data->EnergyRealE			=	Energy_RealE;
	data->EnergyDayE			=	Energy_DayE;
	data->EnergyMonthE			=	Energy_MonthE;
	data->EnergyYearE			=	Energy_YearE;
	data->EnergyTotalE			=	Energy_TotalE;
	data->EnergyCWF				=	Energy_CWF;
	data->RunCntCHReal			=	RunCnt_CHReal;
	data->RunCntCHPlan			=	RunCnt_CHPlan;
	data->RunCntCHRemain		=	RunCnt_CHRemain;
	data->RunCntCHWPReal		=	RunCnt_CHWPReal;
	data->RunCntCHWPPlan		=	RunCnt_CHWPPlan;
	data->RunCntCHWPRemain		=	RunCnt_CHWPRemain;
	data->RunCntCWPReal			=	RunCnt_CWPReal;
	data->RunCntCWPPlan			=	RunCnt_CWPPlan;
	data->RunCntCWPRemain		=	RunCnt_CWPRemain;
	data->RunCntCTFReal			=	RunCnt_CTFReal;
	data->RunCntCTFPlan			=	RunCnt_CTFPlan;
	data->RunCntCTFRemain		=	RunCnt_CTFRemain;
	data->RunCntCHWV			=	RunCnt_CHWV;
	data->RunCntCWV				=	RunCnt_CWV;
	data->RunCntCTV				=	RunCnt_CTV;
	data->SysFlagss				=	SysFlags_s;
	data->CtrlBoard1			=	Ctrl_Board1;
	data->CtrlBoard2			=	Ctrl_Board2;
	data->CtrlBoard3			=	Ctrl_Board3;
	data->CtrlBoard4			=	Ctrl_Board4;
	data->SetCHWPFq				=	Set_CHWPFq;
	data->SetCWPFq				=	Set_CWPFq;
	data->SetCTFFq				=	Set_CTFFq;
	data->SetCHWBPVOC			=	Set_CHWBPVOC;
	data->SetCWBPVOC			=	Set_CWBPVOC;
	data->MakerLogic			=	Maker_Logic;
	data->MakerCHWOTSP			=	Maker_CHWOTSP;
	data->MakerCHWDPSP			=	Maker_CHWDPSP;
	data->MakerBLoadSet			=	Maker_BLoadSet;
	data->MakerSLoadSet			=	Maker_SLoadSet;
	data->MakerCHWDTSP			=	Maker_CHWDTSP;
	data->MakerCWDTSP			=	Maker_CWDTSP;
	data->MakerCWTDownRate		=	Maker_CWTDownRate;
	data->MakerCWTUpRate		=	Maker_CWTUpRate;
	data->MakerCHWTDownRate		=	Maker_CHWTDownRate;
	data->MakerCHWDPOffset		=	Maker_CHWDPOffset;
	data->MakerCHAddLoad		=	Maker_CHAddLoad;
	data->MakerCHWBPVCtrlDeg	=	Maker_CHWBPVCtrlDeg;
	data->MakerCHWBPVMax		=	Maker_CHWBPVMax;
	data->MakerCHWBPVMin		=	Maker_CHWBPVMin;
	data->MakerCHWBPVCtrlDly	=	Maker_CHWBPVCtrlDly;
	data->MakerCWBPVCtrlDeg		=	Maker_CWBPVCtrlDeg;
	data->MakerCWBPVMax			=	Maker_CWBPVMax;
	data->MakerCWBPVMin			=	Maker_CWBPVMin;
	data->MakerCWBPVCtrlDly		=	Maker_CWBPVCtrlDly;
	data->MakerCWBPTOffset		=	Maker_CWBPTOffset;
	data->MakerCWRTProLimit		=	Maker_CWRTProLimit;
	data->MakerCHStartTemp		=	Maker_CHStartTemp;
	data->MakerCHAddTimSlot		=	Maker_CHAddTimSlot;
	data->MakerCHSubTimSlot		=	Maker_CHSubTimSlot;
	data->MakerCHAdd1Load		=	Maker_CHAdd1Load;
	data->MakerCHAdd2Load		=	Maker_CHAdd2Load;
	data->MakerCHAdd3Load		=	Maker_CHAdd3Load;
	data->MakerCHSub2Load		=	Maker_CHSub2Load;
	data->MakerCHSub3Load		=	Maker_CHSub3Load;
	data->MakerCHSub4Load		=	Maker_CHSub4Load;
	data->MakerCHAddTOffset		=	Maker_CHAddTOffset;
	data->MakerCHSubTOffset		=	Maker_CHSubTOffset;
	data->MakerSubCHWOffDly		=	Maker_SubCHWOffDly;
	data->MakerSubCWOffDly		=	Maker_SubCWOffDly;
	data->MakerCHWPTOffset1		=	Maker_CHWPTOffset1;
	data->MakerCHWPTOffset2		=	Maker_CHWPTOffset2;
	data->MakerCHWPStartFq		=	Maker_CHWPStartFq;
	data->MakerCHWPMaxFq		=	Maker_CHWPMaxFq;
	data->MakerCHWPMinFq		=	Maker_CHWPMinFq;
	data->MakerCHWPFqCtrlDly	=	Maker_CHWPFqCtrlDly;
	data->MakerCHWPStopDly		=	Maker_CHWPStopDly;
	data->MakerCHWPStartDly		=	Maker_CHWPStartDly;
	data->MakerCWPTOffset1		=	Maker_CWPTOffset1;
	data->MakerCWPTOffset2		=	Maker_CWPTOffset2;
	data->MakerCWPStartFq		=	Maker_CWPStartFq;
	data->MakerCWPMaxFq			=	Maker_CWPMaxFq;
	data->MakerCWPMinFq			=	Maker_CWPMinFq;
	data->MakerCWPFqCtrlDly		=	Maker_CWPFqCtrlDly;
	data->MakerCWPStopDly		=	Maker_CWPStopDly;
	data->MakerCWPStartDly		=	Maker_CWPStartDly;
	data->MakerCHWPAddFqDeg1	=	Maker_CHWPAddFqDeg1;
	data->MakerCHWPSubFqDeg1	=	Maker_CHWPSubFqDeg1;
	data->MakerCWPAddFqDeg1		=	Maker_CWPAddFqDeg1;
	data->MakerCWPSubFqDeg1		=	Maker_CWPSubFqDeg1;
	data->MakerTFStartFq		=	Maker_TFStartFq;
	data->MakerTFMaxFq			=	Maker_TFMaxFq;
	data->MakerTFMinFq			=	Maker_TFMinFq;
	data->MakerTFSubFqTemp1		=	Maker_TFSubFqTemp1;
	data->MakerTFSubFqTemp2		=	Maker_TFSubFqTemp2;
	data->MakerTFAddFqTemp1		=	Maker_TFAddFqTemp1;
	data->MakerTFAddFqTemp2		=	Maker_TFAddFqTemp2;
	data->MakerTFAddFqDeg1		=	Maker_TFAddFqDeg1;
	data->MakerTFAddFqDeg2		=	Maker_TFAddFqDeg2;
	data->MakerTFSubFqDeg1		=	Maker_TFSubFqDeg1;
	data->MakerTFSubFqDeg2		=	Maker_TFSubFqDeg2;
	data->MakerTFStartTemp		=	Maker_TFStartTemp;
	data->MakerTFAddDly			=	Maker_TFAddDly;
	data->MakerTFSubDly			=	Maker_TFSubDly;
	data->MakerTFAddFqDly		=	Maker_TFAddFqDly;
	data->MakerTFSubFqDly		=	Maker_TFSubFqDly;
	data->MakerTFStopDly		=	Maker_TFStopDly;
	data->MakerTFStartDly		=	Maker_TFStartDly;
	data->MakerTFAddRoTimSlot	=	Maker_TFAddRoTimSlot;
	data->MakerTFSubRoTimSlot	=	Maker_TFSubRoTimSlot;
	data->MakerTFAddRoTemp		=	Maker_TFAddRoTemp;
	data->MakerTFSubRoTemp		=	Maker_TFSubRoTemp;
	data->MakerVFaultDly		=	Maker_VFaultDly;
	data->MakerVCloseDly		=	Maker_VCloseDly;
	data->MakerEHeatStopTemp	=	Maker_EHeatStopTemp;
	data->MakerEHeatStartTemp	=	Maker_EHeatStartTemp;
	data->MakerEHeatTOffset		=	Maker_EHeatTOffset;
	data->MakerEHeatStartDly	=	Maker_EHeatStartDly;
	data->MakerWhCnt			=	Maker_WhCnt;
	data->MakerCHWDn			=	Maker_CHWDn;
	data->MakerCWDn				=	Maker_CWDn;
	data->MakerHmCnt			=	Maker_HmCnt;
	data->MakerCWSTProLimit		=	Maker_CWSTProLimit;
	data->B1StateCHLogic		=	B1State_CHLogic;
	data->B1StateCHCtrlMode		=	B1State_CHCtrlMode;
	data->B1StateCHCHWTSP		=	B1State_CHCHWTSP;
	data->B1StateCHCHWIT		=	B1State_CHCHWIT;
	data->B1StateCHCHWOT		=	B1State_CHCHWOT;
	data->B1StateCHCWIT			=	B1State_CHCWIT;
	data->B1StateCHCWOT			=	B1State_CHCWOT;
	data->B1StateCHRuntime		=	B1State_CHRuntime;
	data->B1StateCHLoad			=	B1State_CHLoad;
	data->B1StateCHWP1Logic		=	B1State_CHWP1Logic;
	data->B1StateCHWP2Logic		=	B1State_CHWP2Logic;
	data->B1StateCWP1Logic		=	B1State_CWP1Logic;
	data->B1StateCWP2Logic		=	B1State_CWP2Logic;
	data->B1StateTFLogic		=	B1State_TFLogic;
	data->B1StateCHID			=	B1State_CHID;
	data->B1StateCHWP1FqSet		=	B1State_CHWP1FqSet;
	data->B1StateCHWP2FqSet		=	B1State_CHWP2FqSet;
	data->B1StateCWP1FqSet		=	B1State_CWP1FqSet;
	data->B1StateCWP2FqSet		=	B1State_CWP2FqSet;
	data->B1StateTFFqSet		=	B1State_TFFqSet;
	data->B1StateCHWV			=	B1State_CHWV;
	data->B1StateCWV			=	B1State_CWV;
	data->B1StateTFV			=	B1State_TFV;
	data->B1StateCHWBPVOSP		=	B1State_CHWBPVOSP;
	data->B1StateCWBPVOSP		=	B1State_CWBPVOSP;
	data->B1StateElseDevices	=	B1State_ElseDevices;
	data->B1StateCHWP1Runtime	=	B1State_CHWP1Runtime;
	data->B1StateCHWP2Runtime	=	B1State_CHWP2Runtime;
	data->B1StateCWP1Runtime	=	B1State_CWP1Runtime;
	data->B1StateCWP2Runtime	=	B1State_CWP2Runtime;
	data->B1StateTFRuntime		=	B1State_TFRuntime;
	data->B1StateEHeatRuntime	=	B1State_EHeatRuntime;
	data->B1StateCHWST			=	B1State_CHWST;
	data->B1StateCHWRT			=	B1State_CHWRT;
	data->B1StateCHWSP			=	B1State_CHWSP;
	data->B1StateCHWRP			=	B1State_CHWRP;
	data->B1StateCWST			=	B1State_CWST;
	data->B1StateCWRT			=	B1State_CWRT;
	data->B1StateCHWRFL			=	B1State_CHWRFL;
	data->B1StateCWRFL			=	B1State_CWRFL;
	data->B2StateCHLogic		=	B2State_CHLogic;
	data->B2StateCHCtrlMode		=	B2State_CHCtrlMode;
	data->B2StateCHCHWTSP		=	B2State_CHCHWTSP;
	data->B2StateCHCHWIT		=	B2State_CHCHWIT;
	data->B2StateCHCHWOT		=	B2State_CHCHWOT;
	data->B2StateCHCWIT			=	B2State_CHCWIT;
	data->B2StateCHCWOT			=	B2State_CHCWOT;
	data->B2StateCHRuntime		=	B2State_CHRuntime;
	data->B2StateCHLoad			=	B2State_CHLoad;
	data->B2StateCHWP1Logic		=	B2State_CHWP1Logic;
	data->B2StateCHWP2Logic		=	B2State_CHWP2Logic;
	data->B2StateCWP1Logic		=	B2State_CWP1Logic;
	data->B2StateCWP2Logic		=	B2State_CWP2Logic;
	data->B2StateTFLogic		=	B2State_TFLogic;
	data->B2StateCHID			=	B2State_CHID;
	data->B2StateCHWP1FqSet		=	B2State_CHWP1FqSet;
	data->B2StateCHWP2FqSet		=	B2State_CHWP2FqSet;
	data->B2StateCWP1FqSet		=	B2State_CWP1FqSet;
	data->B2StateCWP2FqSet		=	B2State_CWP2FqSet;
	data->B2StateTFFqSet		=	B2State_TFFqSet;
	data->B2StateCHWV			=	B2State_CHWV;
	data->B2StateCWV			=	B2State_CWV;
	data->B2StateTFV			=	B2State_TFV;
	data->B2StateCHWBPVOSP		=	B2State_CHWBPVOSP;
	data->B2StateCWBPVOSP		=	B2State_CWBPVOSP;
	data->B2StateElseDevices	=	B2State_ElseDevices;
	data->B2StateCHWP1Runtime	=	B2State_CHWP1Runtime;
	data->B2StateCHWP2Runtime	=	B2State_CHWP2Runtime;
	data->B2StateCWP1Runtime	=	B2State_CWP1Runtime;
	data->B2StateCWP2Runtime	=	B2State_CWP2Runtime;
	data->B2StateTFRuntime		=	B2State_TFRuntime;
	data->B2StateEHeatRuntime	=	B2State_EHeatRuntime;
	data->B2StateCHWST			=	B2State_CHWST;
	data->B2StateCHWRT			=	B2State_CHWRT;
	data->B2StateCHWSP			=	B2State_CHWSP;
	data->B2StateCHWRP			=	B2State_CHWRP;
	data->B2StateCWST			=	B2State_CWST;
	data->B2StateCWRT			=	B2State_CWRT;
	data->B2StateCHWRFL			=	B2State_CHWRFL;
	data->B2StateCWRFL			=	B2State_CWRFL;
	data->B3StateCHLogic		=	B3State_CHLogic;
	data->B3StateCHCtrlMode		=	B3State_CHCtrlMode;
	data->B3StateCHCHWTSP		=	B3State_CHCHWTSP;
	data->B3StateCHCHWIT		=	B3State_CHCHWIT;
	data->B3StateCHCHWOT		=	B3State_CHCHWOT;
	data->B3StateCHCWIT			=	B3State_CHCWIT;
	data->B3StateCHCWOT			=	B3State_CHCWOT;
	data->B3StateCHRuntime		=	B3State_CHRuntime;
	data->B3StateCHLoad			=	B3State_CHLoad;
	data->B3StateCHWP1Logic		=	B3State_CHWP1Logic;
	data->B3StateCHWP2Logic		=	B3State_CHWP2Logic;
	data->B3StateCWP1Logic		=	B3State_CWP1Logic;
	data->B3StateCWP2Logic		=	B3State_CWP2Logic;
	data->B3StateTFLogic		=	B3State_TFLogic;
	data->B3StateCHID			=	B3State_CHID;
	data->B3StateCHWP1FqSet		=	B3State_CHWP1FqSet;
	data->B3StateCHWP2FqSet		=	B3State_CHWP2FqSet;
	data->B3StateCWP1FqSet		=	B3State_CWP1FqSet;
	data->B3StateCWP2FqSet		=	B3State_CWP2FqSet;
	data->B3StateTFFqSet		=	B3State_TFFqSet;
	data->B3StateCHWV			=	B3State_CHWV;
	data->B3StateCWV			=	B3State_CWV;
	data->B3StateTFV			=	B3State_TFV;
	data->B3StateCHWBPVOSP		=	B3State_CHWBPVOSP;
	data->B3StateCWBPVOSP		=	B3State_CWBPVOSP;
	data->B3StateElseDevices	=	B3State_ElseDevices;
	data->B3StateCHWP1Runtime	=	B3State_CHWP1Runtime;
	data->B3StateCHWP2Runtime	=	B3State_CHWP2Runtime;
	data->B3StateCWP1Runtime	=	B3State_CWP1Runtime;
	data->B3StateCWP2Runtime	=	B3State_CWP2Runtime;
	data->B3StateTFRuntime		=	B3State_TFRuntime;
	data->B3StateEHeatRuntime	=	B3State_EHeatRuntime;
	data->B3StateCHWST			=	B3State_CHWST;
	data->B3StateCHWRT			=	B3State_CHWRT;
	data->B3StateCHWSP			=	B3State_CHWSP;
	data->B3StateCHWRP			=	B3State_CHWRP;
	data->B3StateCWST			=	B3State_CWST;
	data->B3StateCWRT			=	B3State_CWRT;
	data->B3StateCHWRFL			=	B3State_CHWRFL;
	data->B3StateCWRFL			=	B3State_CWRFL;
	data->B4StateCHLogic		=	B4State_CHLogic;
	data->B4StateCHCtrlMode		=	B4State_CHCtrlMode;
	data->B4StateCHCHWTSP		=	B4State_CHCHWTSP;
	data->B4StateCHCHWIT		=	B4State_CHCHWIT;
	data->B4StateCHCHWOT		=	B4State_CHCHWOT;
	data->B4StateCHCWIT			=	B4State_CHCWIT;
	data->B4StateCHCWOT			=	B4State_CHCWOT;
	data->B4StateCHRuntime		=	B4State_CHRuntime;
	data->B4StateCHLoad			=	B4State_CHLoad;
	data->B4StateCHWP1Logic		=	B4State_CHWP1Logic;
	data->B4StateCHWP2Logic		=	B4State_CHWP2Logic;
	data->B4StateCWP1Logic		=	B4State_CWP1Logic;
	data->B4StateCWP2Logic		=	B4State_CWP2Logic;
	data->B4StateTFLogic		=	B4State_TFLogic;
	data->B4StateCHID			=	B4State_CHID;
	data->B4StateCHWP1FqSet		=	B4State_CHWP1FqSet;
	data->B4StateCHWP2FqSet		=	B4State_CHWP2FqSet;
	data->B4StateCWP1FqSet		=	B4State_CWP1FqSet;
	data->B4StateCWP2FqSet		=	B4State_CWP2FqSet;
	data->B4StateTFFqSet		=	B4State_TFFqSet;
	data->B4StateCHWV			=	B4State_CHWV;
	data->B4StateCWV			=	B4State_CWV;
	data->B4StateTFV			=	B4State_TFV;
	data->B4StateCHWBPVOSP		=	B4State_CHWBPVOSP;
	data->B4StateCWBPVOSP		=	B4State_CWBPVOSP;
	data->B4StateElseDevices	=	B4State_ElseDevices;
	data->B4StateCHWP1Runtime	=	B4State_CHWP1Runtime;
	data->B4StateCHWP2Runtime	=	B4State_CHWP2Runtime;
	data->B4StateCWP1Runtime	=	B4State_CWP1Runtime;
	data->B4StateCWP2Runtime	=	B4State_CWP2Runtime;
	data->B4StateTFRuntime		=	B4State_TFRuntime;
	data->B4StateEHeatRuntime	=	B4State_EHeatRuntime;
	data->B4StateCHWST			=	B4State_CHWST;
	data->B4StateCHWRT			=	B4State_CHWRT;
	data->B4StateCHWSP			=	B4State_CHWSP;
	data->B4StateCHWRP			=	B4State_CHWRP;
	data->B4StateCWST			=	B4State_CWST;
	data->B4StateCWRT			=	B4State_CWRT;
	data->B4StateCHWRFL			=	B4State_CHWRFL;
	data->B4StateCWRFL			=	B4State_CWRFL;
	data->BAStateLogic			=	BAState_Logic;
	data->BAStateOutT			=	BAState_OutT;
	data->BAStateOutH			=	BAState_OutH;
	data->BAStateCHWDn			=	BAState_CHWDn;
	data->BAStateCWDn			=	BAState_CWDn;
	data->BAStateWh1P			=	BAState_Wh1P;
	data->BAStateWh2P			=	BAState_Wh2P;
	data->BAStateWh3P			=	BAState_Wh3P;
	data->BAStateWh4P			=	BAState_Wh4P;
	data->BAStateWh5P			=	BAState_Wh5P;
	data->BAStateWh6P			=	BAState_Wh6P;
	data->BAStateWh7P			=	BAState_Wh7P;
	data->BAStateWh8P			=	BAState_Wh8P;
	data->BAStateWh9P			=	BAState_Wh9P;
	data->BAStateWh10P			=	BAState_Wh10P;
	data->BAStateWh11P			=	BAState_Wh11P;
	data->BAStateWh12P			=	BAState_Wh12P;
	data->BAStateWh13P			=	BAState_Wh13P;
	data->BAStateWh14P			=	BAState_Wh14P;
	data->BAStateWh15P			=	BAState_Wh15P;
	data->BAStateWh16P			=	BAState_Wh16P;
	data->BAStateWh17P			=	BAState_Wh17P;
	data->BAStateWh18P			=	BAState_Wh18P;
	data->BAStateWh19P			=	BAState_Wh19P;
	data->BAStateWh20P			=	BAState_Wh20P;
	data->BAStateWh1W			=	BAState_Wh1W;
	data->BAStateWh2W			=	BAState_Wh2W;
	data->BAStateWh3W			=	BAState_Wh3W;
	data->BAStateWh4W			=	BAState_Wh4W;
	data->BAStateWh5W			=	BAState_Wh5W;
	data->BAStateWh6W			=	BAState_Wh6W;
	data->BAStateWh7W			=	BAState_Wh7W;
	data->BAStateWh8W			=	BAState_Wh8W;
	data->BAStateWh9W			=	BAState_Wh9W;
	data->BAStateWh10W			=	BAState_Wh10W;
	data->BAStateWh11W			=	BAState_Wh11W;
	data->BAStateWh12W			=	BAState_Wh12W;
	data->BAStateWh13W			=	BAState_Wh13W;
	data->BAStateWh14W			=	BAState_Wh14W;
	data->BAStateWh15W			=	BAState_Wh15W;
	data->BAStateWh16W			=	BAState_Wh16W;
	data->BAStateWh17W			=	BAState_Wh17W;
	data->BAStateWh18W			=	BAState_Wh18W;
	data->BAStateWh19W			=	BAState_Wh19W;
	data->BAStateWh20W			=	BAState_Wh20W;
	data->BAStateHm1RealC		=	BAState_Hm1RealC;
	data->BAStateHm1RealF		=	BAState_Hm1RealF;
	data->BAStateHm2RealC		=	BAState_Hm2RealC;
	data->BAStateHm2RealF		=	BAState_Hm2RealF;
	data->BAStateHm3RealC		=	BAState_Hm3RealC;
	data->BAStateHm3RealF		=	BAState_Hm3RealF;
	data->BAStateHm4RealC		=	BAState_Hm4RealC;
	data->BAStateHm4RealF		=	BAState_Hm4RealF;
	data->BAStateHm1TotalC		=	BAState_Hm1TotalC;
	data->BAStateHm1TotalF		=	BAState_Hm1TotalF;
	data->BAStateHm2TotalC		=	BAState_Hm2TotalC;
	data->BAStateHm2TotalF		=	BAState_Hm2TotalF;
	data->BAStateHm3TotalC		=	BAState_Hm3TotalC;
	data->BAStateHm3TotalF		=	BAState_Hm3TotalF;
	data->BAStateHm4TotalC		=	BAState_Hm4TotalC;
	data->BAStateHm4TotalF		=	BAState_Hm4TotalF;
	data->B1MakerSysType		=	B1Maker_SysType;
	data->B1MakerCHSize			=	B1Maker_CHSize;
	data->B1MakerCHAddr			=	B1Maker_CHAddr;
	data->B1MakerCHWhAddr		=	B1Maker_CHWhAddr;
	data->B1MakerBPVType		=	B1Maker_BPVType;
	data->B1MakerCHWP1Type		=	B1Maker_CHWP1Type;
	data->B1MakerCHWP1WhAddr	=	B1Maker_CHWP1WhAddr;
	data->B1MakerCHWP2Type		=	B1Maker_CHWP2Type;
	data->B1MakerCHWP2WhAddr	=	B1Maker_CHWP2WhAddr;
	data->B1MakerCWP1Type		=	B1Maker_CWP1Type;
	data->B1MakerCWP1WhAddr		=	B1Maker_CWP1WhAddr;
	data->B1MakerCWP2Type		=	B1Maker_CWP2Type;
	data->B1MakerCWP2WhAddr		=	B1Maker_CWP2WhAddr;
	data->B1MakerCTFType		=	B1Maker_CTFType;
	data->B1MakerCTFWhAddr		=	B1Maker_CTFWhAddr;
	data->B1MakerLogic			=	B1Maker_Logic;
	data->B1MakerAI1Def			=	B1Maker_AI1Def;
	data->B1MakerAI2Def			=	B1Maker_AI2Def;
	data->B1MakerAI3Def			=	B1Maker_AI3Def;
	data->B1MakerAI4Def			=	B1Maker_AI4Def;
	data->B1MakerAI1Max			=	B1Maker_AI1Max;
	data->B1MakerAI1Min			=	B1Maker_AI1Min;
	data->B1MakerAI2Max			=	B1Maker_AI2Max;
	data->B1MakerAI2Min			=	B1Maker_AI2Min;
	data->B1MakerAI3Max			=	B1Maker_AI3Max;
	data->B1MakerAI3Min			=	B1Maker_AI3Min;
	data->B1MakerAI4Max			=	B1Maker_AI4Max;
	data->B1MakerAI4Min			=	B1Maker_AI4Min;
	data->B2MakerSysType		=	B2Maker_SysType;
	data->B2MakerCHSize			=	B2Maker_CHSize;
	data->B2MakerCHAddr			=	B2Maker_CHAddr;
	data->B2MakerCHWhAddr		=	B2Maker_CHWhAddr;
	data->B2MakerBPVType		=	B2Maker_BPVType;
	data->B2MakerCHWP1Type		=	B2Maker_CHWP1Type;
	data->B2MakerCHWP1WhAddr	=	B2Maker_CHWP1WhAddr;
	data->B2MakerCHWP2Type		=	B2Maker_CHWP2Type;
	data->B2MakerCHWP2WhAddr	=	B2Maker_CHWP2WhAddr;
	data->B2MakerCWP1Type		=	B2Maker_CWP1Type;
	data->B2MakerCWP1WhAddr		=	B2Maker_CWP1WhAddr;
	data->B2MakerCWP2Type		=	B2Maker_CWP2Type;
	data->B2MakerCWP2WhAddr		=	B2Maker_CWP2WhAddr;
	data->B2MakerCTFType		=	B2Maker_CTFType;
	data->B2MakerCTFWhAddr		=	B2Maker_CTFWhAddr;
	data->B2MakerLogic			=	B2Maker_Logic;
	data->B2MakerAI1Def			=	B2Maker_AI1Def;
	data->B2MakerAI2Def			=	B2Maker_AI2Def;
	data->B2MakerAI3Def			=	B2Maker_AI3Def;
	data->B2MakerAI4Def			=	B2Maker_AI4Def;
	data->B2MakerAI1Max			=	B2Maker_AI1Max;
	data->B2MakerAI1Min			=	B2Maker_AI1Min;
	data->B2MakerAI2Max			=	B2Maker_AI2Max;
	data->B2MakerAI2Min			=	B2Maker_AI2Min;
	data->B2MakerAI3Max			=	B2Maker_AI3Max;
	data->B2MakerAI3Min			=	B2Maker_AI3Min;
	data->B2MakerAI4Max			=	B2Maker_AI4Max;
	data->B2MakerAI4Min			=	B2Maker_AI4Min;
	data->B3MakerSysType		=	B3Maker_SysType;
	data->B3MakerCHSize			=	B3Maker_CHSize;
	data->B3MakerCHAddr			=	B3Maker_CHAddr;
	data->B3MakerCHWhAddr		=	B3Maker_CHWhAddr;
	data->B3MakerBPVType		=	B3Maker_BPVType;
	data->B3MakerCHWP1Type		=	B3Maker_CHWP1Type;
	data->B3MakerCHWP1WhAddr	=	B3Maker_CHWP1WhAddr;
	data->B3MakerCHWP2Type		=	B3Maker_CHWP2Type;
	data->B3MakerCHWP2WhAddr	=	B3Maker_CHWP2WhAddr;
	data->B3MakerCWP1Type		=	B3Maker_CWP1Type;
	data->B3MakerCWP1WhAddr		=	B3Maker_CWP1WhAddr;
	data->B3MakerCWP2Type		=	B3Maker_CWP2Type;
	data->B3MakerCWP2WhAddr		=	B3Maker_CWP2WhAddr;
	data->B3MakerCTFType		=	B3Maker_CTFType;
	data->B3MakerCTFWhAddr		=	B3Maker_CTFWhAddr;
	data->B3MakerLogic			=	B3Maker_Logic;
	data->B3MakerAI1Def			=	B3Maker_AI1Def;
	data->B3MakerAI2Def			=	B3Maker_AI2Def;
	data->B3MakerAI3Def			=	B3Maker_AI3Def;
	data->B3MakerAI4Def			=	B3Maker_AI4Def;
	data->B3MakerAI1Max			=	B3Maker_AI1Max;
	data->B3MakerAI1Min			=	B3Maker_AI1Min;
	data->B3MakerAI2Max			=	B3Maker_AI2Max;
	data->B3MakerAI2Min			=	B3Maker_AI2Min;
	data->B3MakerAI3Max			=	B3Maker_AI3Max;
	data->B3MakerAI3Min			=	B3Maker_AI3Min;
	data->B3MakerAI4Max			=	B3Maker_AI4Max;
	data->B3MakerAI4Min			=	B3Maker_AI4Min;
	data->B4MakerSysType		=	B4Maker_SysType;
	data->B4MakerCHSize			=	B4Maker_CHSize;
	data->B4MakerCHAddr			=	B4Maker_CHAddr;
	data->B4MakerCHWhAddr		=	B4Maker_CHWhAddr;
	data->B4MakerBPVType		=	B4Maker_BPVType;
	data->B4MakerCHWP1Type		=	B4Maker_CHWP1Type;
	data->B4MakerCHWP1WhAddr	=	B4Maker_CHWP1WhAddr;
	data->B4MakerCHWP2Type		=	B4Maker_CHWP2Type;
	data->B4MakerCHWP2WhAddr	=	B4Maker_CHWP2WhAddr;
	data->B4MakerCWP1Type		=	B4Maker_CWP1Type;
	data->B4MakerCWP1WhAddr		=	B4Maker_CWP1WhAddr;
	data->B4MakerCWP2Type		=	B4Maker_CWP2Type;
	data->B4MakerCWP2WhAddr		=	B4Maker_CWP2WhAddr;
	data->B4MakerCTFType		=	B4Maker_CTFType;
	data->B4MakerCTFWhAddr		=	B4Maker_CTFWhAddr;
	data->B4MakerLogic			=	B4Maker_Logic;
	data->B4MakerAI1Def			=	B4Maker_AI1Def;
	data->B4MakerAI2Def			=	B4Maker_AI2Def;
	data->B4MakerAI3Def			=	B4Maker_AI3Def;
	data->B4MakerAI4Def			=	B4Maker_AI4Def;
	data->B4MakerAI1Max			=	B4Maker_AI1Max;
	data->B4MakerAI1Min			=	B4Maker_AI1Min;
	data->B4MakerAI2Max			=	B4Maker_AI2Max;
	data->B4MakerAI2Min			=	B4Maker_AI2Min;
	data->B4MakerAI3Max			=	B4Maker_AI3Max;
	data->B4MakerAI3Min			=	B4Maker_AI3Min;
	data->B4MakerAI4Max			=	B4Maker_AI4Max;
	data->B4MakerAI4Min			=	B4Maker_AI4Min;
}

#endif

