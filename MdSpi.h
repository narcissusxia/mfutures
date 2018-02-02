
#ifndef _FILENAME_CMdSpi
#define _FILENAME_CMdSpi

#include "../TraderAPI/ThostFtdcMdApi.h" 
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
typedef websocketpp::client<websocketpp::config::asio_client> client;


class CMdSpi : public CThostFtdcMdSpi 
{
	public:
	
    CMdSpi (CThostFtdcMdApi *pUserApi) : m_pUserApi (pUserApi) {} 
    //CMdSpi(CThostFtdcTraderApi* pCMdSpi) : m_pUserApi (pUserApi){};
	~CMdSpi(){}
	///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
	virtual void OnFrontConnected();

	virtual void OnFrontDisconnected (int nReason) ;

	///登录请求响应
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///投资者结算结果确认响应
	//virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	///请求查询合约响应
	//virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    
    virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument,CThostFtdcRspInfoField *pRspInfo,int nRequestID, bool bIsLast) ;
    
    virtual void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData) ;

    virtual void OnRspError (CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    virtual bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);
    virtual void OnHeartBeatWarning(int nTimeLapse);
    ///用户登录请求
	virtual void MdReqUserLogin(Json::Value root);
	///用户登出请求
	virtual void MdReqUserLogout();

    ///订阅请求
    virtual void MdSubscribeMarketData(Json::Value root);
	//virtual void MdSubMarketData(Json::Value root);

	virtual std::string getJsonStr(int uniqueID,std::string rspType,std::string isError,Json::Value rspArgs);

	virtual void setWebsocket(client* c,websocketpp::connection_hdl hdl);

    virtual void setUserLoginInfo(int loginID,const char* BROKER_ID,const char* INVESTOR_ID,const char*  PASSWORD);

	private: 
    char*	BROKER_ID ;	// 经纪公司代码
    char*   INVESTOR_ID ;// 注意输入你自己的simnow仿真投资者代码
	char*   PASSWORD ;// 注意输入你自己的simnow仿真用户密码

	client* m_client;

	websocketpp::connection_hdl m_hdl;

    // 指向CThostFtdcMdApi实例的指针 
    CThostFtdcMdApi *m_pUserApi;

	int loginID;
    int iRequestID;
};
 
#endif
