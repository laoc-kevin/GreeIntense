#include "stdint.h"
#include "app_val.h"
#include "mbdict_m.h"
/**************************************************************************/
/* Declaration of mapped variables                                        */
/**************************************************************************/
uint8_t CHProtocolType=2;		

uint16_t CHID = 0x13;		        //机型ID
uint16_t CHType = 11;				//机型
uint16_t CHProtocolVer = 29;	    //机型协议版本
uint16_t CHCommRunCtrl = 0x55;		//机组启停控制(通讯控制)
uint8_t  CHContRunCtrl = 0;		    //机组启停控制(触点控制)
uint8_t  CHControl = 0;		        //机组启停
uint16_t CHRemoteEn = 0;		    //机组远程开关功能
uint16_t CHRunCtrl = 0x55;		    //机组启停控制

uint16_t CHMode;				    //机组控制模式
uint16_t CHStartTimes = 0;		    //机组累计启动次数
uint16_t CHState = 0;				//机组状态
uint16_t CHRunMode = 1;				//机组运行模式

uint16_t CHCHWIT = 0;				//机组冷冻进水温度
uint16_t CHCHWOT = 0;				//机组冷冻出水温度
uint16_t CHCWIT = 0;				//机组冷却进水温度
uint16_t CHCWOT = 0;				//机组冷却出水温度

uint16_t CHCHWIP_B = 0;				//机组冷冻进口压力(过滤后)
uint16_t CHCHWOP = 0;				//机组冷冻出口压力
uint16_t CHCWIP_B  = 0;				//机组冷却进口压力(过滤后)
uint16_t CHCWOP  = 0;				//机组冷却出口压力
uint16_t CHCHWIP_F = 0;				//机组冷冻进口压力(过滤前)
uint16_t CHCWIP_F = 0;				//机组冷却进口压力(过滤前)

int16_t  CHEvapT = 0;               //机组蒸发温度
int16_t  CHCondT = 0;               //机组冷凝温度
int16_t  CHEvapP = 0;               //机组蒸发压力
int16_t  CHCondP = 0;               //机组冷凝压力

uint16_t CompDisT;                  //压缩机排气温度
uint16_t CompOilT;                  //压缩机油温
uint16_t OilPDif = 0;               //油泵压差
uint16_t GVaneAg = 0;               //导叶开度
uint16_t MotorWindT = 0;            //电机绕组温度
uint16_t CHFaultState = 0;			//机组故障
uint16_t CHWhCRatio = 0;            //机组电流百分比

uint16_t CHWhV = 0;                 //机组相电压

uint16_t CHWhC = 0;                 //机组相电流
uint16_t CHWhC_U = 0;               //机组U相电流
uint16_t CHWhC_V = 0;               //机组V相电流
uint16_t CHWhC_W = 0;               //机组W相电流

uint16_t CompRatedCurrent = 683;    //压缩机额定电流设定

uint16_t CHRateCoolCap = 2285;      //机组额定制冷量
uint16_t CHCoolCap = 0;             //机组制冷量
int16_t  FullLoadDesEvapT = 55;     //满负荷设计蒸发温度
uint16_t CWAntiFreezeT = 30;        //冷冻水防冻温度设定
uint16_t MinStartStopInv = 20;      //最少启停间隔时间设定
uint16_t CHCHWOTSP = 70;		    //机组冷冻出水温度设定
uint16_t CHCHWOTSPRx = 0;			//机组冷冻出水温度设定

uint32_t CHCom1Runtime = 0;			//机组压缩机一运行时间
uint32_t CHCom2Runtime = 0;			//机组压缩机二运行时间
uint32_t CHRuntime = 0;			    //机组运行时间

uint16_t CHLoad = 0;				//机组总负荷
uint32_t CHWhW = 0;                 //机组耗电量
uint16_t CHWhW_H = 0;               //机组耗电量（高位）
uint16_t CHWhW_L = 0;               //机组耗电量（低位）
uint32_t CHWhP = 0;                 //机组耗电功率
uint16_t CHWhP_H = 0;               //机组耗电功率（高位）
uint16_t CHWhP_L = 0;               //机组耗电功率（低位）

BOOL CHRunState = 0;			    //机组开关机状态
uint8_t CHCom1RunState = 1;			//机组压缩机一运行状态
uint8_t CHCom2RunState = 0;			//机组压缩机二运行状态
uint8_t CHProtectFlag = 0;			//机组保护停机标志位
uint8_t CHFaultStateClear = 0;	    //机组故障清除
uint8_t CHRYWPState = 0;			//机组热源侧水泵启停状态
uint8_t CHCommBoardFault = 0;		//控制器与机组触摸屏通讯故障
uint8_t CHCommHMIFault = 0;			//触摸屏与机组主板通讯故障

uint8_t CHFaultClearRx = 0;			//机组故障清除
uint16_t CHRunCtrlRx = 0;			//机组启停控制接收，用于与命令下发比较
uint8_t CHFaultClear;			    //机组故障清除

uint8_t CHWSwState=1;                //冷冻水流开关状态
uint8_t CWSwState=1;                 //冷却水流开关状态

uint8_t CHWV_Control = 0;            //冷冻侧蝶阀控制
uint8_t CHWV_Open = 0;               //冷冻侧蝶阀开控制
uint8_t CHWV_OpenedState = 0;	     //冷冻侧蝶阀开到位状态
uint8_t CHWV_Close = 0;              //冷冻侧蝶阀关控制
uint8_t CHWV_ClosedState = 0;	     //冷冻侧蝶阀关到位状态
uint8_t CHWV_FaultState = 0;	     //冷冻侧蝶阀故障状态
uint8_t CHWV_RemoteEn = 0;           //冷冻侧蝶阀就地/远程状态

uint8_t CWV_Control = 0;             //冷却侧蝶阀控制
uint8_t CWV_Open = 0;                //冷却侧蝶阀开控制
uint8_t CWV_OpenedState = 0;	     //冷却侧蝶阀开到位状态
uint8_t CWV_Close = 0;               //冷却侧蝶阀关控制
uint8_t CWV_ClosedState = 0;	     //冷却侧蝶阀关到位状态
uint8_t CWV_FaultState = 0;	         //冷却侧蝶阀故障状态
uint8_t CWV_RemoteEn = 0;            //冷却侧蝶阀就地/远程状态

/* DTU协议数据*/
uint8_t  DTU_Changed = 0;
uint16_t DTU_Inited = 0;
uint16_t QK_ProductID = 0x5601;

uint16_t DTUProtocolVer;
uint16_t HexVer = 10;

uint8_t SysSwitchEN = 0;
uint8_t SysTimerEN  = 0;
uint8_t SysCloseEN  = 0;

uint16_t TimerEn_MonToThur = 0;	
uint16_t TimerEn_FriToSun = 0;	

uint16_t Energy_MonthWH =0;
uint16_t Energy_MonthWL =0;

uint16_t Energy_YearWH =0;
uint16_t Energy_YearWL =0;

uint16_t Energy_TotalWH =0;
uint16_t Energy_TotalWL =0;

uint16_t Energy_DayEH =0;
uint16_t Energy_DayEL =0;

uint16_t Energy_MonthEH =0;
uint16_t Energy_MonthEL =0;

uint16_t Energy_YearEH = 0;
uint16_t Energy_YearEL = 0;

uint16_t Energy_TotalEL = 0;
uint16_t Energy_TotalEH = 0;

uint16_t DTU_State_All_111 = 0;        //DTU的word111，主要指示有无机组、水泵
uint16_t Logic_B1State_CHWP = 0;
uint16_t Logic_B2State_CHWP = 0;
uint16_t Logic_B3State_CHWP = 0;
uint16_t Logic_B4State_CHWP = 0;
uint16_t Logic_B5State_CHWP = 0;
uint16_t Logic_B6State_CHWP = 0;
       
uint16_t Logic_B1State_CWP = 0;
uint16_t Logic_B2State_CWP = 0;
uint16_t Logic_B3State_CWP = 0;
uint16_t Logic_B4State_CWP = 0;
uint16_t Logic_B5State_CWP = 0;
uint16_t Logic_B6State_CWP = 0;

uint16_t DTU_State_All_112 = 0;        //DTU的word112，主要指示有无风机
uint16_t Logic_B1State_CTF = 0;
uint16_t Logic_B2State_CTF = 0;
uint16_t Logic_B3State_CTF = 0;
uint16_t Logic_B4State_CTF = 0;

uint16_t Logic_B1State_CH = 0;
uint16_t Logic_B2State_CH = 0;
uint16_t Logic_B3State_CH = 0;
uint16_t Logic_B4State_CH = 0;

uint16_t DTU_State_All_113 = 0;      //DTU的word113   主要指示有无蝶阀
uint16_t DTU_State_All_114 = 0;      //DTU的word114   主要指示有无蝶阀


uint16_t Logic_B1State_CHWV = 0;
uint16_t Logic_B2State_CHWV = 0;
uint16_t Logic_B3State_CHWV = 0;
uint16_t Logic_B4State_CHWV = 0;

uint16_t Logic_B1State_CWV = 0;
uint16_t Logic_B2State_CWV = 0;
uint16_t Logic_B3State_CWV = 0;
uint16_t Logic_B4State_CWV = 0;

uint16_t Logic_B1State_TFV = 0;
uint16_t Logic_B2State_TFV = 0;
uint16_t Logic_B3State_TFV = 0;
uint16_t Logic_B4State_TFV = 0;

uint8_t  B1State_CHControl = 0;		       
uint8_t  B2State_CHControl = 0;	
uint8_t  B3State_CHControl = 0;	
uint8_t  B4State_CHControl = 0;	

uint8_t  B1State_CHFaultClear = 0;
uint8_t  B2State_CHFaultClear = 0;
uint8_t  B3State_CHFaultClear = 0;
uint8_t  B4State_CHFaultClear = 0;

uint8_t  B1State_CHRemoteEn = 0;
uint8_t  B2State_CHRemoteEn = 0;
uint8_t  B3State_CHRemoteEn = 0;
uint8_t  B4State_CHRemoteEn = 0;

uint16_t B1State_CH_WhWH = 0; 
uint16_t B1State_CH_WhWL = 0; 
uint16_t B2State_CH_WhWH = 0; 
uint16_t B2State_CH_WhWL = 0; 
uint16_t B3State_CH_WhWH = 0; 
uint16_t B3State_CH_WhWL = 0; 
uint16_t B4State_CH_WhWH = 0; 
uint16_t B4State_CH_WhWL = 0; 

uint16_t B1State_CHWP_WhWH = 0; 
uint16_t B1State_CHWP_WhWL = 0; 
uint16_t B2State_CHWP_WhWH = 0; 
uint16_t B2State_CHWP_WhWL = 0; 
uint16_t B3State_CHWP_WhWH = 0; 
uint16_t B3State_CHWP_WhWL = 0; 
uint16_t B4State_CHWP_WhWH = 0; 
uint16_t B4State_CHWP_WhWL = 0; 
uint16_t B5State_CHWP_WhWH = 0; 
uint16_t B5State_CHWP_WhWL = 0; 
uint16_t B6State_CHWP_WhWH = 0; 
uint16_t B6State_CHWP_WhWL = 0; 

uint16_t B1State_CWP_WhWH = 0; 
uint16_t B1State_CWP_WhWL = 0; 
uint16_t B2State_CWP_WhWH = 0; 
uint16_t B2State_CWP_WhWL = 0; 
uint16_t B3State_CWP_WhWH = 0; 
uint16_t B3State_CWP_WhWL = 0; 
uint16_t B4State_CWP_WhWH = 0; 
uint16_t B4State_CWP_WhWL = 0; 
uint16_t B5State_CWP_WhWH = 0; 
uint16_t B5State_CWP_WhWL = 0; 
uint16_t B6State_CWP_WhWH = 0; 
uint16_t B6State_CWP_WhWL = 0; 

uint16_t B1State_TF_WhWH = 0; 
uint16_t B1State_TF_WhWL = 0; 
uint16_t B2State_TF_WhWH = 0; 
uint16_t B2State_TF_WhWL = 0; 
uint16_t B3State_TF_WhWH = 0; 
uint16_t B3State_TF_WhWL = 0; 
uint16_t B4State_TF_WhWH = 0; 
uint16_t B4State_TF_WhWL = 0; 

uint16_t B1State_CH_WhP = 0; 
uint16_t B2State_CH_WhP = 0;  
uint16_t B3State_CH_WhP = 0; 
uint16_t B4State_CH_WhP = 0; 

uint16_t B1State_CHWP_WhP = 0; 
uint16_t B2State_CHWP_WhP = 0; 
uint16_t B3State_CHWP_WhP = 0; 
uint16_t B4State_CHWP_WhP = 0; 
uint16_t B5State_CHWP_WhP = 0; 
uint16_t B6State_CHWP_WhP = 0; 

uint16_t B1State_CWP_WhP = 0; 
uint16_t B2State_CWP_WhP = 0; 
uint16_t B3State_CWP_WhP = 0; 
uint16_t B4State_CWP_WhP = 0; 
uint16_t B5State_CWP_WhP = 0; 
uint16_t B6State_CWP_WhP = 0; 

uint16_t B1State_TF_WhP = 0; 
uint16_t B2State_TF_WhP = 0; 
uint16_t B3State_TF_WhP = 0; 
uint16_t B4State_TF_WhP = 0; 
