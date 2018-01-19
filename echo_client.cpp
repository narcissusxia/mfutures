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
typedef websocketpp::client<websocketpp::config::asio_client> client;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

// pull out the type of messages sent by our config
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;

// This message handler will be invoked once for each incoming message. It
// prints the message and then sends a copy of the message back to the server.

void CTraderSpi::OnFrontConnected()
{
       printf("OnFrontConnected:called.\n");
       static int i = 0;
       // 在登出后系统会重新调用OnFrontConnected，这里简单判断并忽略第1次之后的所有调用。
       if (i++==0) {
           printf("OnFrontConnected:called.\n");
            //sem_post(&sem);
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

void on_message(client* c, websocketpp::connection_hdl hdl, message_ptr msg) {
    std::cout << "on_message called with hdl: " << hdl.lock().get()
              << " and message: " << msg->get_payload()
              << std::endl;

    Json::Value root = readStrJson(msg->get_payload());  
    websocketpp::lib::error_code ec;
    cout  <<"key=  " << root["msg"].asString()   
         << endl;  

    c->send(hdl, msg->get_payload(), msg->get_opcode(), ec);
    if (ec) {
        std::cout << "Echo failed because: " << ec.message() << std::endl;
    }
}

int main(int argc, char* argv[]) {
    // Create a client endpoint
    client c;

    std::string uri = "ws://47.96.149.27/bb/futures/websocket/111";

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
  sem_init(&sem,0,0);
    printf("userid:\n");
    // 等待登录成功消息
  //sem_wait(&sem);

  // 初始化UserApi
  pUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi();      // 创建UserApi
  CTraderSpi* sh= new CTraderSpi();
  pUserApi->RegisterSpi ((CThostFtdcTraderSpi*)sh);   
  //CTraderSpi* pUserSpi = new CTraderSpi();

  //pUserApi->RegisterSpi((CThostFtdcTraderSpi*)pUserSpi);      // 注册事件类
  //pUserApi->SubscribePublicTopic(TERT_RESTART);         // 注册公有流
  //pUserApi->SubscribePrivateTopic(TERT_RESTART);          // 注册私有流
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

    try {
        // Set logging to be pretty verbose (everything except message payloads)
        c.set_access_channels(websocketpp::log::alevel::all);
        c.clear_access_channels(websocketpp::log::alevel::frame_payload);

        // Initialize ASIO
        c.init_asio();

        // Register our message handler
        c.set_message_handler(bind(&on_message,&c,::_1,::_2));

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


 

