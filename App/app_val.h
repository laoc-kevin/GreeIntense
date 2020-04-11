#ifndef _APP_VAL_H_
#define _APP_VAL_H_
#include <stdint.h>

extern uint8_t CHProtocolType;		

extern uint16_t CHID;				//机型ID
extern uint16_t CHType;				//机型
extern uint16_t CHProtocolVer;		//机型协议版本

extern uint16_t CHCommRunCtrl;		//机组启停控制(通讯控制)
extern uint8_t  CHContRunCtrl;		 //机组启停控制(触点控制)
extern uint8_t  CHControl;		     //机组启停

extern uint16_t CHRemoteEn;			//机组远程开关功能

extern uint16_t CHMode;				    //机组控制模式
extern uint16_t CHStartTimes;		    //机组累计启动次数
extern uint16_t CHState;				//机组状态
extern uint16_t CHRunMode;				//机组运行模式


extern uint16_t CHCHWIT;				//机组冷冻进水温度
extern uint16_t CHCHWOT;				//机组冷冻出水温度
extern uint16_t CHCWIT;				    //机组冷却进水温度
extern uint16_t CHCWOT;				    //机组冷却出水温度

extern uint16_t CHCHWIP_B;				//机组冷冻进口压力
extern uint16_t CHCHWOP;				//机组冷冻出口压力
extern uint16_t CHCWIP_B;				//机组冷却进口压力
extern uint16_t CHCWOP;				    //机组冷却出口压力
extern uint16_t CHCHWIP_F;				//机组冷冻进口压力(过滤前)
extern uint16_t CHCWIP_F;				//机组冷却进口压力(过滤前)

extern int16_t  CHEvapT;               //机组蒸发温度
extern int16_t  CHCondT;               //机组冷凝温度

extern int16_t  CHEvapP;               //机组蒸发压力
extern int16_t  CHCondP;               //机组冷凝压力
 
extern uint16_t CompDisT;              //排气温度
extern uint16_t CompOilT;              //压缩机油温
extern uint16_t OilPDif;              //油泵压差
extern uint16_t GVaneAg;              //导叶开度
extern uint16_t MotorWindT;            //电机绕组温度

extern uint16_t CHCHWOTSP;				//机组冷冻出水温度设定
extern uint16_t CHCHWOTSPRx;			//机组冷冻出水温度设定

extern uint32_t CHCom1Runtime;			//机组压缩机一运行时间
extern uint32_t CHCom2Runtime;			//机组压缩机二运行时间
extern uint32_t CHRuntime;			    //机组运行时间


extern uint16_t CHLoad;				   //机组总负荷
extern uint32_t CHWhW;                 //机组耗电量
extern uint16_t CHWhW_H;               //机组耗电量（高位）
extern uint16_t CHWhW_L;               //机组耗电量（低位）

extern uint32_t CHWhP;                 //机组耗电功率
extern uint16_t CHWhP_H;               //机组耗电功率（高位）
extern uint16_t CHWhP_L;               //机组耗电功率（低位）

extern uint16_t CHWhCRatio;            //机组电流百分比
extern uint16_t CHWhV;                 //机组相电压
extern uint16_t CHWhC;                 //机组相电流
extern uint16_t CHWhC_U;               //机组U相电流
extern uint16_t CHWhC_V;               //机组V相电流
extern uint16_t CHWhC_W;               //机组W相电流

extern uint16_t CompRatedCurrent;      //压缩机额定电流设定

extern uint8_t CHFaultStateClear;			//机组故障清除
extern uint16_t CHFaultState;			    //机组故障
 
extern uint8_t CHRunState;			    //机组运行状态
extern uint8_t CHCom1RunState;			//机组压缩机一运行状态
extern uint8_t CHCom2RunState;			//机组压缩机二运行状态
extern uint8_t CHProtectFlag;			//机组保护停机标志位 
extern uint8_t CHCommHMIFault;			//触摸屏与机组主板通讯故障
extern uint8_t CHRYWPState;			    //机组热源侧水泵启停状态
extern uint8_t CHCommBoardFault;		//控制器与机组触摸屏通讯故障
extern uint8_t CHFaultClearRx;			//机组故障清除
extern uint16_t CHRunCtrlRx;			//机组启停控制接收，用于与命令下发比较

extern uint8_t CWSwState;                   //冷冻水流开关状态
extern uint8_t CHWSwState;                  //冷却水流开关状态

extern uint8_t CHWV_Control ;            //冷冻侧蝶阀控制
extern uint8_t CHWV_Open;               //冷冻侧蝶阀开控制
extern uint8_t CHWV_OpenedState;	     //冷冻侧蝶阀开到位状态
extern uint8_t CHWV_Close;              //冷冻侧蝶阀关控制
extern uint8_t CHWV_ClosedState;	     //冷冻侧蝶阀关到位状态
extern uint8_t CHWV_FaultState;	     //冷冻侧蝶阀故障状态
extern uint8_t CHWV_RemoteEn;           //冷冻侧蝶阀就地/远程状态

extern uint8_t CWV_Control;             //冷却侧蝶阀控制
extern uint8_t CWV_Open;                //冷却侧蝶阀开控制
extern uint8_t CWV_OpenedState;	     //冷却侧蝶阀开到位状态
extern uint8_t CWV_Close;               //冷却侧蝶阀关控制
extern uint8_t CWV_ClosedState;	     //冷却侧蝶阀关到位状态
extern uint8_t CWV_FaultState;	         //冷却侧蝶阀故障状态
extern uint8_t CWV_RemoteEn;            //冷却侧蝶阀就地/远程状态

extern uint16_t  CHRateCoolCap;           //机组额定制冷量
extern uint16_t CHCoolCap;               //机组制冷量
extern int16_t  FullLoadDesEvapT;        //满负荷设计蒸发温度
extern uint16_t CWAntiFreezeT;           //冷冻水防冻温度设定
extern uint16_t MinStartStopInv;         //最少启停间隔时间设定

extern uint8_t  DTU_Changed;
extern uint16_t  DTU_Inited;
extern uint16_t QK_ProductID ;
extern uint16_t DTUProtocolVer;
extern uint16_t HexVer;

extern uint8_t SysSwitchEN;
extern uint8_t SysTimerEN;
extern uint8_t SysCloseEN;

extern uint16_t TimerEn_MonToThur;	
extern uint16_t TimerEn_FriToSun;	


extern uint16_t Energy_MonthWH;
extern uint16_t Energy_MonthWL;

extern uint16_t Energy_YearWH;
extern uint16_t Energy_YearWL;

extern uint16_t Energy_TotalWH;
extern uint16_t Energy_TotalWL;

extern uint16_t Energy_DayEH;
extern uint16_t Energy_DayEL;

extern uint16_t Energy_MonthEH;
extern uint16_t Energy_MonthEL;

extern uint16_t Energy_YearEH;
extern uint16_t Energy_YearEL;

extern uint16_t Energy_TotalEL;
extern uint16_t Energy_TotalEH;


extern uint16_t DTU_State_All_111;
extern uint16_t DTU_State_All_112;
extern uint16_t DTU_State_All_113;
extern uint16_t DTU_State_All_114;

extern uint16_t Logic_B1State_CHWP;
extern uint16_t Logic_B2State_CHWP;
extern uint16_t Logic_B3State_CHWP;
extern uint16_t Logic_B4State_CHWP;
extern uint16_t Logic_B5State_CHWP;
extern uint16_t Logic_B6State_CHWP;

extern uint16_t Logic_B1State_CWP;
extern uint16_t Logic_B2State_CWP;
extern uint16_t Logic_B3State_CWP;
extern uint16_t Logic_B4State_CWP;
extern uint16_t Logic_B5State_CWP;
extern uint16_t Logic_B6State_CWP;

extern uint16_t Logic_B1State_CTF;
extern uint16_t Logic_B2State_CTF;
extern uint16_t Logic_B3State_CTF;
extern uint16_t Logic_B4State_CTF;

extern uint16_t Logic_B1State_CH;
extern uint16_t Logic_B2State_CH;
extern uint16_t Logic_B3State_CH;
extern uint16_t Logic_B4State_CH;

extern uint16_t Logic_B1State_CHWV;
extern uint16_t Logic_B2State_CHWV;
extern uint16_t Logic_B3State_CHWV;
extern uint16_t Logic_B4State_CHWV;

extern uint16_t Logic_B1State_CWV;
extern uint16_t Logic_B2State_CWV;
extern uint16_t Logic_B3State_CWV;
extern uint16_t Logic_B4State_CWV;


extern uint16_t Logic_B1State_TFV;
extern uint16_t Logic_B2State_TFV;
extern uint16_t Logic_B3State_TFV;
extern uint16_t Logic_B4State_TFV;

extern uint8_t  B1State_CHControl;		       
extern uint8_t  B2State_CHControl;	
extern uint8_t  B3State_CHControl;	
extern uint8_t  B4State_CHControl;	

extern uint8_t  B1State_CHFaultClear;
extern uint8_t  B2State_CHFaultClear;
extern uint8_t  B3State_CHFaultClear;
extern uint8_t  B4State_CHFaultClear;

extern uint8_t  B1State_CHRemoteEn;
extern uint8_t  B2State_CHRemoteEn;
extern uint8_t  B3State_CHRemoteEn;
extern uint8_t  B4State_CHRemoteEn;


extern uint16_t B1State_CH_WhWH; 
extern uint16_t B1State_CH_WhWL; 
extern uint16_t B2State_CH_WhWH; 
extern uint16_t B2State_CH_WhWL; 
extern uint16_t B3State_CH_WhWH; 
extern uint16_t B3State_CH_WhWL; 
extern uint16_t B4State_CH_WhWH; 
extern uint16_t B4State_CH_WhWL; 

extern uint16_t B1State_CHWP_WhWH; 
extern uint16_t B1State_CHWP_WhWL; 
extern uint16_t B2State_CHWP_WhWH; 
extern uint16_t B2State_CHWP_WhWL; 
extern uint16_t B3State_CHWP_WhWH; 
extern uint16_t B3State_CHWP_WhWL; 
extern uint16_t B4State_CHWP_WhWH; 
extern uint16_t B4State_CHWP_WhWL; 
extern uint16_t B5State_CHWP_WhWH; 
extern uint16_t B5State_CHWP_WhWL; 
extern uint16_t B6State_CHWP_WhWH; 
extern uint16_t B6State_CHWP_WhWL; 

extern uint16_t B1State_CWP_WhWH; 
extern uint16_t B1State_CWP_WhWL; 
extern uint16_t B2State_CWP_WhWH; 
extern uint16_t B2State_CWP_WhWL; 
extern uint16_t B3State_CWP_WhWH; 
extern uint16_t B3State_CWP_WhWL; 
extern uint16_t B4State_CWP_WhWH; 
extern uint16_t B4State_CWP_WhWL; 
extern uint16_t B5State_CWP_WhWH; 
extern uint16_t B5State_CWP_WhWL; 
extern uint16_t B6State_CWP_WhWH; 
extern uint16_t B6State_CWP_WhWL; 

extern uint16_t B1State_TF_WhWH; 
extern uint16_t B1State_TF_WhWL; 
extern uint16_t B2State_TF_WhWH; 
extern uint16_t B2State_TF_WhWL; 
extern uint16_t B3State_TF_WhWH; 
extern uint16_t B3State_TF_WhWL; 
extern uint16_t B4State_TF_WhWH; 
extern uint16_t B4State_TF_WhWL; 

extern uint16_t B1State_CH_WhP; 
extern uint16_t B2State_CH_WhP;  
extern uint16_t B3State_CH_WhP; 
extern uint16_t B4State_CH_WhP; 

extern uint16_t B1State_CHWP_WhP; 
extern uint16_t B2State_CHWP_WhP; 
extern uint16_t B3State_CHWP_WhP; 
extern uint16_t B4State_CHWP_WhP; 
extern uint16_t B5State_CHWP_WhP; 
extern uint16_t B6State_CHWP_WhP; 

extern uint16_t B1State_CWP_WhP; 
extern uint16_t B2State_CWP_WhP; 
extern uint16_t B3State_CWP_WhP; 
extern uint16_t B4State_CWP_WhP; 
extern uint16_t B5State_CWP_WhP; 
extern uint16_t B6State_CWP_WhP; 

extern uint16_t B1State_TF_WhP; 
extern uint16_t B2State_TF_WhP; 
extern uint16_t B3State_TF_WhP; 
extern uint16_t B4State_TF_WhP; 




#endif
