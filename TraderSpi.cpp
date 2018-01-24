#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <errno.h>
#include <iostream>

#include <json/json.h>

using namespace std;


#include "../TraderAPI/ThostFtdcTraderApi.h"
#include  "TraderSpi.h"
#include <time.h>


#pragma warning(disable : 4996)

// USER_API参数
extern CThostFtdcTraderApi* pUserApi;

// 配置参数
//extern char FRONT_ADDR[];		// 前置地址
//extern char BROKER_ID[];		// 经纪公司代码
extern char INVESTOR_ID[];		// 投资者代码
//extern char PASSWORD[];			// 用户密码
extern char INSTRUMENT_ID[];	// 合约代码
extern TThostFtdcPriceType	LIMIT_PRICE;	// 价格
extern TThostFtdcDirectionType	DIRECTION;	// 买卖方向

// 请求编号
int iRequestID=0;

// 会话参数
//TThostFtdcFrontIDType	FRONT_ID;	//前置编号//
//TThostFtdcSessionIDType	SESSION_ID;	//会话编号
//TThostFtdcOrderRefType	ORDER_REF;	//报单引用

//TThostFtdcBrokerIDType	 BROKER_ID = "9999";				// 经纪公司代码
//TThostFtdcInvestorIDType INVESTOR_ID = "110623";			// 注意输入你自己的simnow仿真投资者代码
//TThostFtdcPasswordType   PASSWORD = "xialei1981";			// 注意输入你自己的simnow仿真用户密码

time_t lOrderTime;
time_t lOrderOkTime;


void CTraderSpi::setWebsocket(client* c,websocketpp::connection_hdl hdl)
{  
	m_client = c; m_hdl =hdl;
}

void CTraderSpi::setUserLoginInfo(int id , const char* BROKER_ID,const char* INVESTOR_ID,const char*  PASSWORD)
{  
    this->loginID = id;
	this->BROKER_ID = new char[strlen(BROKER_ID)+1];
    strcpy(this->BROKER_ID, BROKER_ID);
    this->INVESTOR_ID = new char[strlen(INVESTOR_ID)+1];
    strcpy(this->INVESTOR_ID, INVESTOR_ID);
    this->PASSWORD = new char[strlen(PASSWORD)+1];
    strcpy(this->PASSWORD, PASSWORD);
  
}

std::string CTraderSpi::getJsonStr(int uniqueID,std::string rspType,std::string isError,Json::Value rspArgs)
{   ///获取当前交易日
    Json::Value root;
    root["RspArgs"]  =rspArgs;
    root["UniqueID"] =uniqueID;
    root["RspType"]  =rspType;
    root["IsError"]  =isError;
    
	std::string out;
	try  
    {  
    //root.toStyledString();
    Json::StreamWriterBuilder wbuilder;
    wbuilder["indentation"] = "";
    out = Json::writeString(wbuilder, root);
    
    }catch(std::exception &ex)  
    {  
        printf("StructDataToJsonString exception %s.\n", ex.what());  
        return out;
        
    } 
    std::cout << "'" << out << "'" << std::endl;
    return out;
}

void CTraderSpi::OnFrontConnected()
{
	cerr << "--->>> " << "OnFrontConnected" << endl;
	ReqUserLogin(loginID);
	///用户登录请求
	
}

void CTraderSpi::ReqUserLogin(int loginID)
{
	CThostFtdcReqUserLoginField req;
	//memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, this->BROKER_ID);
	strcpy(req.UserID, this->INVESTOR_ID);
	strcpy(req.Password, this->PASSWORD);
	int iResult = pUserApi->ReqUserLogin(&req, loginID);
	cerr << "--->>> 发送用户登录请求: " <<req.BrokerID<< ((iResult == 0) ? "成功" : "失败") << endl;
}

void CTraderSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspUserLogin" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{

		// 保存会话参数
		this->FRONT_ID = pRspUserLogin->FrontID;
		this->SESSION_ID = pRspUserLogin->SessionID;

		//this->FRONT_ID = iFrontID;
        //strcpy(this->FRONT_ID, pRspUserLogin->FrontID);
        //this->SESSION_ID = new char[strlen(pRspUserLogin->SESSION_ID)+1];
        //strcpy(this->SESSION_ID, pRspUserLogin->SESSION_ID);

		//SESSION_ID = pRspUserLogin->SessionID;
		cerr << "--->>> " << "OnRspUserLogin2" << endl;
		//int iNextOrderRef = atoi(pRspUserLogin->MaxOrderRef);
		//iNextOrderRef++;
		//char* ORDER_REF;
		//sprintf(ORDER_REF, "%d", iNextOrderRef);
		//cerr << "--->>> " << "OnRspUserLogin4" << endl;
		//this->ORDER_REF = new char[strlen(ORDER_REF)+1];
		//cerr << "--->>> " << "OnRspUserLogin5" << endl;
		this->ORDER_REF = new char[strlen(pRspUserLogin->MaxOrderRef)+1];
		strcpy(this->ORDER_REF, pRspUserLogin->MaxOrderRef);
        cerr << "--->>> " << "OnRspUserLogin3" << endl;
		///获取当前交易日
		Json::Value rspArgs;
        rspArgs["TradingDay"] = pRspUserLogin->TradingDay;
        rspArgs["LoginTime"] = pRspUserLogin->LoginTime;
        rspArgs["BrokerID"] = pRspUserLogin->BrokerID;
        rspArgs["UserID"] = pRspUserLogin->UserID;
        rspArgs["SystemName"] = pRspUserLogin->SystemName;
        rspArgs["FrontID"] = pRspUserLogin->FrontID;
        rspArgs["SessionID"] = pRspUserLogin->SessionID;
        rspArgs["MaxOrderRef"] = pRspUserLogin->MaxOrderRef;
        rspArgs["SHFETime"] = pRspUserLogin->SHFETime;
        rspArgs["DCETime"] = pRspUserLogin->DCETime;
        rspArgs["CZCETime"] = pRspUserLogin->CZCETime;
        rspArgs["FFEXTime"] = pRspUserLogin->FFEXTime;
        rspArgs["INETime"] = pRspUserLogin->INETime;
        cerr << "--->>> 获取当前交易日 = " << pUserApi->GetTradingDay() << endl;
		//std::string SIP_msg="xialei1981\r\n";
        m_client->send(m_hdl, getJsonStr(nRequestID,"OnRspUserLogin","false",rspArgs).c_str()
        		, websocketpp::frame::opcode::text);
		///投资者结算结果确认,
		//ReqSettlementInfoConfirm();
	}
}

void CTraderSpi::ReqSettlementInfoConfirm(Json::Value root)
{
	CThostFtdcSettlementInfoConfirmField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.InvestorID, INVESTOR_ID);
	int iResult = pUserApi->ReqSettlementInfoConfirm(&req, root["UniqueID"].asInt());
	cerr << "--->>> 投资者结算结果确认: " << ((iResult == 0) ? "成功" : "失败") << endl;
}

void CTraderSpi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspSettlementInfoConfirm" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		///请求查询合约
		ReqQryInstrument();
	}
}

void CTraderSpi::ReqQryInstrument()
{
	CThostFtdcQryInstrumentField req;
	memset(&req, 0, sizeof(req));
	//add xialei strcpy(req.InstrumentID, INSTRUMENT_ID);
	int iResult = pUserApi->ReqQryInstrument(&req, ++iRequestID);
	cerr << "--->>> 请求查询合约: " << ((iResult == 0) ? "成功" : "失败") << endl;
}

void CTraderSpi::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspQryInstrument" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		///请求查询合约
		ReqQryTradingAccount();
	}
}

void CTraderSpi::ReqQryTradingAccount()
{
	CThostFtdcQryTradingAccountField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.InvestorID, INVESTOR_ID);
	int iResult = pUserApi->ReqQryTradingAccount(&req, ++iRequestID);
	cerr << "--->>> 请求查询资金账户: " << ((iResult == 0) ? "成功" : "失败") << endl;
}

void CTraderSpi::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspQryTradingAccount" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		///请求查询投资者持仓
		ReqQryInvestorPosition();
	}
}

void CTraderSpi::ReqQryInvestorPosition()
{
	CThostFtdcQryInvestorPositionField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.InvestorID, INVESTOR_ID);
	//add xialei strcpy(req.InstrumentID, INSTRUMENT_ID);
	int iResult = pUserApi->ReqQryInvestorPosition(&req, ++iRequestID);
	cerr << "--->>> 请求查询投资者持仓: " << ((iResult == 0) ? "成功" : "失败") << endl;
}

void CTraderSpi::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspQryInvestorPosition" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
        
	}
}

void CTraderSpi::ReqOrderInsert(Json::Value root)
{
	CThostFtdcInputOrderField req;
	memset(&req, 0, sizeof(req));
    ///经纪公司代码
    strcpy(req.BrokerID, BROKER_ID);
    ///投资者代码
    strcpy(req.InvestorID, INVESTOR_ID);
    ///报单价格条件: 限价
    req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
    ///报单引用
    strcpy(req.OrderRef, root["ReqArgs"]["OrderRef"].asString().c_str());
    ///用户代码
    // TThostFtdcUserIDType  UserID
    ///合约代码
    strcpy(req.InstrumentID, root["ReqArgs"]["InstrumentID"].asString().c_str());
    ///买卖方向: 
    req.Direction=root["ReqArgs"]["Direction"].asString().c_str()[0];
    //strcpy(&req.Direction, root["ReqArgs"]["Direction"].asString().c_str());
    //买 THOST_FTDC_D_Buy '0'/ 卖THOST_FTDC_D_Sell '1'
    //req.Direction = root["ReqArgs"]["Direction"].asString()[0];
    ///组合开平标志: 开仓
    req.CombOffsetFlag[0] = root["ReqArgs"]["CombOffsetFlag"].asString().c_str()[0];
    //strcpy(&req.CombOffsetFlag[0], root["ReqArgs"]["CombOffsetFlag"].asString().c_str());
    //req.CombOffsetFlag[0] = const_cast<char *>(root["ReqArgs"]["Direction"].asString().c_str());//THOST_FTDC_OF_Open;
    ///价格.c_str()
    req.LimitPrice = root["ReqArgs"]["LimitPrice"].asDouble();/// LIMIT_PRICE;
    ///数量: 1
    req.VolumeTotalOriginal = root["ReqArgs"]["VolumeTotalOriginal"].asInt();//1;

    ///组合投机套保标志
    req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
    ///投机 THOST_FTDC_HF_Speculation '1'
    ///套利 THOST_FTDC_HF_Arbitrage '2'
    ///套保 THOST_FTDC_HF_Hedge '3'
    ///做市商 THOST_FTDC_HF_MarketMaker '5'
    
    ///有效期类型: 当日有效
    req.TimeCondition = THOST_FTDC_TC_GFD;
    ///GTD日期
    //  TThostFtdcDateType  GTDDate;
    ///成交量类型: 任何数量
    req.VolumeCondition = THOST_FTDC_VC_AV;
    ///最小成交量: 1
    req.MinVolume = 1;
    ///触发条件: 立即
    req.ContingentCondition = THOST_FTDC_CC_Immediately;
    ///止损价
    // TThostFtdcPriceType StopPrice;
    ///强平原因: 非强平
    req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
    ///自动挂起标志: 否
    req.IsAutoSuspend = 0;
    ///业务单元
    //  TThostFtdcBusinessUnitType  BusinessUnit;
    ///请求编号
    //  TThostFtdcRequestIDType RequestID;
    ///用户强评标志: 否
    req.UserForceClose = 0;

	lOrderTime=time(NULL);
	int iResult = pUserApi->ReqOrderInsert(&req, ++iRequestID);
	cerr << "--->>> 报单录入请求: " << ((iResult == 0) ? "成功" : "失败") << endl;
}

void CTraderSpi::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspOrderInsert" << endl;
    Json::Value rspArgs;
	if(IsErrorRspInfo(pRspInfo)){
        rspArgs["ErrorMsg"] = pRspInfo->ErrorMsg;
        m_client->send(m_hdl, getJsonStr(nRequestID,"OnRspOrderInsert","true",rspArgs).c_str()
                , websocketpp::frame::opcode::text);
    }
}

void CTraderSpi::ReqOrderAction(CThostFtdcOrderField *pOrder)
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
    ///删除 THOST_FTDC_AF_Delete '0'  修改 THOST_FTDC_AF_Modify '3'
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

void CTraderSpi::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspOrderAction" << endl;
	IsErrorRspInfo(pRspInfo);
}


///报单通知
void CTraderSpi::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
	cerr << "--->>> " << "OnRtnOrder"  << endl;
	lOrderOkTime=time(NULL);
	time_t lTime=lOrderOkTime-lOrderTime;
	cerr << "--->>> 报单到报单通知的时间差 = " << lTime << endl;
	if (IsMyOrder(pOrder))
	{
        Json::Value rspArgs;
        rspArgs["RequestID"] = pOrder->RequestID;
        rspArgs["OrderLocalID"] = pOrder->OrderLocalID;
        rspArgs["ExchangeID"] = pOrder->ExchangeID;
        rspArgs["ParticipantID"] = pOrder->ParticipantID;
        rspArgs["ClientID"] =pOrder->ClientID;
        rspArgs["ExchangeInstID"] = pOrder->ExchangeInstID;
        rspArgs["TraderID"] = pOrder->TraderID;
        rspArgs["InstallID"] = pOrder->InstallID;
        rspArgs["OrderSubmitStatus"] = pOrder->OrderSubmitStatus;
        rspArgs["NotifySequence"] = pOrder->NotifySequence;
        rspArgs["TradingDay"] = pOrder->TradingDay;
        rspArgs["SettlementID"] = pOrder->SettlementID;
        rspArgs["OrderSysID"] = pOrder->OrderSysID;
        rspArgs["OrderSource"] = pOrder->OrderSource;
        rspArgs["OrderStatus"] = pOrder->OrderStatus;
        rspArgs["OrderType"] = pOrder->OrderType;
        rspArgs["VolumeTraded"] = pOrder->VolumeTraded;
        rspArgs["VolumeTotal"] = pOrder->VolumeTotal;
        rspArgs["InsertDate"] = pOrder->InsertDate;
        rspArgs["InsertTime"] = pOrder->InsertTime;
        rspArgs["ActiveTime"] = pOrder->ActiveTime;
        rspArgs["SuspendTime"] = pOrder->SuspendTime;
        rspArgs["UpdateTime"] = pOrder->UpdateTime;
        rspArgs["CancelTime"] = pOrder->CancelTime;
        rspArgs["ActiveTraderID"] = pOrder->ActiveTraderID;
        rspArgs["ClearingPartID"] = pOrder->ClearingPartID;
        rspArgs["SequenceNo"] = pOrder->SequenceNo;
        rspArgs["FrontID"] = pOrder->FrontID;
        rspArgs["SessionID"] = pOrder->SessionID;
        rspArgs["UserProductInfo"] = pOrder->UserProductInfo;
        rspArgs["StatusMsg"] = pOrder->StatusMsg;
        rspArgs["UserForceClose"] = pOrder->UserForceClose;
        rspArgs["ActiveUserID"] = pOrder->ActiveUserID;
        rspArgs["BrokerOrderSeq"] = pOrder->BrokerOrderSeq;
        rspArgs["RelativeOrderSysID"] = pOrder->RelativeOrderSysID;
        rspArgs["ZCETotalTradedVolume"] = pOrder->ZCETotalTradedVolume;
        rspArgs["IsSwapOrder"] = pOrder->IsSwapOrder;
        rspArgs["BranchID"] = pOrder->BranchID;
        rspArgs["InvestUnitID"] = pOrder->InvestUnitID;
        rspArgs["AccountID"] = pOrder->AccountID;
        rspArgs["CurrencyID"] = pOrder->CurrencyID;
        rspArgs["IPAddress"] = pOrder->IPAddress;
        rspArgs["MacAddress"] = pOrder->MacAddress;
        m_client->send(m_hdl, getJsonStr(pOrder->RequestID,"OnRtnOrder","false",rspArgs).c_str()
                , websocketpp::frame::opcode::text);

    }
	if (IsTradingOrder(pOrder))
	{
			//ReqOrderAction(pOrder);
	}
    else if (pOrder->OrderStatus == THOST_FTDC_OST_Canceled){
			cout << "--->>> 撤单成功" << endl;
	}
}

///成交通知
void CTraderSpi::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
	cerr << "--->>> " << "OnRtnTrade"  << endl;
    if(true){
        Json::Value rspArgs;
        rspArgs["BrokerID"] = pTrade->BrokerID;
        rspArgs["InvestorID"] = pTrade->InvestorID;
        rspArgs["InstrumentID"] = pTrade->InstrumentID;
        rspArgs["OrderRef"] = pTrade->OrderRef;
        rspArgs["UserID"] = pTrade->UserID;
        rspArgs["TradeID"] = pTrade->TradeID;
        rspArgs["Direction"] = pTrade->Direction;
        rspArgs["OrderSysID"] = pTrade->OrderSysID;
        rspArgs["ParticipantID"] = pTrade->ParticipantID;
        rspArgs["ClientID"] = pTrade->ClientID;
        rspArgs["TradingRole"] = pTrade->TradingRole;
        rspArgs["ExchangeInstID"] = pTrade->ExchangeInstID;
        rspArgs["OffsetFlag"] = pTrade->OffsetFlag;
        rspArgs["HedgeFlag"] = pTrade->HedgeFlag;
        rspArgs["Price"] = pTrade->Price;
        rspArgs["Volume"] = pTrade->Volume;
        rspArgs["TradeDate"] = pTrade->TradeDate;
        rspArgs["TradeTime"] = pTrade->TradeTime;
        rspArgs["TradeType"] = pTrade->TradeType;
        rspArgs["PriceSource"] = pTrade->PriceSource;
        rspArgs["TraderID"] = pTrade->TraderID;
        rspArgs["OrderLocalID"] = pTrade->OrderLocalID;
        rspArgs["ClearingPartID"] = pTrade->ClearingPartID;
        rspArgs["BusinessUnit"] = pTrade->BusinessUnit;
        rspArgs["SequenceNo"] = pTrade->SequenceNo;
        rspArgs["TradingDay"] = pTrade->TradingDay;
        rspArgs["SettlementID"] = pTrade->SettlementID;
        rspArgs["BrokerOrderSeq"] = pTrade->BrokerOrderSeq;
        rspArgs["TradeSource"] = pTrade->TradeSource;
        m_client->send(m_hdl, getJsonStr(0,"OnRtnTrade","false",rspArgs).c_str()
                , websocketpp::frame::opcode::text);
    }
}

void CTraderSpi:: OnFrontDisconnected(int nReason)
{
	cerr << "--->>> " << "OnFrontDisconnected" << endl;
	cerr << "--->>> Reason = " << nReason << endl;
}
		
void CTraderSpi::OnHeartBeatWarning(int nTimeLapse)
{
	cerr << "--->>> " << "OnHeartBeatWarning" << endl;
	cerr << "--->>> nTimerLapse = " << nTimeLapse << endl;
}

void CTraderSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspError" << endl;
	if(IsErrorRspInfo(pRspInfo)){
      Json::Value rspArgs;
      rspArgs["ErrorMsg"] = pRspInfo->ErrorMsg;
      m_client->send(m_hdl, getJsonStr(nRequestID,"OnRspError","true",rspArgs).c_str()
                , websocketpp::frame::opcode::text);
    }
}

bool CTraderSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	// 如果ErrorID != 0, 说明收到了错误的响应
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
		cerr << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
	return bResult;
}

bool CTraderSpi::IsMyOrder(CThostFtdcOrderField *pOrder)
{
	return ((pOrder->FrontID == FRONT_ID) &&
			(pOrder->SessionID == SESSION_ID) &&
			(strcmp(pOrder->OrderRef, ORDER_REF) == 0));
}

bool CTraderSpi::IsTradingOrder(CThostFtdcOrderField *pOrder)
{
	return ((pOrder->OrderStatus != THOST_FTDC_OST_PartTradedNotQueueing) &&
			(pOrder->OrderStatus != THOST_FTDC_OST_Canceled) &&
			(pOrder->OrderStatus != THOST_FTDC_OST_AllTraded));
}

