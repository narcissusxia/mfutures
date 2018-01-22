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

// USER_API����
extern CThostFtdcTraderApi* pUserApi;

// ���ò���
//extern char FRONT_ADDR[];		// ǰ�õ�ַ
//extern char BROKER_ID[];		// ���͹�˾����
extern char INVESTOR_ID[];		// Ͷ���ߴ���
//extern char PASSWORD[];			// �û�����
extern char INSTRUMENT_ID[];	// ��Լ����
extern TThostFtdcPriceType	LIMIT_PRICE;	// �۸�
extern TThostFtdcDirectionType	DIRECTION;	// ��������

// ������
int iRequestID=0;

// �Ự����
//TThostFtdcFrontIDType	FRONT_ID;	//ǰ�ñ��//
//TThostFtdcSessionIDType	SESSION_ID;	//�Ự���
//TThostFtdcOrderRefType	ORDER_REF;	//��������

//TThostFtdcBrokerIDType	 BROKER_ID = "9999";				// ���͹�˾����
//TThostFtdcInvestorIDType INVESTOR_ID = "110623";			// ע���������Լ���simnow����Ͷ���ߴ���
//TThostFtdcPasswordType   PASSWORD = "xialei1981";			// ע���������Լ���simnow�����û�����

time_t lOrderTime;
time_t lOrderOkTime;


void CTraderSpi::setWebsocket(client* c,websocketpp::connection_hdl hdl)
{  
	m_client = c; m_hdl =hdl;
}

void CTraderSpi::setUserLoginInfo( const char* BROKER_ID,const char* INVESTOR_ID,const char*  PASSWORD)
{  
	this->BROKER_ID = new char[strlen(BROKER_ID)+1];
    strcpy(this->BROKER_ID, BROKER_ID);
    this->INVESTOR_ID = new char[strlen(INVESTOR_ID)+1];
    strcpy(this->INVESTOR_ID, INVESTOR_ID);
    this->PASSWORD = new char[strlen(PASSWORD)+1];
    strcpy(this->PASSWORD, PASSWORD);
  
}

std::string CTraderSpi::getJsonStr(int uniqueID,std::string rspType,std::string isError,Json::Value rspArgs)
{   ///��ȡ��ǰ������
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
	ReqUserLogin();
	///�û���¼����
	
 
	
}

void CTraderSpi::ReqUserLogin()
{
	CThostFtdcReqUserLoginField req;
	//memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, this->BROKER_ID);
	strcpy(req.UserID, this->INVESTOR_ID);
	strcpy(req.Password, this->PASSWORD);
	int iResult = pUserApi->ReqUserLogin(&req, ++iRequestID);
	cerr << "--->>> �����û���¼����: " <<req.BrokerID<< ((iResult == 0) ? "�ɹ�" : "ʧ��") << endl;
}

void CTraderSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspUserLogin" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{

		// ����Ự����
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
		///��ȡ��ǰ������
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
        cerr << "--->>> ��ȡ��ǰ������ = " << pUserApi->GetTradingDay() << endl;
		//std::string SIP_msg="xialei1981\r\n";
        m_client->send(m_hdl, getJsonStr(nRequestID,"OnRspUserLogin","false",rspArgs).c_str()
        		, websocketpp::frame::opcode::text);
		///Ͷ���߽�����ȷ��,
		//ReqSettlementInfoConfirm();
	}
}

void CTraderSpi::ReqSettlementInfoConfirm()
{
	CThostFtdcSettlementInfoConfirmField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.InvestorID, INVESTOR_ID);
	int iResult = pUserApi->ReqSettlementInfoConfirm(&req, ++iRequestID);
	cerr << "--->>> Ͷ���߽�����ȷ��: " << ((iResult == 0) ? "�ɹ�" : "ʧ��") << endl;
}

void CTraderSpi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspSettlementInfoConfirm" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		///�����ѯ��Լ
		ReqQryInstrument();
	}
}

void CTraderSpi::ReqQryInstrument()
{
	CThostFtdcQryInstrumentField req;
	memset(&req, 0, sizeof(req));
	//add xialei strcpy(req.InstrumentID, INSTRUMENT_ID);
	int iResult = pUserApi->ReqQryInstrument(&req, ++iRequestID);
	cerr << "--->>> �����ѯ��Լ: " << ((iResult == 0) ? "�ɹ�" : "ʧ��") << endl;
}

void CTraderSpi::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspQryInstrument" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		///�����ѯ��Լ
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
	cerr << "--->>> �����ѯ�ʽ��˻�: " << ((iResult == 0) ? "�ɹ�" : "ʧ��") << endl;
}

void CTraderSpi::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspQryTradingAccount" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		///�����ѯͶ���ֲ߳�
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
	cerr << "--->>> �����ѯͶ���ֲ߳�: " << ((iResult == 0) ? "�ɹ�" : "ʧ��") << endl;
}

void CTraderSpi::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspQryInvestorPosition" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		///����¼������
		ReqOrderInsert();
	}
}

void CTraderSpi::ReqOrderInsert()
{
	CThostFtdcInputOrderField req;
	memset(&req, 0, sizeof(req));
	///���͹�˾����
	strcpy(req.BrokerID, BROKER_ID);
	///Ͷ���ߴ���
	strcpy(req.InvestorID, INVESTOR_ID);
	///��Լ����
	// add xialei strcpy(req.InstrumentID, INSTRUMENT_ID);
	///��������
	strcpy(req.OrderRef, ORDER_REF);
	///�û�����
//	TThostFtdcUserIDType	UserID;
	///�����۸�����: �޼�
	req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
	///��������: 
	// add xialei req.Direction = DIRECTION;
	///��Ͽ�ƽ��־: ����
	req.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
	///���Ͷ���ױ���־
	req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	///�۸�
	// add xialei req.LimitPrice = LIMIT_PRICE;
	///����: 1
	req.VolumeTotalOriginal = 1;
	///��Ч������: ������Ч
	req.TimeCondition = THOST_FTDC_TC_GFD;
	///GTD����
//	TThostFtdcDateType	GTDDate;
	///�ɽ�������: �κ�����
	req.VolumeCondition = THOST_FTDC_VC_AV;
	///��С�ɽ���: 1
	req.MinVolume = 1;
	///��������: ����
	req.ContingentCondition = THOST_FTDC_CC_Immediately;
	///ֹ���
//	TThostFtdcPriceType	StopPrice;
	///ǿƽԭ��: ��ǿƽ
	req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	///�Զ������־: ��
	req.IsAutoSuspend = 0;
	///ҵ��Ԫ
//	TThostFtdcBusinessUnitType	BusinessUnit;
	///������
//	TThostFtdcRequestIDType	RequestID;
	///�û�ǿ����־: ��
	req.UserForceClose = 0;

	lOrderTime=time(NULL);
	int iResult = pUserApi->ReqOrderInsert(&req, ++iRequestID);
	cerr << "--->>> ����¼������: " << ((iResult == 0) ? "�ɹ�" : "ʧ��") << endl;
}

void CTraderSpi::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspOrderInsert" << endl;
	IsErrorRspInfo(pRspInfo);
}

void CTraderSpi::ReqOrderAction(CThostFtdcOrderField *pOrder)
{
	static bool ORDER_ACTION_SENT = false;		//�Ƿ����˱���
	if (ORDER_ACTION_SENT)
		return;

	CThostFtdcInputOrderActionField req;
	memset(&req, 0, sizeof(req));
	///���͹�˾����
	strcpy(req.BrokerID, pOrder->BrokerID);
	///Ͷ���ߴ���
	strcpy(req.InvestorID, pOrder->InvestorID);
	///������������
//	TThostFtdcOrderActionRefType	OrderActionRef;
	///��������
	strcpy(req.OrderRef, pOrder->OrderRef);
	///������
//	TThostFtdcRequestIDType	RequestID;
	///ǰ�ñ��
	req.FrontID = FRONT_ID;
	///�Ự���
	req.SessionID = SESSION_ID;
	///����������
//	TThostFtdcExchangeIDType	ExchangeID;
	///�������
//	TThostFtdcOrderSysIDType	OrderSysID;
	///������־
	req.ActionFlag = THOST_FTDC_AF_Delete;
	///�۸�
//	TThostFtdcPriceType	LimitPrice;
	///�����仯
//	TThostFtdcVolumeType	VolumeChange;
	///�û�����
//	TThostFtdcUserIDType	UserID;
	///��Լ����
	strcpy(req.InstrumentID, pOrder->InstrumentID);
	lOrderTime=time(NULL);
	int iResult = pUserApi->ReqOrderAction(&req, ++iRequestID);
	cerr << "--->>> ������������: " << ((iResult == 0) ? "�ɹ�" : "ʧ��") << endl;
	ORDER_ACTION_SENT = true;
}

void CTraderSpi::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspOrderAction" << endl;
	IsErrorRspInfo(pRspInfo);
}

///����֪ͨ
void CTraderSpi::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
	cerr << "--->>> " << "OnRtnOrder"  << endl;
	lOrderOkTime=time(NULL);
	time_t lTime=lOrderOkTime-lOrderTime;
	cerr << "--->>> ����������֪ͨ��ʱ��� = " << lTime << endl;
	if (IsMyOrder(pOrder))
	{
		if (IsTradingOrder(pOrder))
		{
			//ReqOrderAction(pOrder);
		}
		else if (pOrder->OrderStatus == THOST_FTDC_OST_Canceled)
			cout << "--->>> �����ɹ�" << endl;
	}
}

///�ɽ�֪ͨ
void CTraderSpi::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
	cerr << "--->>> " << "OnRtnTrade"  << endl;
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
	IsErrorRspInfo(pRspInfo);
}

bool CTraderSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	// ���ErrorID != 0, ˵���յ��˴������Ӧ
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

