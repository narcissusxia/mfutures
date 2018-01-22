// testTraderApi.cpp : �������̨Ӧ�ó������ڵ㡣
//
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <errno.h>
#include <iostream>

using namespace std;
#include "../TraderAPI/ThostFtdcTraderApi.h"
#include "TraderSpi.h"

// �߳̿������
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

// ��¼����ṹ��
CThostFtdcReqUserLoginField userLoginField;
// �û�����ṹ��
CThostFtdcUserLogoutField userLogoutField;

// UserApi����
CThostFtdcTraderApi* pUserApi;

char  FRONT_ADDR[] = "tcp://180.168.146.187:10030";		// ǰ�õ�ַ
/**
TThostFtdcBrokerIDType	BROKER_ID = "9999";				// ���͹�˾����
TThostFtdcInvestorIDType INVESTOR_ID = "110623";			// ע���������Լ���simnow����Ͷ���ߴ���
TThostFtdcPasswordType  PASSWORD = "xialei1981";			// ע���������Լ���simnow�����û�����
*/

TThostFtdcInstrumentIDType INSTRUMENT_ID = "ag1801";	// ��Լ���� ��ע����ʱ����ı��ԼID,����ʹ�ù�ʱ��Լ
TThostFtdcDirectionType	DIRECTION = THOST_FTDC_D_Sell;	// ��������
TThostFtdcPriceType	LIMIT_PRICE = 2380;				// �۸�

// ������
//int iRequestID = 0;
// �߳�ͬ����־
sem_t sem;

/**
// �Ự����
TThostFtdcFrontIDType	FRONT_ID;	//ǰ�ñ��
TThostFtdcSessionIDType	SESSION_ID;	//�Ự���
TThostFtdcOrderRefType	ORDER_REF;	//��������
time_t lOrderTime;
time_t lOrderOkTime;



//export LD_LIBRARY_PATH=/sboot/TraderAPI:$LD_LIBRARY_PATH 
class CTraderSpi : public CThostFtdcTraderSpi
{
public:
	CTraderSpi (CThostFtdcTraderApi *pUserApi) : m_pUserApi (pUserApi) {printf("CTraderSpi:called.\n");} 

	~CTraderSpi(){}
    
	
	///���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�
	virtual void OnFrontConnected(){
		printf("OnFrontConnected:called.\n");
		static int i = 0;
		// �ڵǳ���ϵͳ�����µ���OnFrontConnected��������жϲ����Ե�1��֮������е��á�
		if (i++==0) {
        	printf("OnFrontConnected:called.\n");
			//sem_post(&sem);
		}
	    //�û���¼����
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
        cerr << "--->>> �����û���¼����: "<<iRequestID << ((iResult == 0) ? "�ɹ�" : "ʧ��") << endl;
	}

	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
    {
		cerr << "--->>> " << "OnRspUserLogin:"<<pRspInfo->ErrorID<<pRspUserLogin->FrontID<<","<<pRspUserLogin->SessionID<<","<<pRspUserLogin->SHFETime<< endl;
		if(pRspInfo->ErrorID != 0){
		   //��¼ʧ�ܣ��ͻ�����Ҫ���д�����
		   printf("Failed to login, errorcode = %d errormsg= %s requestid = %d chain = %d", pRspInfo->ErrorID, pRspInfo->ErrorMsg, nRequestID, bIsLast);
		   exit(-1);
		}

		if (bIsLast && !IsErrorRspInfo(pRspInfo))
		{
		// ����Ự����
		FRONT_ID = pRspUserLogin->FrontID;
		SESSION_ID = pRspUserLogin->SessionID;
		int iNextOrderRef = atoi(pRspUserLogin->MaxOrderRef);
		cerr << "--->>> " << "FRONT_ID:"<<FRONT_ID<<"," <<SESSION_ID<<iNextOrderRef<<","<<bIsLast<< endl;
		iNextOrderRef++;
		sprintf(ORDER_REF, "%d", iNextOrderRef);
		///��ȡ��ǰ������
		cerr << "--->>> ��ȡ��ǰ������ = " << pUserApi->GetTradingDay() << endl;
		///Ͷ���߽�����ȷ��
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
		cerr << "--->>> Ͷ���߽�����ȷ��: " << ((iResult == 0) ? "�ɹ�" : "ʧ��") << endl;
	}

	virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		cerr << "--->>> " << "OnRspSettlementInfoConfirm" << endl;
		if (bIsLast && !IsErrorRspInfo(pRspInfo))
		{
			///�����ѯ��Լ
			ReqQryInstrument();
		}
	}
    ///�����ѯ��Լ
	virtual void ReqQryInstrument()
	{
		CThostFtdcQryInstrumentField req;
		memset(&req, 0, sizeof(req));
		strcpy(req.InstrumentID, INSTRUMENT_ID);
		int iResult = m_pUserApi->ReqQryInstrument(&req, ++iRequestID);
		cerr << "--->>> �����ѯ��Լ: " << ((iResult == 0) ? "�ɹ�" : "ʧ��") << endl;
	}
    ///�����ѯ��Լ��Ӧ
	virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		cerr << "--->>> " << "OnRspQryInstrument" <<nRequestID<< endl;
		if (bIsLast && !IsErrorRspInfo(pRspInfo))
		{
			//cerr << "--->>> " << "DeliveryYear:" <<pInstrument->DeliveryYear<< endl;
			
			///�����ѯ��Լ
			ReqQryTradingAccount();
		}
	}
    ///�����ѯ�ʽ��˻�
	virtual void ReqQryTradingAccount()
	{
		CThostFtdcQryTradingAccountField req;
		memset(&req, 0, sizeof(req));
		strcpy(req.BrokerID, BROKER_ID);
		strcpy(req.InvestorID, INVESTOR_ID);
		//strcpy(req.CurrencyID, "RMB");
		sleep(1);
		int iResult = m_pUserApi->ReqQryTradingAccount(&req, ++iRequestID);
		cerr << "--->>> �����ѯ�ʽ��˻�: " << ((iResult == 0) ? "�ɹ�" : "ʧ��")<<iResult << endl;
	}
	///�����ѯ�ʽ��˻���Ӧ
	virtual void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		cerr << "--->>> " << "OnRspQryTradingAccount:"<<pTradingAccount->PreDeposit << endl;
		
		 
		if (bIsLast && !IsErrorRspInfo(pRspInfo))
		{
			///�����ѯͶ���ֲ߳�
			sleep(1);
			ReqQryInvestorPosition();
		}
	}
    ///�����ѯͶ���ֲ߳�
	virtual void ReqQryInvestorPosition()
	{
		CThostFtdcQryInvestorPositionField req;
		memset(&req, 0, sizeof(req));
		strcpy(req.BrokerID, BROKER_ID);
		strcpy(req.InvestorID, INVESTOR_ID);
		strcpy(req.InstrumentID, INSTRUMENT_ID);
		int iResult = pUserApi->ReqQryInvestorPosition(&req, ++iRequestID);
		cerr << "--->>> �����ѯͶ���ֲ߳�: " << ((iResult == 0) ? "�ɹ�" : "ʧ��") << endl;
	}
    ///�����ѯͶ���ֲ߳���Ӧ
	virtual void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		cerr << "--->>> " << "OnRspQryInvestorPosition" << endl;
		if (bIsLast && !IsErrorRspInfo(pRspInfo))
		{
			///����¼������
			ReqOrderInsert();
		}
	}
    ///����¼������
	virtual void ReqOrderInsert()
	{
		CThostFtdcInputOrderField req;
		memset(&req, 0, sizeof(req));
		///���͹�˾����
		strcpy(req.BrokerID, BROKER_ID);
		///Ͷ���ߴ���
		strcpy(req.InvestorID, INVESTOR_ID);
		///��Լ����
		strcpy(req.InstrumentID, INSTRUMENT_ID);
		///��������
		strcpy(req.OrderRef, ORDER_REF);
		///�û�����
	//	TThostFtdcUserIDType	UserID;
		///�����۸�����: �޼�
		req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
		///��������: 
		req.Direction = DIRECTION;
		///��Ͽ�ƽ��־: ����
		req.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
		///���Ͷ���ױ���־
		req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
		///�۸�
		req.LimitPrice = LIMIT_PRICE;
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

	void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		cerr << "--->>> " << "OnRspOrderInsert" << endl;
		IsErrorRspInfo(pRspInfo);
	}

	void ReqOrderAction(CThostFtdcOrderField *pOrder)
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

	void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		cerr << "--->>> " << "OnRspOrderAction" << endl;
		IsErrorRspInfo(pRspInfo);
	}

	///����֪ͨ
	void OnRtnOrder(CThostFtdcOrderField *pOrder)
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
		// ���ErrorID != 0, ˵���յ��˴������Ӧ
		bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
		if (bResult)
			cerr << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
		return bResult;
	}


	// ָ��CThostFtdcMdApiʵ����ָ�� 
    CThostFtdcTraderApi *m_pUserApi ; 


};


int main()
{

    // ��ʼ���߳�ͬ������
	sem_init(&sem,0,0);
    printf("userid:\n");
    // �ȴ���¼�ɹ���Ϣ
	//sem_wait(&sem);

	// ��ʼ��UserApi
	pUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi();			// ����UserApi
    CTraderSpi* sh= new CTraderSpi();
    pUserApi->RegisterSpi ((CThostFtdcTraderSpi*)sh);   
	//CTraderSpi* pUserSpi = new CTraderSpi();

	//pUserApi->RegisterSpi((CThostFtdcTraderSpi*)pUserSpi);			// ע���¼���
	//pUserApi->SubscribePublicTopic(TERT_RESTART);					// ע�ṫ����
	//pUserApi->SubscribePrivateTopic(TERT_RESTART);					// ע��˽����
	printf("usereed:\n");
	pUserApi->RegisterFront("tcp://180.168.146.187:10000");	
	pUserApi->Init();
	// �ȴ�������������¼��Ϣ
	sem_wait(&sem);
 
 
	printf("usereed:\n");
	//pUserApi->Init();
	printf("usereed:\n");

	pUserApi->Join();
	//pUserApi->Release();
        
    return 0;
}
