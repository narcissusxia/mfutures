#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <errno.h>
#include <iostream>
#include <iconv.h>

#include <sstream>

#include <json/json.h>

using namespace std;

#include "Boost_Tools.h" 
#include "../TraderAPI/ThostFtdcTraderApi.h"

#include  "TraderSpi.h"
#include <time.h>


#pragma warning(disable : 4996)

// USER_API参数
//extern CThostFtdcTraderApi* pUserApi;

// 配置参数
//extern char FRONT_ADDR[];  // 前置地址
//extern char BROKER_ID[];  // 经纪公司代码
//extern char INVESTOR_ID[];  // 投资者代码
//extern char PASSWORD[];   // 用户密码
//extern char INSTRUMENT_ID[]; // 合约代码
//extern TThostFtdcPriceType LIMIT_PRICE; // 价格
//extern TThostFtdcDirectionType DIRECTION; // 买卖方向

// 请求编号
int iRequestID=0;
using namespace boost::locale::conv;  
// 会话参数
//TThostFtdcFrontIDType FRONT_ID; //前置编号//
//TThostFtdcSessionIDType SESSION_ID; //会话编号
//TThostFtdcOrderRefType ORDER_REF; //报单引用

//TThostFtdcBrokerIDType  BROKER_ID = "9999";    // 经纪公司代码
//TThostFtdcInvestorIDType INVESTOR_ID = "110623";   // 注意输入你自己的simnow仿真投资者代码
//TThostFtdcPasswordType   PASSWORD = "xialei1981";   // 注意输入你自己的simnow仿真用户密码

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

void CTraderSpi::OnFrontConnected()
{
 cerr << "--->>> " << "OnFrontConnected" << endl;

 
 strcpy(loginStatus,"N");
 ///用户登录请求
 ReqUserLogin(loginID);
 
}

void CTraderSpi::onRspConnect(Json::Value root)
{
  Json::Value rspArgs;
  
  rspArgs["loginStatus"] = loginStatus;
  m_client->send(m_hdl, getJsonStr(0,"onRspConnect","false",rspArgs).c_str()
    , websocketpp::frame::opcode::text);
 ///用户登录请求
}




void CTraderSpi::ReqUserLogin(int loginID)
{
 CThostFtdcReqUserLoginField req;
 //memset(&req, 0, sizeof(req));
 strcpy(req.BrokerID, this->BROKER_ID);
 strcpy(req.UserID, this->INVESTOR_ID);
 strcpy(req.Password, this->PASSWORD);
 int iResult = t_pUserApi->ReqUserLogin(&req, loginID);
 cerr << "--->>> 发送用户登录请求: " <<req.BrokerID<< ((iResult == 0) ? "成功" : "失败") << endl;
}

void CTraderSpi::ReqUserLogout()
{
 CThostFtdcUserLogoutField req;
 //memset(&req, 0, sizeof(req));
 strcpy(req.BrokerID, this->BROKER_ID);
 strcpy(req.UserID, this->INVESTOR_ID);

 int iResult = t_pUserApi->ReqUserLogout(&req, ++iRequestID);
 //strcpy(loginStatus,"Y");
 cerr << "--->>> 发送用户退出请求: " <<req.BrokerID<< ((iResult == 0) ? "成功" : "失败") << endl;
}

void CTraderSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
  CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{


 cerr << "--->>> " << "OnRspUserLogin" << endl;
 if (bIsLast && !IsErrorRspInfo(pRspInfo))
 {
  cerr << "--->>> " << "sss" << endl;
  strcpy(loginStatus,"Y");
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
  rspArgs["INETime"] = pRspUserLogin->INETime;
  rspArgs["INETime"] = pRspUserLogin->INETime;
  rspArgs["SHFETime"] = pRspUserLogin->SHFETime;
  rspArgs["DCETime"] = pRspUserLogin->DCETime;
  rspArgs["CZCETime"] = pRspUserLogin->CZCETime;
  rspArgs["FFEXTime"] = pRspUserLogin->FFEXTime;
  rspArgs["INETime"] = pRspUserLogin->INETime;
  //err << "--->>> 获取当前交易日 = " << pUserApi->GetTradingDay() << endl;
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
 int iResult = t_pUserApi->ReqSettlementInfoConfirm(&req, root["UniqueID"].asInt());
 cerr << "--->>> 投资者结算结果确认: " << ((iResult == 0) ? "成功" : "失败") << endl;
}

void CTraderSpi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
 cerr << "--->>> " << "OnRspSettlementInfoConfirm" << endl;
 if (bIsLast && !IsErrorRspInfo(pRspInfo))
 {
        Json::Value rspArgs;
        rspArgs["BrokerID"] = pSettlementInfoConfirm->BrokerID;
        rspArgs["InvestorID"] = pSettlementInfoConfirm->InvestorID;
        rspArgs["ConfirmDate"] = pSettlementInfoConfirm->ConfirmDate;
        rspArgs["ConfirmTime"] = pSettlementInfoConfirm->ConfirmTime;
        
        //cerr << "--->>> 获取当前交易日 = " << pUserApi->GetTradingDay() << endl;
        //std::string SIP_msg="xialei1981\r\n";
        m_client->send(m_hdl, getJsonStr(nRequestID,"OnRspSettlementInfoConfirm","false",rspArgs).c_str()
                , websocketpp::frame::opcode::text);
  ///请求查询合约
  //ReqQryInstrument();
 }
}

void CTraderSpi::ReqQryInstrument(Json::Value root)
{
 CThostFtdcQryInstrumentField req;
 memset(&req, 0, sizeof(req));
 //add xialei strcpy(req.InstrumentID, INSTRUMENT_ID);
 int iResult = t_pUserApi->ReqQryInstrument(&req, root["UniqueID"].asInt());
 cerr << "--->>> 请求查询合约: " << ((iResult == 0) ? "成功" : "失败") << endl;
}


//中文需要先转utf-8
//
std::string boosttoolsnamespace::CBoostTools::gbktoutf8(std::string const &text)  
{  
    std::string const &to_encoding("UTF-8");  
    std::string const &from_encoding("GBK");  
    method_type how = default_method;  
    return boost::locale::conv::between(text.c_str(), text.c_str() + text.size(), to_encoding, from_encoding, how);  
}  

void CTraderSpi::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{

  Json::Value rspArgs;
  rspArgs["InstrumentID"]=pInstrument->InstrumentID;
  rspArgs["ExchangeID"]=pInstrument->ExchangeID;
  rspArgs["InstrumentName"]=boosttoolsnamespace::CBoostTools::gbktoutf8(pInstrument->InstrumentName);//out;;
  rspArgs["ExchangeInstID"]=pInstrument->ExchangeInstID;
  rspArgs["ProductID"]=pInstrument->ProductID;
  rspArgs["ProductClass"]=pInstrument->ProductClass;
  rspArgs["DeliveryYear"]=pInstrument->DeliveryYear;
  rspArgs["DeliveryMonth"]=pInstrument->DeliveryMonth;
  rspArgs["MaxMarketOrderVolume"]=pInstrument->MaxMarketOrderVolume;
  rspArgs["MinMarketOrderVolume"]=pInstrument->MinMarketOrderVolume;
  rspArgs["MaxLimitOrderVolume"]=pInstrument->MaxLimitOrderVolume;
  rspArgs["MinLimitOrderVolume"]=pInstrument->MinLimitOrderVolume;
  rspArgs["VolumeMultiple"]=pInstrument->VolumeMultiple;
  rspArgs["PriceTick"]=pInstrument->PriceTick;
  rspArgs["CreateDate"]=pInstrument->CreateDate;
  rspArgs["OpenDate"]=pInstrument->OpenDate;
  rspArgs["ExpireDate"]=pInstrument->ExpireDate;
  rspArgs["StartDelivDate"]=pInstrument->StartDelivDate;
  rspArgs["EndDelivDate"]=pInstrument->EndDelivDate;
  rspArgs["InstLifePhase"]=pInstrument->InstLifePhase;
  rspArgs["IsTrading"]=pInstrument->IsTrading;
  rspArgs["PositionType"]=pInstrument->PositionType;
  rspArgs["PositionDateType"]=pInstrument->PositionDateType;
  rspArgs["LongMarginRatio"]=pInstrument->LongMarginRatio;
  rspArgs["ShortMarginRatio"]=pInstrument->ShortMarginRatio;
  rspArgs["MaxMarginSideAlgorithm"]=pInstrument->MaxMarginSideAlgorithm;
  rspArgs["UnderlyingInstrID"]=pInstrument->UnderlyingInstrID;
  rspArgs["StrikePrice"]=pInstrument->StrikePrice;
  rspArgs["OptionsType"]=pInstrument->OptionsType;
  rspArgs["UnderlyingMultiple"]=pInstrument->UnderlyingMultiple;
  rspArgs["CombinationType"]=pInstrument->CombinationType;
  m_client->send(m_hdl, getJsonStr(nRequestID,"OnRspQryInstrument","false",rspArgs).c_str()
    , websocketpp::frame::opcode::text);
  /**
  cerr << "--->>> 获取当前交易品种 = " << pInstrument->InstrumentID              << endl;
  cerr << "--->>> 获取当前交易品种 = " << pInstrument->ExchangeID                 << endl;
  cerr << "--->>> 获取当前交易品种 = " << pInstrument->InstrumentName            << endl;
  cerr << "--->>> 获取当前交易品种 = " << pInstrument->ExchangeInstID            << endl;
  cerr << "--->>> 获取当前交易品种 = " << pInstrument->ProductID                  << endl;
  cerr << "--->>> 获取当前交易品种 = " << pInstrument->ProductClass              << endl;
  cerr << "--->>> 获取当前交易品种 = " << pInstrument->DeliveryYear              << endl;
  cerr << "--->>> 获取当前交易品种 = " << pInstrument->DeliveryMonth             << endl;
  cerr << "--->>> 获取当前交易品种 = " << pInstrument->MaxMarketOrderVolume        << endl;
  cerr << "--->>> 获取当前交易品种 = " << pInstrument->MinMarketOrderVolume        << endl;
  cerr << "--->>> 获取当前交易品种 = " << pInstrument->MaxLimitOrderVolume         << endl;
  cerr << "--->>> 获取当前交易品种 = " << pInstrument->MinLimitOrderVolume         << endl;
  cerr << "--->>> 获取当前交易品种 = " << pInstrument->VolumeMultiple              << endl;
  cerr << "--->>> 获取当前交易品种 = " << pInstrument->PriceTick                   << endl;
  cerr << "--->>> 获取当前交易品种 = " << pInstrument->CreateDate                  << endl;
  cerr << "--->>> 获取当前交易品种 = " << pInstrument->OpenDate                    << endl;
  cerr << "--->>> 获取当前交易品种 = " << pInstrument->ExpireDate                  << endl;
  cerr << "--->>> 获取当前交易品种 = " << pInstrument->StartDelivDate              << endl;
  cerr << "--->>> 获取当前交易品种 = " << pInstrument->EndDelivDate                << endl;
  cerr << "--->>> 获取当前交易品种 = " << pInstrument->InstLifePhase               << endl;
  cerr << "--->>> 获取当前交易品种 = " << pInstrument->IsTrading                   << endl;
  cerr << "--->>> 获取当前交易品种 = " << pInstrument->PositionType                << endl;
  cerr << "--->>> 获取当前交易品种 = " << pInstrument->PositionDateType            << endl;
  cerr << "--->>> 获取当前交易品种 = " << pInstrument->LongMarginRatio             << endl;
  cerr << "--->>> 获取当前交易品种 = " << pInstrument->ShortMarginRatio            << endl;
  cerr << "--->>> 获取当前交易品种 = " << pInstrument->MaxMarginSideAlgorithm      << endl;
  cerr << "--->>> 获取当前交易品种 = " << pInstrument->UnderlyingInstrID           << endl;
  cerr << "--->>> 获取当前交易品种 = " << pInstrument->StrikePrice                 << endl;
  cerr << "--->>> 获取当前交易品种 = " << pInstrument->OptionsType                 << endl;
  cerr << "--->>> 获取当前交易品种 = " << pInstrument->UnderlyingMultiple          << endl;
  cerr << "--->>> 获取当前交易品种 = " << pInstrument->CombinationType             << endl;
 */
 //if (bIsLast && !IsErrorRspInfo(pRspInfo))
 //{
  ///请求查询合约
   //ReqQryTradingAccount();
      //cerr << "--->>> " << "OnRspQryInstrument" << pInstrument->InstrumentID << endl;    //
 //}
}

void CTraderSpi::ReqQryTradingAccount(Json::Value root)
{
 CThostFtdcQryTradingAccountField req;
 memset(&req, 0, sizeof(req));
 strcpy(req.BrokerID, BROKER_ID);
 strcpy(req.InvestorID, INVESTOR_ID);
 int iResult = t_pUserApi->ReqQryTradingAccount(&req,root["UniqueID"].asInt());
 cerr << "--->>> 请求查询资金账户: " << ((iResult == 0) ? "成功" : "失败") << endl;
}

void CTraderSpi::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
 cerr << "--->>> " << "OnRspQryTradingAccount" << endl;
 Json::Value rspArgs;
 if (!IsErrorRspInfo(pRspInfo))
 {
  
    rspArgs["BrokerID"]=pTradingAccount->BrokerID;
    rspArgs["AccountID"]=pTradingAccount->AccountID;
    rspArgs["PreMortgage"]=pTradingAccount->PreMortgage;
    rspArgs["PreCredit"]=pTradingAccount->PreCredit;
    rspArgs["PreDeposit"]=pTradingAccount->PreDeposit;
    rspArgs["PreBalance"]=pTradingAccount->PreBalance;
    rspArgs["PreMargin"]=pTradingAccount->PreMargin;
    rspArgs["InterestBase"]=pTradingAccount->InterestBase;
    rspArgs["Interest"]=pTradingAccount->Interest;
    rspArgs["Deposit"]=pTradingAccount->Deposit;
    rspArgs["Withdraw"]=pTradingAccount->Withdraw;
    rspArgs["FrozenMargin"]=pTradingAccount->FrozenMargin;
    rspArgs["FrozenCash"]=pTradingAccount->FrozenCash;
    rspArgs["FrozenCommission"]=pTradingAccount->FrozenCommission;
    rspArgs["CurrMargin"]=pTradingAccount->CurrMargin;
    rspArgs["CashIn"]=pTradingAccount->CashIn;
    rspArgs["Commission"]=pTradingAccount->Commission;
    rspArgs["CloseProfit"]=pTradingAccount->CloseProfit;
    rspArgs["PositionProfit"]=pTradingAccount->PositionProfit;
    rspArgs["Balance"]=pTradingAccount->Balance;
    rspArgs["Available"]=pTradingAccount->Available;
    rspArgs["WithdrawQuota"]=pTradingAccount->WithdrawQuota;
    rspArgs["Reserve"]=pTradingAccount->Reserve;
    rspArgs["TradingDay"]=pTradingAccount->TradingDay;
    rspArgs["SettlementID"]=pTradingAccount->SettlementID;
    rspArgs["Credit"]=pTradingAccount->Credit;
    rspArgs["Mortgage"]=pTradingAccount->Mortgage;
    rspArgs["ExchangeMargin"]=pTradingAccount->ExchangeMargin;
    rspArgs["DeliveryMargin"]=pTradingAccount->DeliveryMargin;
    rspArgs["ExchangeDeliveryMargin"]=pTradingAccount->ExchangeDeliveryMargin;
    rspArgs["ReserveBalance"]=pTradingAccount->ReserveBalance;
    rspArgs["CurrencyID"]=pTradingAccount->CurrencyID;
    rspArgs["PreFundMortgageIn"]=pTradingAccount->PreFundMortgageIn;
    rspArgs["PreFundMortgageOut"]=pTradingAccount->PreFundMortgageOut;
    rspArgs["FundMortgageAvailable"]=pTradingAccount->FundMortgageAvailable;
    rspArgs["MortgageableFund"]=pTradingAccount->MortgageableFund;
    rspArgs["SpecProductMargin"]=pTradingAccount->SpecProductMargin;
    rspArgs["SpecProductCommission"]=pTradingAccount->SpecProductCommission;
    rspArgs["SpecProductFrozenCommission"]=pTradingAccount->SpecProductFrozenCommission;
    rspArgs["SpecProductPositionProfit"]=pTradingAccount->SpecProductPositionProfit;
    rspArgs["SpecProductCloseProfit"]=pTradingAccount->SpecProductCloseProfit;
    rspArgs["SpecProductPositionProfitByAlg"]=pTradingAccount->SpecProductPositionProfitByAlg;
    rspArgs["SpecProductExchangeMargin"]=pTradingAccount->SpecProductExchangeMargin;

    m_client->send(m_hdl, getJsonStr(nRequestID,"OnRspQryTradingAccount","true",rspArgs).c_str()
                , websocketpp::frame::opcode::text);

 }
}

void CTraderSpi::ReqQryInvestorPosition(Json::Value root)
{
 cerr << "--->>> 请求查询投资者持仓: "  << endl;
 CThostFtdcQryInvestorPositionField req;
 memset(&req, 0, sizeof(req));
 strcpy(req.BrokerID, BROKER_ID);
 strcpy(req.InvestorID, INVESTOR_ID);
 cerr << "--->>> 请求查询投资者持仓: " <<this->BROKER_ID<<this->INVESTOR_ID << endl;
 //add xialei strcpy(req.InstrumentID, INSTRUMENT_ID);
 int iResult = t_pUserApi->ReqQryInvestorPosition(&req, root["UniqueID"].asInt());
 cerr << "--->>> 请求查询投资者持仓: " << ((iResult == 0) ? "成功" : "失败") << endl;
}

void CTraderSpi::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
 cerr << "--->>> " << "OnRspQryInvestorPosition:" << endl;
 Json::Value rspArgs;
 //if (bIsLast && !IsErrorRspInfo(pRspInfo))
 //{
 if(pInvestorPosition == NULL)
  return;
 cerr << "--->>> " << "OnRspQryInvestorPosition11:" << endl;
 //if( !IsErrorRspInfo(pRspInfo)){
       cerr << "--->>> " << "OnRspQryInvestorPosition12:" << endl;
       cerr << pInvestorPosition<<endl;
    rspArgs["InstrumentID"]=pInvestorPosition->InstrumentID;
    rspArgs["BrokerID"]=pInvestorPosition->BrokerID;
    rspArgs["InvestorID"]=pInvestorPosition->InvestorID;
    rspArgs["PosiDirection"]=pInvestorPosition->PosiDirection;
    rspArgs["HedgeFlag"]=pInvestorPosition->HedgeFlag;
    rspArgs["PositionDate"]=pInvestorPosition->PositionDate;
    rspArgs["YdPosition"]=pInvestorPosition->YdPosition;
    rspArgs["Position"]=pInvestorPosition->Position;
    rspArgs["LongFrozen"]=pInvestorPosition->LongFrozen;
    rspArgs["ShortFrozen"]=pInvestorPosition->ShortFrozen;
    rspArgs["LongFrozenAmount"]=pInvestorPosition->LongFrozenAmount;
    rspArgs["ShortFrozenAmount"]=pInvestorPosition->ShortFrozenAmount;
    rspArgs["OpenVolume"]=pInvestorPosition->OpenVolume;
    rspArgs["CloseVolume"]=pInvestorPosition->CloseVolume;
    rspArgs["OpenAmount"]=pInvestorPosition->OpenAmount;
    rspArgs["CloseAmount"]=pInvestorPosition->CloseAmount;
    rspArgs["PositionCost"]=pInvestorPosition->PositionCost;
    rspArgs["PreMargin"]=pInvestorPosition->PreMargin;
    rspArgs["UseMargin"]=pInvestorPosition->UseMargin;
    rspArgs["FrozenMargin"]=pInvestorPosition->FrozenMargin;
    rspArgs["FrozenCash"]=pInvestorPosition->FrozenCash;
    rspArgs["FrozenCommission"]=pInvestorPosition->FrozenCommission;
    rspArgs["CashIn"]=pInvestorPosition->CashIn;
    rspArgs["Commission"]=pInvestorPosition->Commission;
    rspArgs["CloseProfit"]=pInvestorPosition->CloseProfit;
    rspArgs["PositionProfit"]=pInvestorPosition->PositionProfit;
    rspArgs["PreSettlementPrice"]=pInvestorPosition->PreSettlementPrice;
    rspArgs["SettlementPrice"]=pInvestorPosition->SettlementPrice;
    rspArgs["TradingDay"]=pInvestorPosition->TradingDay;
    rspArgs["SettlementID"]=pInvestorPosition->SettlementID;
    rspArgs["OpenCost"]=pInvestorPosition->OpenCost;
    rspArgs["ExchangeMargin"]=pInvestorPosition->ExchangeMargin;
    rspArgs["CombPosition"]=pInvestorPosition->CombPosition;
    rspArgs["CombLongFrozen"]=pInvestorPosition->CombLongFrozen;
    rspArgs["CombShortFrozen"]=pInvestorPosition->CombShortFrozen;
    rspArgs["CloseProfitByDate"]=pInvestorPosition->CloseProfitByDate;
    rspArgs["CloseProfitByTrade"]=pInvestorPosition->CloseProfitByTrade;
    rspArgs["TodayPosition"]=pInvestorPosition->TodayPosition;
    rspArgs["MarginRateByMoney"]=pInvestorPosition->MarginRateByMoney;
    rspArgs["MarginRateByVolume"]=pInvestorPosition->MarginRateByVolume;
    rspArgs["StrikeFrozen"]=pInvestorPosition->StrikeFrozen;
    rspArgs["StrikeFrozenAmount"]=pInvestorPosition->StrikeFrozenAmount;
    rspArgs["AbandonFrozen"]=pInvestorPosition->AbandonFrozen;
    
    //rspArgs["ErrorMsg"] = pRspInfo->ErrorMsg;
    cerr << "--->>> " << "begin" << endl;
    m_client->send(m_hdl, getJsonStr(nRequestID,"OnRspQryInvestorPosition","true",rspArgs).c_str()
                , websocketpp::frame::opcode::text);
    cerr << "--->>> " << "end" << endl;

 //}
}

void CTraderSpi::ReqOrderInsert(Json::Value root)
{
  CThostFtdcInputOrderField req;
    memset(&req, 0, sizeof(req));
    ///经纪公司代码
    strcpy(req.BrokerID, BROKER_ID);
    ///投资者代码
    strcpy(req.InvestorID, INVESTOR_ID);
    strcpy(req.OrderRef, root["ReqArgs"]["OrderRef"].asString().c_str());
    ///合约代码
    strcpy(req.InstrumentID, root["ReqArgs"]["InstrumentID"].asString().c_str());
    //strcpy(req.InstrumentID, "ag1801");
    ///报单引用
    //strcpy(req.OrderRef, "123");
    ///用户代码
  //  TThostFtdcUserIDType  UserID;
    ///报单价格条件: 限价
    req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
    ///买卖方向: 
    req.Direction = root["ReqArgs"]["Direction"].asString().c_str()[0];
    ///组合开平标志: 开仓
    req.CombOffsetFlag[0] =  root["ReqArgs"]["CombOffsetFlag"].asString().c_str()[0];
    ///组合投机套保标志
    req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
    ///价格
    req.LimitPrice = root["ReqArgs"]["LimitPrice"].asDouble();//LIMIT_PRICE;
    ///数量: 1
    req.VolumeTotalOriginal =  root["ReqArgs"]["VolumeTotalOriginal"].asInt();//1;
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
  //  TThostFtdcPriceType StopPrice;
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
    int iResult = t_pUserApi->ReqOrderInsert(&req,root["UniqueID"].asInt());
    cerr << "--->>> 报单录入请求: "<<root["UniqueID"].asInt()<<";xx=" << ((iResult == 0) ? "成功" : "失败") << endl;
  }

void CTraderSpi::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
 cerr << "--->>> " << "OnRspOrderInsert" << endl;
 Json::Value rspArgs;
 if(IsErrorRspInfo(pRspInfo)){
        rspArgs["ErrorMsg"] = boosttoolsnamespace::CBoostTools::gbktoutf8(pRspInfo->ErrorMsg);
        m_client->send(m_hdl, getJsonStr(nRequestID,"OnRspOrderInsert","true",rspArgs).c_str()
                , websocketpp::frame::opcode::text);
 }
}

void CTraderSpi::ReqOrderAction(CThostFtdcOrderField *pOrder)
{
 static bool ORDER_ACTION_SENT = false;  //是否发送了报单
 if (ORDER_ACTION_SENT)
  return;

 CThostFtdcInputOrderActionField req;
 memset(&req, 0, sizeof(req));
 ///经纪公司代码
 strcpy(req.BrokerID, pOrder->BrokerID);
 ///投资者代码
 strcpy(req.InvestorID, pOrder->InvestorID);
 ///报单操作引用
// TThostFtdcOrderActionRefType OrderActionRef;
 ///报单引用
 strcpy(req.OrderRef, pOrder->OrderRef);
 ///请求编号
// TThostFtdcRequestIDType RequestID;
 ///前置编号
 req.FrontID = FRONT_ID;
 ///会话编号
 req.SessionID = SESSION_ID;
 ///交易所代码
// TThostFtdcExchangeIDType ExchangeID;
 ///报单编号
// TThostFtdcOrderSysIDType OrderSysID;
 ///操作标志
 req.ActionFlag = THOST_FTDC_AF_Delete;
    ///删除 THOST_FTDC_AF_Delete '0'  修改 THOST_FTDC_AF_Modify '3'
 ///价格
// TThostFtdcPriceType LimitPrice;
 ///数量变化
// TThostFtdcVolumeType VolumeChange;
 ///用户代码
// TThostFtdcUserIDType UserID;
 ///合约代码
 strcpy(req.InstrumentID, pOrder->InstrumentID);
 lOrderTime=time(NULL);
 int iResult = t_pUserApi->ReqOrderAction(&req, ++iRequestID);
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
 cerr << "--->>> " << "OnRtnOrder="<<pOrder->RequestID << endl;
 cerr << "--->>> 报单到报单通知的时间差 = "<<pOrder->StatusMsg  << endl;
 //if (IsMyOrder(pOrder))
 //{
        Json::Value rspArgs;
        
        rspArgs["OrderRef"] = pOrder->OrderRef;
        rspArgs["RequestID"] = pOrder->RequestID;
        rspArgs["OrderLocalID"] = pOrder->OrderLocalID;
        rspArgs["ExchangeID"] = pOrder->ExchangeID;
        rspArgs["ParticipantID"] = pOrder->ParticipantID;
        rspArgs["ClientID"] =pOrder->ClientID;
        rspArgs["ExchangeInstID"] = pOrder->ExchangeInstID;
        //rspArgs["TradeID"] = pOrder->TradeID;
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
        rspArgs["StatusMsg"] = boosttoolsnamespace::CBoostTools::gbktoutf8(pOrder->StatusMsg);
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

    //}
 //if (IsTradingOrder(pOrder))
 //{
   //ReqOrderAction(pOrder);
 //}
  //  else if (pOrder->OrderStatus == THOST_FTDC_OST_Canceled){
  // cout << "--->>> 撤单成功" << endl;
// }
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
        rspArgs["ExchangeID"] = pTrade->ExchangeID;
        
        std::cout << "on_message called with hdl: " << m_hdl.lock().get()
              //<< " and message: " << msg->get_payload()
              << std::endl;
        m_client->send(m_hdl, getJsonStr(0,"OnRtnTrade","false",rspArgs).c_str()
                , websocketpp::frame::opcode::text);
    }
}

void CTraderSpi::OnFrontDisconnected(int nReason)
{
 cerr << "--->>> " << "OnFrontDisconnected" << endl;
 cerr << "--->>> Reason = " << nReason << endl;
 Json::Value rspArgs;
 rspArgs["InvestorID"] =INVESTOR_ID;
 rspArgs["Reason"] = nReason;
 m_client->send(m_hdl, getJsonStr(0,"OnFrontDisconnected","false",rspArgs).c_str()
                , websocketpp::frame::opcode::text);

 strcpy(loginStatus,"N");
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
      rspArgs["ErrorMsg"] = boosttoolsnamespace::CBoostTools::gbktoutf8(pRspInfo->ErrorMsg);
      m_client->send(m_hdl, getJsonStr(nRequestID,"OnRspError","true",rspArgs).c_str()
                , websocketpp::frame::opcode::text);
 }
}

bool CTraderSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
 // 如果ErrorID != 0, 说明收到了错误的响应
 bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
 if (bResult)
  cerr << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" 
    <<  boosttoolsnamespace::CBoostTools::gbktoutf8(pRspInfo->ErrorMsg) << endl;
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



