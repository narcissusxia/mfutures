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

// USER_API����
//extern CThostFtdcTraderApi* pUserApi;

// ���ò���
//extern char FRONT_ADDR[];  // ǰ�õ�ַ
//extern char BROKER_ID[];  // ���͹�˾����
//extern char INVESTOR_ID[];  // Ͷ���ߴ���
//extern char PASSWORD[];   // �û�����
//extern char INSTRUMENT_ID[]; // ��Լ����
//extern TThostFtdcPriceType LIMIT_PRICE; // �۸�
//extern TThostFtdcDirectionType DIRECTION; // ��������

// ������
int iRequestID=0;
using namespace boost::locale::conv;  
// �Ự����
//TThostFtdcFrontIDType FRONT_ID; //ǰ�ñ��//
//TThostFtdcSessionIDType SESSION_ID; //�Ự���
//TThostFtdcOrderRefType ORDER_REF; //��������

//TThostFtdcBrokerIDType  BROKER_ID = "9999";    // ���͹�˾����
//TThostFtdcInvestorIDType INVESTOR_ID = "110623";   // ע���������Լ���simnow����Ͷ���ߴ���
//TThostFtdcPasswordType   PASSWORD = "xialei1981";   // ע���������Լ���simnow�����û�����

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
{   ///��ȡ��ǰ������
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
 ///�û���¼����
 ReqUserLogin(loginID);
 
}

void CTraderSpi::onRspConnect(Json::Value root)
{
  Json::Value rspArgs;
  
  rspArgs["loginStatus"] = loginStatus;
  m_client->send(m_hdl, getJsonStr(0,"onRspConnect","false",rspArgs).c_str()
    , websocketpp::frame::opcode::text);
 ///�û���¼����
}




void CTraderSpi::ReqUserLogin(int loginID)
{
 CThostFtdcReqUserLoginField req;
 //memset(&req, 0, sizeof(req));
 strcpy(req.BrokerID, this->BROKER_ID);
 strcpy(req.UserID, this->INVESTOR_ID);
 strcpy(req.Password, this->PASSWORD);
 int iResult = t_pUserApi->ReqUserLogin(&req, loginID);
 cerr << "--->>> �����û���¼����: " <<req.BrokerID<< ((iResult == 0) ? "�ɹ�" : "ʧ��") << endl;
}

void CTraderSpi::ReqUserLogout()
{
 CThostFtdcUserLogoutField req;
 //memset(&req, 0, sizeof(req));
 strcpy(req.BrokerID, this->BROKER_ID);
 strcpy(req.UserID, this->INVESTOR_ID);

 int iResult = t_pUserApi->ReqUserLogout(&req, ++iRequestID);
 //strcpy(loginStatus,"Y");
 cerr << "--->>> �����û��˳�����: " <<req.BrokerID<< ((iResult == 0) ? "�ɹ�" : "ʧ��") << endl;
}

void CTraderSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
  CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{


 cerr << "--->>> " << "OnRspUserLogin" << endl;
 if (bIsLast && !IsErrorRspInfo(pRspInfo))
 {
  cerr << "--->>> " << "sss" << endl;
  strcpy(loginStatus,"Y");
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
  rspArgs["INETime"] = pRspUserLogin->INETime;
  rspArgs["INETime"] = pRspUserLogin->INETime;
  rspArgs["SHFETime"] = pRspUserLogin->SHFETime;
  rspArgs["DCETime"] = pRspUserLogin->DCETime;
  rspArgs["CZCETime"] = pRspUserLogin->CZCETime;
  rspArgs["FFEXTime"] = pRspUserLogin->FFEXTime;
  rspArgs["INETime"] = pRspUserLogin->INETime;
  //err << "--->>> ��ȡ��ǰ������ = " << pUserApi->GetTradingDay() << endl;
  //std::string SIP_msg="xialei1981\r\n";
  m_client->send(m_hdl, getJsonStr(nRequestID,"OnRspUserLogin","false",rspArgs).c_str()
    , websocketpp::frame::opcode::text);
  ///Ͷ���߽�����ȷ��,
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
 cerr << "--->>> Ͷ���߽�����ȷ��: " << ((iResult == 0) ? "�ɹ�" : "ʧ��") << endl;
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
        
        //cerr << "--->>> ��ȡ��ǰ������ = " << pUserApi->GetTradingDay() << endl;
        //std::string SIP_msg="xialei1981\r\n";
        m_client->send(m_hdl, getJsonStr(nRequestID,"OnRspSettlementInfoConfirm","false",rspArgs).c_str()
                , websocketpp::frame::opcode::text);
  ///�����ѯ��Լ
  //ReqQryInstrument();
 }
}

void CTraderSpi::ReqQryInstrument(Json::Value root)
{
 CThostFtdcQryInstrumentField req;
 memset(&req, 0, sizeof(req));
 //add xialei strcpy(req.InstrumentID, INSTRUMENT_ID);
 int iResult = t_pUserApi->ReqQryInstrument(&req, root["UniqueID"].asInt());
 cerr << "--->>> �����ѯ��Լ: " << ((iResult == 0) ? "�ɹ�" : "ʧ��") << endl;
}


//������Ҫ��תutf-8
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
  cerr << "--->>> ��ȡ��ǰ����Ʒ�� = " << pInstrument->InstrumentID              << endl;
  cerr << "--->>> ��ȡ��ǰ����Ʒ�� = " << pInstrument->ExchangeID                 << endl;
  cerr << "--->>> ��ȡ��ǰ����Ʒ�� = " << pInstrument->InstrumentName            << endl;
  cerr << "--->>> ��ȡ��ǰ����Ʒ�� = " << pInstrument->ExchangeInstID            << endl;
  cerr << "--->>> ��ȡ��ǰ����Ʒ�� = " << pInstrument->ProductID                  << endl;
  cerr << "--->>> ��ȡ��ǰ����Ʒ�� = " << pInstrument->ProductClass              << endl;
  cerr << "--->>> ��ȡ��ǰ����Ʒ�� = " << pInstrument->DeliveryYear              << endl;
  cerr << "--->>> ��ȡ��ǰ����Ʒ�� = " << pInstrument->DeliveryMonth             << endl;
  cerr << "--->>> ��ȡ��ǰ����Ʒ�� = " << pInstrument->MaxMarketOrderVolume        << endl;
  cerr << "--->>> ��ȡ��ǰ����Ʒ�� = " << pInstrument->MinMarketOrderVolume        << endl;
  cerr << "--->>> ��ȡ��ǰ����Ʒ�� = " << pInstrument->MaxLimitOrderVolume         << endl;
  cerr << "--->>> ��ȡ��ǰ����Ʒ�� = " << pInstrument->MinLimitOrderVolume         << endl;
  cerr << "--->>> ��ȡ��ǰ����Ʒ�� = " << pInstrument->VolumeMultiple              << endl;
  cerr << "--->>> ��ȡ��ǰ����Ʒ�� = " << pInstrument->PriceTick                   << endl;
  cerr << "--->>> ��ȡ��ǰ����Ʒ�� = " << pInstrument->CreateDate                  << endl;
  cerr << "--->>> ��ȡ��ǰ����Ʒ�� = " << pInstrument->OpenDate                    << endl;
  cerr << "--->>> ��ȡ��ǰ����Ʒ�� = " << pInstrument->ExpireDate                  << endl;
  cerr << "--->>> ��ȡ��ǰ����Ʒ�� = " << pInstrument->StartDelivDate              << endl;
  cerr << "--->>> ��ȡ��ǰ����Ʒ�� = " << pInstrument->EndDelivDate                << endl;
  cerr << "--->>> ��ȡ��ǰ����Ʒ�� = " << pInstrument->InstLifePhase               << endl;
  cerr << "--->>> ��ȡ��ǰ����Ʒ�� = " << pInstrument->IsTrading                   << endl;
  cerr << "--->>> ��ȡ��ǰ����Ʒ�� = " << pInstrument->PositionType                << endl;
  cerr << "--->>> ��ȡ��ǰ����Ʒ�� = " << pInstrument->PositionDateType            << endl;
  cerr << "--->>> ��ȡ��ǰ����Ʒ�� = " << pInstrument->LongMarginRatio             << endl;
  cerr << "--->>> ��ȡ��ǰ����Ʒ�� = " << pInstrument->ShortMarginRatio            << endl;
  cerr << "--->>> ��ȡ��ǰ����Ʒ�� = " << pInstrument->MaxMarginSideAlgorithm      << endl;
  cerr << "--->>> ��ȡ��ǰ����Ʒ�� = " << pInstrument->UnderlyingInstrID           << endl;
  cerr << "--->>> ��ȡ��ǰ����Ʒ�� = " << pInstrument->StrikePrice                 << endl;
  cerr << "--->>> ��ȡ��ǰ����Ʒ�� = " << pInstrument->OptionsType                 << endl;
  cerr << "--->>> ��ȡ��ǰ����Ʒ�� = " << pInstrument->UnderlyingMultiple          << endl;
  cerr << "--->>> ��ȡ��ǰ����Ʒ�� = " << pInstrument->CombinationType             << endl;
 */
 //if (bIsLast && !IsErrorRspInfo(pRspInfo))
 //{
  ///�����ѯ��Լ
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
 cerr << "--->>> �����ѯ�ʽ��˻�: " << ((iResult == 0) ? "�ɹ�" : "ʧ��") << endl;
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
 cerr << "--->>> �����ѯͶ���ֲ߳�: "  << endl;
 CThostFtdcQryInvestorPositionField req;
 memset(&req, 0, sizeof(req));
 strcpy(req.BrokerID, BROKER_ID);
 strcpy(req.InvestorID, INVESTOR_ID);
 cerr << "--->>> �����ѯͶ���ֲ߳�: " <<this->BROKER_ID<<this->INVESTOR_ID << endl;
 //add xialei strcpy(req.InstrumentID, INSTRUMENT_ID);
 int iResult = t_pUserApi->ReqQryInvestorPosition(&req, root["UniqueID"].asInt());
 cerr << "--->>> �����ѯͶ���ֲ߳�: " << ((iResult == 0) ? "�ɹ�" : "ʧ��") << endl;
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
    ///���͹�˾����
    strcpy(req.BrokerID, BROKER_ID);
    ///Ͷ���ߴ���
    strcpy(req.InvestorID, INVESTOR_ID);
    strcpy(req.OrderRef, root["ReqArgs"]["OrderRef"].asString().c_str());
    ///��Լ����
    strcpy(req.InstrumentID, root["ReqArgs"]["InstrumentID"].asString().c_str());
    //strcpy(req.InstrumentID, "ag1801");
    ///��������
    //strcpy(req.OrderRef, "123");
    ///�û�����
  //  TThostFtdcUserIDType  UserID;
    ///�����۸�����: �޼�
    req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
    ///��������: 
    req.Direction = root["ReqArgs"]["Direction"].asString().c_str()[0];
    ///��Ͽ�ƽ��־: ����
    req.CombOffsetFlag[0] =  root["ReqArgs"]["CombOffsetFlag"].asString().c_str()[0];
    ///���Ͷ���ױ���־
    req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
    ///�۸�
    req.LimitPrice = root["ReqArgs"]["LimitPrice"].asDouble();//LIMIT_PRICE;
    ///����: 1
    req.VolumeTotalOriginal =  root["ReqArgs"]["VolumeTotalOriginal"].asInt();//1;
    ///��Ч������: ������Ч
    req.TimeCondition = THOST_FTDC_TC_GFD;
    ///GTD����
  //  TThostFtdcDateType  GTDDate;
    ///�ɽ�������: �κ�����
    req.VolumeCondition = THOST_FTDC_VC_AV;
    ///��С�ɽ���: 1
    req.MinVolume = 1;
    ///��������: ����
    req.ContingentCondition = THOST_FTDC_CC_Immediately;
    ///ֹ���
  //  TThostFtdcPriceType StopPrice;
    ///ǿƽԭ��: ��ǿƽ
    req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
    ///�Զ������־: ��
    req.IsAutoSuspend = 0;
    ///ҵ��Ԫ
  //  TThostFtdcBusinessUnitType  BusinessUnit;
    ///������
  //  TThostFtdcRequestIDType RequestID;
    ///�û�ǿ����־: ��
    req.UserForceClose = 0;

    lOrderTime=time(NULL);
    int iResult = t_pUserApi->ReqOrderInsert(&req,root["UniqueID"].asInt());
    cerr << "--->>> ����¼������: "<<root["UniqueID"].asInt()<<";xx=" << ((iResult == 0) ? "�ɹ�" : "ʧ��") << endl;
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
 static bool ORDER_ACTION_SENT = false;  //�Ƿ����˱���
 if (ORDER_ACTION_SENT)
  return;

 CThostFtdcInputOrderActionField req;
 memset(&req, 0, sizeof(req));
 ///���͹�˾����
 strcpy(req.BrokerID, pOrder->BrokerID);
 ///Ͷ���ߴ���
 strcpy(req.InvestorID, pOrder->InvestorID);
 ///������������
// TThostFtdcOrderActionRefType OrderActionRef;
 ///��������
 strcpy(req.OrderRef, pOrder->OrderRef);
 ///������
// TThostFtdcRequestIDType RequestID;
 ///ǰ�ñ��
 req.FrontID = FRONT_ID;
 ///�Ự���
 req.SessionID = SESSION_ID;
 ///����������
// TThostFtdcExchangeIDType ExchangeID;
 ///�������
// TThostFtdcOrderSysIDType OrderSysID;
 ///������־
 req.ActionFlag = THOST_FTDC_AF_Delete;
    ///ɾ�� THOST_FTDC_AF_Delete '0'  �޸� THOST_FTDC_AF_Modify '3'
 ///�۸�
// TThostFtdcPriceType LimitPrice;
 ///�����仯
// TThostFtdcVolumeType VolumeChange;
 ///�û�����
// TThostFtdcUserIDType UserID;
 ///��Լ����
 strcpy(req.InstrumentID, pOrder->InstrumentID);
 lOrderTime=time(NULL);
 int iResult = t_pUserApi->ReqOrderAction(&req, ++iRequestID);
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
 cerr << "--->>> " << "OnRtnOrder="<<pOrder->RequestID << endl;
 cerr << "--->>> ����������֪ͨ��ʱ��� = "<<pOrder->StatusMsg  << endl;
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
  // cout << "--->>> �����ɹ�" << endl;
// }
}

///�ɽ�֪ͨ
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
 // ���ErrorID != 0, ˵���յ��˴������Ӧ
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



