#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#pragma warning(disable : 4996)
#endif


#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

#include <iostream>
#include <json/json.h>


#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#include <iostream>
 
#include <Poco/Net/WebSocket.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/NetException.h>
#include <Poco/Exception.h>

#include "../TraderAPI/ThostFtdcTraderApi.h"
//#include "TraderSpi.h"
using Poco::Net::HTTPClientSession;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::WebSocket;
using Poco::Net::WebSocketException;
using Poco::Exception;



using namespace std;  
typedef websocketpp::client<websocketpp::config::asio_client> client;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

// pull out the type of messages sent by our config
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;
// UserApi对象
CThostFtdcTraderApi* pUserApi;

sem_t sem;

int iRequestID = 0;
TThostFtdcBrokerIDType  BROKER_ID = "9999";       // 经纪公司代码
TThostFtdcInvestorIDType INVESTOR_ID = "110623";      // 注意输入你自己的simnow仿真投资者代码
TThostFtdcPasswordType  PASSWORD = "xialei1981";      // 注意输入你自己的simnow仿真用户密码
TThostFtdcFrontIDType FRONT_ID; //前置编号
TThostFtdcSessionIDType SESSION_ID; //会话编号
TThostFtdcOrderRefType  ORDER_REF;  //报单引用
time_t lOrderTime;
time_t lOrderOkTime;


class CTraderSpi : public CThostFtdcTraderSpi
{

  
  websocketpp::connection_hdl hdl;

public:
 // CTraderSpi (//websocketpp::client<websocketpp::config::asio_client>  *pclient,
   // websocketpp::connection_hdl * phdl) {this.hdl = phdl;} 

  ~CTraderSpi(){}
    

    //Json::Value root = readStrJson(msg->get_payload());
    //websocketpp::lib::error_code ec;
    //cout  <<"kcout  <<ey=  " << root["msg"].asString()   
    //     << endl;  



    //c->send(hdl, msg->get_payload(), msg->get_opcode(), ec);
    //if (ec) {
    //    std::cout << "Echo failed because: " << ec.message() << std::endl;
    //}

  WebSocket * ws ;
  ///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
  virtual void OnFrontConnected(){
    printf("OnFrontConnected:called.\n");
 
    int flags;
    int n;
    std::string payload;

    HTTPClientSession cs("192.168.1.6", 8080);
    HTTPRequest request(HTTPRequest::HTTP_GET, "/futures/websocket/cppclient", "HTTP/1.1");
        HTTPResponse response;
        std::string cmd;
 
        ws = new WebSocket(cs, request, response); // Causes the timeout
  char buffer[1024];
        payload = "SGClient: Hello World!";
        cout << "Send: SGClient: Hello World!" << endl;
        ws->sendFrame(payload.data(), payload.size(), WebSocket::FRAME_TEXT);
        n = ws->receiveFrame(buffer, sizeof(buffer), flags);
        buffer[n] = '\0';
        cout << "Received: " << buffer << endl;
        //
        
        do
        {
           //cout << "Received: " << endl;
         
            flags = 0;
           
                n = ws->receiveFrame(buffer, sizeof(buffer), flags);
                if (n > 0)

              buffer[n] = '\0';
              cout << "Received: " << buffer << endl;
                    if ((flags & WebSocket::FRAME_OP_BITMASK) == WebSocket::FRAME_OP_BINARY)
                    {
                        // process and send out to all other clients
                        
                    }
                
            
        }
        while (true);
        // client has closed, so remove from list
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
        int iResult = pUserApi->ReqUserLogin(&reqUserLogin, ++iRequestID);         
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
    //ReqSettlementInfoConfirm();
    }

     std::string payload = "SGClient: Hello World!";
    cout << "Send: SGClient: Hello World!" << endl;
    ws->sendFrame(payload.data(), payload.size(), WebSocket::FRAME_TEXT);

  }


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



};


Json::Value readStrJson(const std::string& str)   
{  
 
  Json::Reader reader;  
  Json::Value root;  
   
  if(reader.parse(str,root))  
  {  
    return root;
  }  
    
}  



int main(int argc, char* argv[]) {

          // 初始化线程同步变量
  //sem_init(&sem,0,0);
  printf("userid:\n");
    // 等待登录成功消息
  //
  
 // sem_wait(&sem);
  // 初始化UserApi
  // 
  pUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi();      // 创建UserApi
  CTraderSpi* sh= new CTraderSpi();
  pUserApi->RegisterSpi ((CThostFtdcTraderSpi*)sh);   
  //CTraderSpi* pUserSpi = new CTraderSpi();

  //pUserApi->RegisterSpi((CThostFtdcTraderSpi*)pUserSpi);      // 注册事件类
  //pUserApi->SubscribePublicTopic(TERT_RESTART);         // 注册公有流
  //pUserApi->SubscribePrivateTopic(TERT_RESTART);          // 注册私有流
  printf("usereed:\n");
  pUserApi->RegisterFront("tcp://180.168.146.187:10030"); 
  //tcp://180.168.146.187:10000
  //tcp://180.168.212.70:41205
  //180.168.146.187:10030
  pUserApi->Init();
  // 等待服务器发出登录消息
  //sem_wait(&sem);
 
 
  printf("usereed:\n");
 

  pUserApi->Join();
   //pUserApi->Init();
  //printf("jjjj:\n");
  //pUserApi->Release();
}


 

