
#include <iostream>
using namespace std;

#include <json/json.h>
#pragma warning(disable : 4996)

#include  "MdSpi.h"



// 请求编号


std::string CMdSpi::getJsonStr(int uniqueID,std::string rspType,std::string isError,Json::Value rspArgs)
{   ///获取当前交易日
    Json::Value root;
    root["RspArgs"]  =rspArgs;
    root["UniqueID"] =uniqueID;
    root["RspType"]  =rspType;
    root["IsError"]  =isError;
    
 std::string out;//UTF8String 
 try  
    { 
    //cerr << "--->>> " <<rspArgs["InstrumentName"]<< "OnFrontConnected="<<root.toStyledString() << endl; 
    //printf();
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

void CMdSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast)
{
	cerr << "--->>> OnRspError" << endl;
	IsErrorRspInfo(pRspInfo);
	printf ("OnRspError:\n"); 
    printf ("ErrorCode=[%d],     ErrorMsg=[%s]\n",     pRspInfo->ErrorID, 
    pRspInfo->ErrorMsg); 
    printf ("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast); 
}

void CMdSpi::OnFrontDisconnected(int nReason)
{
	cerr << "--->>> OnFrontDisconnected"  << endl;
	cerr << "--->>> OnFrontDisconnected Reason = " << nReason << endl;
}
		
void CMdSpi::OnHeartBeatWarning(int nTimeLapse)
{
	cerr << "--->>> OnHeartBeatWarning"   << endl;
	cerr << "--->>> nTimerLapse = " << nTimeLapse << endl;
}

void CMdSpi::OnFrontConnected()
{
	  CThostFtdcReqUserLoginField req;
      strcpy(req.BrokerID, BROKER_ID);
	  strcpy(req.UserID, INVESTOR_ID);
	  strcpy(req.Password, PASSWORD);
      printf ("OnFrontConnected.\n");
      m_pUserApi->ReqUserLogin (&req, ++iRequestID); 
}

void CMdSpi::MdReqUserLogout()
{
	CThostFtdcUserLogoutField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.UserID, INVESTOR_ID);
	//strcpy(req.Password, PASSWORD);
	cerr <<m_pUserApi<<endl;
	int iResult = m_pUserApi->ReqUserLogout(&req, ++iRequestID);
	cerr << "--->>> 发送用户登出请求: " << ((iResult == 0) ? "成功" : "失败") << endl;
}

void CMdSpi::MdReqUserLogin(Json::Value root)
{
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.UserID, INVESTOR_ID);
	strcpy(req.Password, PASSWORD);
	int iResult = m_pUserApi->ReqUserLogin(&req, ++iRequestID);
	cerr << "--->>> 发送用户登录请求: " << ((iResult == 0) ? "成功" : "失败") << endl;
}

void CMdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{

    cerr << "--->>> OnRspUserLogin"   << endl;
	Json::Value rspArgs;
	rspArgs["TradingDay"] = pRspUserLogin->TradingDay;
	rspArgs["LoginTime"] = pRspUserLogin->LoginTime;
	rspArgs["BrokerID"] = pRspUserLogin->BrokerID;
	rspArgs["UserID"] = pRspUserLogin->UserID;
	rspArgs["SystemName"] = pRspUserLogin->SystemName;
	rspArgs["FrontID"] = pRspUserLogin->FrontID;
	rspArgs["SessionID"] = pRspUserLogin->SessionID;
	rspArgs["MaxOrderRef"] = pRspUserLogin->MaxOrderRef;
	rspArgs["INETime"] = pRspUserLogin->INETime;
	rspArgs["INETime"] = pRspUserLogin->INETime;
	rspArgs["SHFETime"] = pRspUserLogin->SHFETime;
	rspArgs["DCETime"] = pRspUserLogin->DCETime;
	rspArgs["CZCETime"] = pRspUserLogin->CZCETime;
	rspArgs["FFEXTime"] = pRspUserLogin->FFEXTime;
	rspArgs["INETime"] = pRspUserLogin->INETime;
	//cerr << "--->>> 获取当前交易日 = " << m_pUserApi->GetTradingDay() << endl;
    
	m_client->send(m_hdl, getJsonStr(nRequestID,"MdOnRspUserLogin","false",rspArgs).c_str()
		, websocketpp::frame::opcode::text);
	
}

void CMdSpi::MdSubscribeMarketData(Json::Value root)
{
	
	char  **Instrumnet = new char*[1]; 
	Instrumnet[0]=const_cast<char *>(root["ReqArgs"]["InstrumentID"].asString().c_str());
	int iResult = m_pUserApi->SubscribeMarketData(Instrumnet, 1);
	cerr << "--->>> 发送行情订阅请求: " << ((iResult == 0) ? "成功" : "失败") << endl;
}

void CMdSpi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "OnRspSubMarketData" << endl;
}

void CMdSpi::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr <<"OnRspUnSubMarketData"  << endl;
}

double filterDouble(double a){
	if (a > 10000000000000.0 )
		return -1.0;
}

void CMdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	/**
	cerr << "OnRtnDepthMarketData" << endl;
	printf ("E输出报单录入结果\n");     
	printf("交易日：【%s】\n",pDepthMarketData->TradingDay);
	printf("合约代码：【%s】\n",pDepthMarketData->InstrumentID);  
	printf("交易所代码：【%s】\n",pDepthMarketData->ExchangeID);  
	printf("合约在交易所的代码：【%s】\n",pDepthMarketData->ExchangeInstID);   
	printf("最新价：【%f】\n",pDepthMarketData->LastPrice );  
	printf("上次结算价：【%f】\n",pDepthMarketData->PreSettlementPrice );
	printf("昨收盘：【%f】\n",pDepthMarketData->PreClosePrice);  
	printf("昨持仓量：【%f】\n",pDepthMarketData->PreOpenInterest);  
	printf("今开盘：【%f】\n",pDepthMarketData->OpenPrice ); 
	printf("最高价：【%f】\n",pDepthMarketData->HighestPrice );
	printf("最低价：【%f】\n",pDepthMarketData->LowestPrice );
	printf("数量：【%d】\n",pDepthMarketData->Volume );
	printf("成交金额：【%f】\n",pDepthMarketData->Turnover ); 
	printf("持仓量：【%f】\n",pDepthMarketData->OpenInterest );  
	printf("今收盘：【%f】\n",filterDouble(pDepthMarketData->ClosePrice) );  
	printf("本次结算价：【%f】\n",filterDouble(pDepthMarketData->SettlementPrice)); 
	printf("涨停板价：【%f】\n",filterDouble(pDepthMarketData->UpperLimitPrice) ); 
	printf("跌停板价：【%f】\n",filterDouble(pDepthMarketData->LowerLimitPrice )); 
	printf("昨虚实度：【%f】\n",filterDouble(pDepthMarketData->PreDelta) ); 
	printf("今虚实度：【%f】\n",filterDouble(pDepthMarketData->CurrDelta )); 
	printf("最后修改时间：【%s】\n",pDepthMarketData->UpdateTime ); 
	printf("最后修改毫秒：【%d】\n",pDepthMarketData->UpdateMillisec ); 
	printf("x1：【%f】\n",filterDouble(pDepthMarketData->AskPrice1 )); 
	printf("x2：【%d】\n",pDepthMarketData->AskVolume1 ); 
	printf("x3：【%f】\n",filterDouble(pDepthMarketData->BidPrice2) ); 
	printf("x4：【%d】\n",pDepthMarketData->BidVolume2 ); 
*/

	Json::Value rspArgs;
	rspArgs["TradingDay"]=pDepthMarketData->TradingDay;
	rspArgs["InstrumentID"]=pDepthMarketData->InstrumentID;
	rspArgs["ExchangeID"]=pDepthMarketData->ExchangeID;
	rspArgs["ExchangeInstID"]=pDepthMarketData->ExchangeInstID;
	rspArgs["LastPrice"]=pDepthMarketData->LastPrice;
	rspArgs["PreSettlementPrice"]=pDepthMarketData->PreSettlementPrice;
	rspArgs["PreClosePrice"]=pDepthMarketData->PreClosePrice;
	rspArgs["PreOpenInterest"]=pDepthMarketData->PreOpenInterest;
	rspArgs["OpenPrice"]=pDepthMarketData->OpenPrice;
	rspArgs["HighestPrice"]=pDepthMarketData->HighestPrice;
	rspArgs["LowestPrice"]=pDepthMarketData->LowestPrice;
	rspArgs["Volume"]=pDepthMarketData->Volume;
	rspArgs["Turnover"]=pDepthMarketData->Turnover;
	rspArgs["OpenInterest"]=pDepthMarketData->OpenInterest;
	rspArgs["ClosePrice"]=filterDouble(pDepthMarketData->ClosePrice);
	rspArgs["SettlementPrice"]=filterDouble(pDepthMarketData->SettlementPrice);
	rspArgs["UpperLimitPrice"]=filterDouble(pDepthMarketData->UpperLimitPrice);
	rspArgs["LowerLimitPrice"]=filterDouble(pDepthMarketData->LowerLimitPrice);
	rspArgs["PreDelta"]=filterDouble(pDepthMarketData->PreDelta);
	rspArgs["CurrDelta"]=filterDouble(pDepthMarketData->CurrDelta);
	rspArgs["UpdateTime"]=pDepthMarketData->UpdateTime;
	rspArgs["UpdateMillisec"]=pDepthMarketData->UpdateMillisec;
	rspArgs["BidPrice1"]=filterDouble(pDepthMarketData->BidPrice1);
	rspArgs["BidVolume1"]=pDepthMarketData->BidVolume1;
	rspArgs["AskPrice1"]=filterDouble(pDepthMarketData->AskPrice1);
	rspArgs["AskVolume1"]=pDepthMarketData->AskVolume1;
	rspArgs["BidPrice2"]=filterDouble(pDepthMarketData->BidPrice2);
	rspArgs["BidVolume2"]=pDepthMarketData->BidVolume2;
	rspArgs["AskPrice2"]=filterDouble(pDepthMarketData->AskPrice2);
	rspArgs["AskVolume2"]=pDepthMarketData->AskVolume2;
	rspArgs["BidPrice3"]=filterDouble(pDepthMarketData->BidPrice3);
	rspArgs["BidVolume3"]=pDepthMarketData->BidVolume3;
	rspArgs["AskPrice3"]=filterDouble(pDepthMarketData->AskPrice3);
	rspArgs["AskVolume3"]=pDepthMarketData->AskVolume3;
	rspArgs["BidPrice4"]=filterDouble(pDepthMarketData->BidPrice4);
	rspArgs["BidVolume4"]=pDepthMarketData->BidVolume4;
	rspArgs["AskPrice4"]=filterDouble(pDepthMarketData->AskPrice4);
	rspArgs["AskVolume4"]=pDepthMarketData->AskVolume4;
	rspArgs["BidPrice5"]=filterDouble(pDepthMarketData->BidPrice5);
	rspArgs["BidVolume5"]=pDepthMarketData->BidVolume5;
	rspArgs["AskPrice5"]=filterDouble(pDepthMarketData->AskPrice5);
	rspArgs["AskVolume5"]=pDepthMarketData->AskVolume5;
	rspArgs["AveragePrice"]=filterDouble(pDepthMarketData->AveragePrice);
	rspArgs["ActionDay"]=pDepthMarketData->ActionDay;
	//cerr << "--->>> 获取当前交易日 = " << pUserApi->GetTradingDay() << endl;
    
	m_client->send(m_hdl, getJsonStr(0,"OnRtnDepthMarketData","false",rspArgs).c_str()
		, websocketpp::frame::opcode::text);

}

bool CMdSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	// 如果ErrorID != 0, 说明收到了错误的响应
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
		cerr << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
	return bResult;
}

void CMdSpi::setWebsocket(client* c,websocketpp::connection_hdl hdl)
{  
 m_client = c; m_hdl =hdl;
}

void CMdSpi::setUserLoginInfo(int loginID,const char* BROKER_ID,const char* INVESTOR_ID,const char*  PASSWORD)
{  

    this->BROKER_ID = new char[strlen(BROKER_ID)+1];
    strcpy(this->BROKER_ID, BROKER_ID);
    this->INVESTOR_ID = new char[strlen(INVESTOR_ID)+1];
    strcpy(this->INVESTOR_ID, INVESTOR_ID);
    this->PASSWORD = new char[strlen(PASSWORD)+1];
    strcpy(this->PASSWORD, PASSWORD);
  
}