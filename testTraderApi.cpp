// testTraderApi.cpp : 定义控制台应用程序的入口点。
//
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <errno.h>
#include <iostream>

using namespace std;
#include "../TraderAPI/ThostFtdcTraderApi.h"
#include "TraderSpi.h"

// 线程控制相关
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

// 登录请求结构体
CThostFtdcReqUserLoginField userLoginField;
// 用户请求结构体
CThostFtdcUserLogoutField userLogoutField;

// UserApi对象
CThostFtdcTraderApi* pUserApi;

char  FRONT_ADDR[] = "tcp://180.168.146.187:10030";		// 前置地址
/**
TThostFtdcBrokerIDType	BROKER_ID = "9999";				// 经纪公司代码
TThostFtdcInvestorIDType INVESTOR_ID = "110623";			// 注意输入你自己的simnow仿真投资者代码
TThostFtdcPasswordType  PASSWORD = "xialei1981";			// 注意输入你自己的simnow仿真用户密码
*/

TThostFtdcInstrumentIDType INSTRUMENT_ID = "ag1801";	// 合约代码 ，注意与时俱进改变合约ID,避免使用过时合约
TThostFtdcDirectionType	DIRECTION = THOST_FTDC_D_Sell;	// 买卖方向
TThostFtdcPriceType	LIMIT_PRICE = 2380;				// 价格

// 请求编号
//int iRequestID = 0;
// 线程同步标志
sem_t sem;

/**
// 会话参数
TThostFtdcFrontIDType	FRONT_ID;	//前置编号
TThostFtdcSessionIDType	SESSION_ID;	//会话编号
TThostFtdcOrderRefType	ORDER_REF;	//报单引用
time_t lOrderTime;
time_t lOrderOkTime;



//export LD_LIBRARY_PATH=/sboot/TraderAPI:$LD_LIBRARY_PATH 
class CTraderSpi : public CThostFtdcTraderSpi
{
public:
	CTraderSpi (CThostFtdcTraderApi *pUserApi) : m_pUserApi (pUserApi) {printf("CTraderSpi:called.\n");} 

	~CTraderSpi(){}
    
	
	///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
	virtual void OnFrontConnected(){
		printf("OnFrontConnected:called.\n");
		static int i = 0;
		// 在登出后系统会重新调用OnFrontConnected，这里简单判断并忽略第1次之后的所有调用。
		if (i++==0) {
        	printf("OnFrontConnected:called.\n");
			//sem_post(&sem);
		}
	    //用户登录请求
	    ReqUserLogin();
	}

	void ReqUserLogin()
	{
		CThostFtdcReqUserLoginField reqUserLogin ; 
		memset(&reqUserLogin, 0, sizeof(reqUserLogin));
		strcpy(reqUserLogin.BrokerID,BROKER_ID);
		strcpy(reqUserLogin.UserID,INVESTOR_ID);
		strcpy(reqUserLogin.Password,PASSWORD); 
		printf ("ReqUserLogin.\n");            
        int iResult = m_pUserApi->ReqUserLogin(&reqUserLogin, ++iRequestID);         
        cerr << "--->>> 发送用户登录请求: "<<iRequestID << ((iResult == 0) ? "成功" : "失败") << endl;
	}

	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
    {
		cerr << "--->>> " << "OnRspUserLogin:"<<pRspInfo->ErrorID<<pRspUserLogin->FrontID<<","<<pRspUserLogin->SessionID<<","<<pRspUserLogin->SHFETime<< endl;
		if(pRspInfo->ErrorID != 0){
		   //登录失败，客户端需要进行错误处理
		   printf("Failed to login, errorcode = %d errormsg= %s requestid = %d chain = %d", pRspInfo->ErrorID, pRspInfo->ErrorMsg, nRequestID, bIsLast);
		   exit(-1);
		}

		if (bIsLast && !IsErrorRspInfo(pRspInfo))
		{
		// 保存会话参数
		FRONT_ID = pRspUserLogin->FrontID;
		SESSION_ID = pRspUserLogin->SessionID;
		int iNextOrderRef = atoi(pRspUserLogin->MaxOrderRef);
		cerr << "--->>> " << "FRONT_ID:"<<FRONT_ID<<"," <<SESSION_ID<<iNextOrderRef<<","<<bIsLast<< endl;
		iNextOrderRef++;
		sprintf(ORDER_REF, "%d", iNextOrderRef);
		///获取当前交易日
		cerr << "--->>> 获取当前交易日 = " << pUserApi->GetTradingDay() << endl;
		///投资者结算结果确认
		ReqSettlementInfoConfirm();
		}

	}

	virtual void ReqSettlementInfoConfirm()
	{
	   	CThostFtdcSettlementInfoConfirmField req;
	    memset(&req, 0, sizeof(req));
		strcpy(req.BrokerID, BROKER_ID);
		strcpy(req.InvestorID, INVESTOR_ID);
		int iResult = m_pUserApi->ReqSettlementInfoConfirm(&req, ++iRequestID);
		cerr << "--->>> 投资者结算结果确认: " << ((iResult == 0) ? "成功" : "失败") << endl;
	}

	virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		cerr << "--->>> " << "OnRspSettlementInfoConfirm" << endl;
		if (bIsLast && !IsErrorRspInfo(pRspInfo))
		{
			///请求查询合约
			ReqQryInstrument();
		}
	}
    ///请求查询合约
	virtual void ReqQryInstrument()
	{
		CThostFtdcQryInstrumentField req;
		memset(&req, 0, sizeof(req));
		strcpy(req.InstrumentID, INSTRUMENT_ID);
		int iResult = m_pUserApi->ReqQryInstrument(&req, ++iRequestID);
		cerr << "--->>> 请求查询合约: " << ((iResult == 0) ? "成功" : "失败") << endl;
	}
    ///请求查询合约响应
	virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		cerr << "--->>> " << "OnRspQryInstrument" <<nRequestID<< endl;
		if (bIsLast && !IsErrorRspInfo(pRspInfo))
		{
			//cerr << "--->>> " << "DeliveryYear:" <<pInstrument->DeliveryYear<< endl;
			
			///请求查询合约
			ReqQryTradingAccount();
		}
	}
    ///请求查询资金账户
	virtual void ReqQryTradingAccount()
	{
		CThostFtdcQryTradingAccountField req;
		memset(&req, 0, sizeof(req));
		strcpy(req.BrokerID, BROKER_ID);
		strcpy(req.InvestorID, INVESTOR_ID);
		//strcpy(req.CurrencyID, "RMB");
		sleep(1);
		int iResult = m_pUserApi->ReqQryTradingAccount(&req, ++iRequestID);
		cerr << "--->>> 请求查询资金账户: " << ((iResult == 0) ? "成功" : "失败")<<iResult << endl;
	}
	///请求查询资金账户响应
	virtual void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		cerr << "--->>> " << "OnRspQryTradingAccount:"<<pTradingAccount->PreDeposit << endl;
		
		 
		if (bIsLast && !IsErrorRspInfo(pRspInfo))
		{
			///请求查询投资者持仓
			sleep(1);
			ReqQryInvestorPosition();
		}
	}
    ///请求查询投资者持仓
	virtual void ReqQryInvestorPosition()
	{
		CThostFtdcQryInvestorPositionField req;
		memset(&req, 0, sizeof(req));
		strcpy(req.BrokerID, BROKER_ID);
		strcpy(req.InvestorID, INVESTOR_ID);
		strcpy(req.InstrumentID, INSTRUMENT_ID);
		int iResult = pUserApi->ReqQryInvestorPosition(&req, ++iRequestID);
		cerr << "--->>> 请求查询投资者持仓: " << ((iResult == 0) ? "成功" : "失败") << endl;
	}
    ///请求查询投资者持仓响应
	virtual void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		cerr << "--->>> " << "OnRspQryInvestorPosition" << endl;
		if (bIsLast && !IsErrorRspInfo(pRspInfo))
		{
			///报单录入请求
			ReqOrderInsert();
		}
	}
    ///报单录入请求
	virtual void ReqOrderInsert()
	{
		CThostFtdcInputOrderField req;
		memset(&req, 0, sizeof(req));
		///经纪公司代码
		strcpy(req.BrokerID, BROKER_ID);
		///投资者代码
		strcpy(req.InvestorID, INVESTOR_ID);
		///合约代码
		strcpy(req.InstrumentID, INSTRUMENT_ID);
		///报单引用
		strcpy(req.OrderRef, ORDER_REF);
		///用户代码
	//	TThostFtdcUserIDType	UserID;
		///报单价格条件: 限价
		req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
		///买卖方向: 
		req.Direction = DIRECTION;
		///组合开平标志: 开仓
		req.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
		///组合投机套保标志
		req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
		///价格
		req.LimitPrice = LIMIT_PRICE;
		///数量: 1
		req.VolumeTotalOriginal = 1;
		///有效期类型: 当日有效
		req.TimeCondition = THOST_FTDC_TC_GFD;
		///GTD日期
	//	TThostFtdcDateType	GTDDate;
		///成交量类型: 任何数量
		req.VolumeCondition = THOST_FTDC_VC_AV;
		///最小成交量: 1
		req.MinVolume = 1;
		///触发条件: 立即
		req.ContingentCondition = THOST_FTDC_CC_Immediately;
		///止损价
	//	TThostFtdcPriceType	StopPrice;
		///强平原因: 非强平
		req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
		///自动挂起标志: 否
		req.IsAutoSuspend = 0;
		///业务单元
	//	TThostFtdcBusinessUnitType	BusinessUnit;
		///请求编号
	//	TThostFtdcRequestIDType	RequestID;
		///用户强评标志: 否
		req.UserForceClose = 0;

		lOrderTime=time(NULL);
		int iResult = pUserApi->ReqOrderInsert(&req, ++iRequestID);
		cerr << "--->>> 报单录入请求: " << ((iResult == 0) ? "成功" : "失败") << endl;
	}

	void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		cerr << "--->>> " << "OnRspOrderInsert" << endl;
		IsErrorRspInfo(pRspInfo);
	}

	void ReqOrderAction(CThostFtdcOrderField *pOrder)
	{
		static bool ORDER_ACTION_SENT = false;		//是否发送了报单
		if (ORDER_ACTION_SENT)
			return;

		CThostFtdcInputOrderActionField req;
		memset(&req, 0, sizeof(req));
		///经纪公司代码
		strcpy(req.BrokerID, pOrder->BrokerID);
		///投资者代码
		strcpy(req.InvestorID, pOrder->InvestorID);
		///报单操作引用
	//	TThostFtdcOrderActionRefType	OrderActionRef;
		///报单引用
		strcpy(req.OrderRef, pOrder->OrderRef);
		///请求编号
	//	TThostFtdcRequestIDType	RequestID;
		///前置编号
		req.FrontID = FRONT_ID;
		///会话编号
		req.SessionID = SESSION_ID;
		///交易所代码
	//	TThostFtdcExchangeIDType	ExchangeID;
		///报单编号
	//	TThostFtdcOrderSysIDType	OrderSysID;
		///操作标志
		req.ActionFlag = THOST_FTDC_AF_Delete;
		///价格
	//	TThostFtdcPriceType	LimitPrice;
		///数量变化
	//	TThostFtdcVolumeType	VolumeChange;
		///用户代码
	//	TThostFtdcUserIDType	UserID;
		///合约代码
		strcpy(req.InstrumentID, pOrder->InstrumentID);
		lOrderTime=time(NULL);
		int iResult = pUserApi->ReqOrderAction(&req, ++iRequestID);
		cerr << "--->>> 报单操作请求: " << ((iResult == 0) ? "成功" : "失败") << endl;
		ORDER_ACTION_SENT = true;
	}

	void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		cerr << "--->>> " << "OnRspOrderAction" << endl;
		IsErrorRspInfo(pRspInfo);
	}

	///报单通知
	void OnRtnOrder(CThostFtdcOrderField *pOrder)
	{
		cerr << "--->>> " << "OnRtnOrder"  << endl;
		lOrderOkTime=time(NULL);
		time_t lTime=lOrderOkTime-lOrderTime;
		cerr << "--->>> 报单到报单通知的时间差 = " << lTime << endl;
		if (IsMyOrder(pOrder))
		{
			if (IsTradingOrder(pOrder))
			{
				//ReqOrderAction(pOrder);
			}
			else if (pOrder->OrderStatus == THOST_FTDC_OST_Canceled)
				cout << "--->>> 撤单成功" << endl;
		}
	}

	///成交通知
	void OnRtnTrade(CThostFtdcTradeField *pTrade)
	{
		cerr << "--->>> " << "OnRtnTrade"  << endl;
	}

	void OnFrontDisconnected(int nReason)
	{
		cerr << "--->>> " << "OnFrontDisconnected" << endl;
		cerr << "--->>> Reason = " << nReason << endl;
	}
			
	void OnHeartBeatWarning(int nTimeLapse)
	{
		cerr << "--->>> " << "OnHeartBeatWarning" << endl;
		cerr << "--->>> nTimerLapse = " << nTimeLapse << endl;
	}

	void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);
		cerr << "--->>> " << "OnRspError" << endl;
		IsErrorRspInfo(pRspInfo);
	}

*/

	bool IsMyOrder(CThostFtdcOrderField *pOrder)
	{
		return ((pOrder->FrontID == FRONT_ID) &&
				(pOrder->SessionID == SESSION_ID) &&
				(strcmp(pOrder->OrderRef, ORDER_REF) == 0));
	}

	bool IsTradingOrder(CThostFtdcOrderField *pOrder)
	{
		return ((pOrder->OrderStatus != THOST_FTDC_OST_PartTradedNotQueueing) &&
				(pOrder->OrderStatus != THOST_FTDC_OST_Canceled) &&
				(pOrder->OrderStatus != THOST_FTDC_OST_AllTraded));
	}



	private:
	bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
	{
		// 如果ErrorID != 0, 说明收到了错误的响应
		bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
		if (bResult)
			cerr << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
		return bResult;
	}


	// 指向CThostFtdcMdApi实例的指针 
    CThostFtdcTraderApi *m_pUserApi ; 


};


int main()
{

    // 初始化线程同步变量
	sem_init(&sem,0,0);
    printf("userid:\n");
    // 等待登录成功消息
	//sem_wait(&sem);

	// 初始化UserApi
	pUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi();			// 创建UserApi
    CTraderSpi* sh= new CTraderSpi();
    pUserApi->RegisterSpi ((CThostFtdcTraderSpi*)sh);   
	//CTraderSpi* pUserSpi = new CTraderSpi();

	//pUserApi->RegisterSpi((CThostFtdcTraderSpi*)pUserSpi);			// 注册事件类
	//pUserApi->SubscribePublicTopic(TERT_RESTART);					// 注册公有流
	//pUserApi->SubscribePrivateTopic(TERT_RESTART);					// 注册私有流
	printf("usereed:\n");
	pUserApi->RegisterFront("tcp://180.168.146.187:10000");	
	pUserApi->Init();
	// 等待服务器发出登录消息
	sem_wait(&sem);
 
 
	printf("usereed:\n");
	//pUserApi->Init();
	printf("usereed:\n");

	pUserApi->Join();
	//pUserApi->Release();
        
    return 0;
}
