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

#include "TraderSpi.h"

using namespace std;  


using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;
// UserApi对象
CThostFtdcTraderApi* pUserApi;
CTraderSpi* sh;
client c;

// pull out the type of messages sent by our config
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;

websocketpp::connection_hdl m_hdl;
// This message handler will be invoked once for each incoming message. It
// prints the message and then sends a copy of the message back to the server.
/**
void CTraderSpi::OnFrontConnected()
{
       printf("OnFrontConnected:called.\n");
       static int i = 0;
       // 在登出后系统会重新调用OnFrontConnected，这里简单判断并忽略第1次之后的所有调用。
       if (i++==0) {
           printf("OnFrontConnected:called.\n");
            //sem_post(&sem);
       }
       std::string SIP_msg="xialei1981\r\n";
       c.send(m_hdl, SIP_msg.c_str(), websocketpp::frame::opcode::text);
        
       
};

void CTraderSpi::ReqUserLogin()
{
  CThostFtdcReqUserLoginField req;
  //memset(&req, 0, sizeof(req));
  //strcpy(req.BrokerID, BROKER_ID);
  //strcpy(req.UserID, INVESTOR_ID);
  //strcpy(req.Password, PASSWORD);
  //int iResult = pUserApi->ReqUserLogin(&req, ++iRequestID);
  cerr << "--->>> 发送用户登录请求: "  << endl;
}

void CTraderSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
  cerr << "--->>> " << "OnRspUserLogin" << endl;
   
}

*/

//JsonCpp
Json::Value readStrJson(const std::string& str)   
{  
 
   
  Json::Reader reader;  
  Json::Value root;  
   
  if(reader.parse(str,root))  
  {  
    return root;
  } 
    
}  

void on_message(client* c, websocketpp::connection_hdl hdl, message_ptr msg) {
    std::cout << "on_message called with hdl: " << hdl.lock().get()
              << " and message: " << msg->get_payload()
              << std::endl;
    //sh->ReqUserLogin();
    Json::Value root = readStrJson(msg->get_payload());

    //登录请求
    if (root["ReqType"].asString() == "ReqUserLogin") {
      const char* saddr = root["ReqArgs"]["URL"].asString().c_str();
      char* addr   = new char[strlen(saddr)+1];
      strcpy(addr, saddr);
      
      cout  <<"UniqueID=" << root["ReqArgs"]["URL"].asString()<< endl;
      cout  <<"BROKER_ID=" <<root["ReqArgs"]["BROKER_ID"].asString().c_str()<< endl;
      cout  <<"INVESTOR_ID=" << root["ReqArgs"]["INVESTOR_ID"].asString().c_str()<< endl;
      cout  <<"PASSWORD=" << root["ReqArgs"]["PASSWORD"].asString().c_str()<< endl;
      pUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi();      // 创建UserApi
      sh= new CTraderSpi();
      pUserApi->RegisterSpi ((CThostFtdcTraderSpi*)sh);   
      //如果webservice打开,给交易系统赋websocket值
      sh->setWebsocket(c,hdl);
      sh->setUserLoginInfo(root["ReqArgs"]["BROKER_ID"].asString().c_str()
          ,root["ReqArgs"]["INVESTOR_ID"].asString().c_str(),root["ReqArgs"]["PASSWORD"].asString().c_str());
      //启动Websocket
      pUserApi->RegisterFront(addr);
      pUserApi->Init(); 
    }

    websocketpp::lib::error_code ec;
    cout  <<"key=" << root["ReqType"].asString()
         << endl;  

    // 访问节点，Return the member named key if it exist, defaultValue otherwise.  
    //code = root.get("uploadid", "null").asString();  

    c->send(hdl, msg->get_payload(), msg->get_opcode(), ec);
    if (ec) {
        std::cout << "Echo failed because: " << ec.message() << std::endl;
    }
}


void on_open(client* c, websocketpp::connection_hdl hdl) {
    // now it is safe to use the connection
    // 初始化UserApi
  std::cout << "Websocket connection ready" << std::endl;
  pUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi();      // 创建UserApi
  sh= new CTraderSpi();
  pUserApi->RegisterSpi ((CThostFtdcTraderSpi*)sh);   
  //CTraderSpi* pUserSpi = new CTraderSpi();

  //pUserApi->RegisterSpi((CThostFtdcTraderSpi*)pUserSpi);      // 注册事件类
  //pUserApi->SubscribePublicTopic(TERT_RESTART);         // 注册公有流
  //pUserApi->SubscribePrivateTopic(TERT_RESTART);          // 注册私有流
  printf("usereed:\n");
  
  // 等待服务器发出登录消息
  //sem_wait(&sem);
  
  
    //如果webservice打开,给交易系统赋值
  sh->setWebsocket(c,hdl);
    //received=false;
    // Send a SIP OPTIONS message to the server:
  
   
}


int main(int argc, char* argv[]) {
    // Create a client endpoint
  

    std::string uri = "ws://192.168.1.6:8080/futures/websocket/cppclient";

    if (argc == 2) {
        uri = argv[1];
    }

    const char* str =   
      "{\"praenomen\":\"Gaius\",\"nomen\":\"Julius\",\"cognomen\":\"Caezar\","  
      "\"born\":-100,\"died\":-44}";

    Json::Value root = readStrJson(str);  
      
    string praenomen = root["praenomen"].asString();  
    string nomen = root["nomen"].asString();  
    string cognomen = root["cognomen"].asString();  
    int born = root["born"].asInt();  
    int died = root["died"].asInt();  
    cout << praenomen + " " + nomen + " " + cognomen  
          << " was born in year " << born   
          << ", died in year " << died << endl;  

     // 初始化线程同步变量
  //sem_init(&sem,0,0);
  printf("userid:\n");
  // 等待登录成功消息
  //sem_wait(&sem);

  

 
  printf("usereed:\n");
  //pUserApi->Init();
  printf("usereed:\n");

  //pUserApi->Join();
  //pUserApi->Release();

  try {
        // Set logging to be pretty verbose (everything except message payloads)
    c.set_access_channels(websocketpp::log::alevel::all);
    c.clear_access_channels(websocketpp::log::alevel::frame_payload);

    // Initialize ASIO
    c.init_asio();

    // Register our message handler
    c.set_message_handler(bind(&on_message,&c,::_1,::_2));
    c.set_open_handler(bind(&on_open,&c,::_1));

    websocketpp::lib::error_code ec;
    client::connection_ptr con = c.get_connection(uri, ec);
    if (ec) {
            std::cout << "could not create connection because: " << ec.message() << std::endl;
            return 0;
    }

        // Note that connect here only requests a connection. No network messages are
        // exchanged until the event loop starts running in the next line.
    c.connect(con);

        // Start the ASIO io_service run loop
        // this will cause a single connection to be made to the server. c.run()
        // will exit when this connection is closed.
    c.run();
    
    } catch (websocketpp::exception const & e) {
        std::cout << e.what() << std::endl;
    }
}


