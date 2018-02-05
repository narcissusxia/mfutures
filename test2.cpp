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
#include <unistd.h>

#include "TraderSpi.h"
#include "MdSpi.h"

using namespace std;  


using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;


CThostFtdcTraderApi* pTraderApi;
CThostFtdcMdApi* pMdApi;

CTraderSpi* spi;
CMdSpi* mdspi;
client mc;
std::string uri = "ws://192.168.1.6:8080/futures/websocket/cppclient001";

// pull out the type of messages sent by our config
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;

websocketpp::connection_hdl m_hdl;


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
      std::string saddr = root["ReqArgs"]["URL"].asString();
      char* addr;  
      const int len = saddr.length();  
      addr=new char[len+1];  
      strcpy(addr,saddr.c_str());

      const int loginID = root["UniqueID"].asInt();
      
      cout  <<"URL=" << root["ReqArgs"]["URL"].asString()<< endl;
      cout  <<"BROKER_ID=" <<root["ReqArgs"]["BROKER_ID"].asString().c_str()<< endl;
      cout  <<"INVESTOR_ID=" << root["ReqArgs"]["INVESTOR_ID"].asString().c_str()<< endl;
      cout  <<"PASSWORD=" << root["ReqArgs"]["PASSWORD"].asString().c_str()<< endl;
      //pUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi();      // 创建UserApi
      //sh= new CTraderSpi();
      //pUserApi->RegisterSpi ((CThostFtdcTraderSpi*)sh);   
      //如果webservice打开,给交易系统赋websocket值
      if (!( NULL == pTraderApi) )
      pTraderApi->Release();
      pTraderApi = CThostFtdcTraderApi::CreateFtdcTraderApi();      // 创建UserApi
      spi = new CTraderSpi(pTraderApi);
      pTraderApi->RegisterSpi ((CThostFtdcTraderSpi*)spi); 
      spi->setWebsocket(c,hdl);
      spi->setUserLoginInfo(loginID,root["ReqArgs"]["BROKER_ID"].asString().c_str()
          ,root["ReqArgs"]["INVESTOR_ID"].asString().c_str(),root["ReqArgs"]["PASSWORD"].asString().c_str());
      //启动Websocket
      pTraderApi->RegisterFront(addr);
      pTraderApi->Init(); 
    }//行情登录请求
    if (root["ReqType"].asString() == "MdReqUserLogin") {
      if (!( NULL == pMdApi) )
      pMdApi->Release();
      pMdApi = CThostFtdcMdApi::CreateFtdcMdApi ();
      mdspi = new CMdSpi(pMdApi);
      //sh= new CTraderSpi();
      pMdApi->RegisterSpi((CThostFtdcMdSpi*)mdspi);   
      std::string saddr = root["ReqArgs"]["MarketUrl"].asString();
      char* addr;  
      const int len = saddr.length();  
      addr=new char[len+1];  
      strcpy(addr,saddr.c_str());
      cout  <<"md addr=" << addr << endl;  
      
      const int loginID = root["UniqueID"].asInt();
      //如果webservice打开,给交易系统赋websocket值
      mdspi->setWebsocket(c,hdl);
      mdspi->setUserLoginInfo(loginID,root["ReqArgs"]["BROKER_ID"].asString().c_str()
          ,root["ReqArgs"]["INVESTOR_ID"].asString().c_str(),root["ReqArgs"]["PASSWORD"].asString().c_str());
      //启动Websocket
      pMdApi->RegisterFront(addr);
      pMdApi->Init(); 
    }//报单确认
    else if(root["ReqType"].asString() == "ReqSettlementInfoConfirm"){
      spi->ReqSettlementInfoConfirm(root);
    }
    //下单请求
    else if(root["ReqType"].asString() == "ReqOrderInsert"){
      spi->ReqOrderInsert(root);
    }//撤单请求
    else if(root["ReqType"].asString() == "ReqOrderInsert"){
      spi->ReqOrderInsert(root);
    }//订阅请求
    else if(root["ReqType"].asString() == "MdSubscribeMarketData"){
      mdspi->MdSubscribeMarketData(root);
    }//查询资金账户
    else if(root["ReqType"].asString() == "ReqQryTradingAccount"){
      spi->ReqQryTradingAccount(root);
    }//查询投资者持仓
    else if(root["ReqType"].asString() == "ReqQryInvestorPosition"){
      spi->ReqQryInvestorPosition(root);
    }


    websocketpp::lib::error_code ec;
    cout  <<"key=" << root["ReqType"].asString()
         << endl;  

    // 访问节点，Return the member named key if it exist, defaultValue otherwise.  
    //code = root.get("uploadid", "null").asString();  
/**
    c->send(hdl, msg->get_payload(), msg->get_opcode(), ec);
    if (ec) {
        std::cout << "Echo failed because: " << ec.message() << std::endl;
    }*/
}

bool status = false;

void on_open(client* c, websocketpp::connection_hdl hdl) {
    // now it is safe to use the connection
    // 初始化UserApi
  status = true;
  std::cout << "Websocket connection ready" << std::endl;
  
}

void on_fail(client * c, websocketpp::connection_hdl hdl) {
       
    cout << "have client on_fail" << endl;
    client::connection_ptr con = c->get_con_from_hdl(hdl);
    std::cout << "on_fail："<< con->get_response_header("Server")<<" "<<con->get_ec().message()<< std::endl; 
}

void on_close(client *c, websocketpp::connection_hdl hdl)
{ 


  status = false;
  cout << "have client on_close" << endl;
  client mmc;
  mmc.set_access_channels(websocketpp::log::alevel::all);
  mmc.clear_access_channels(websocketpp::log::alevel::frame_payload);

    // Initialize ASIO
  mmc.init_asio();

  // Register our message handler
  mmc.set_message_handler(bind(&on_message,&mmc,::_1,::_2));
  mmc.set_open_handler(bind(&on_open,&mmc,::_1));
    // Register our close handler
  mmc.set_close_handler(bind(&on_close, &mmc, _1));
  mmc.set_fail_handler(bind(&on_fail, &mmc, _1));

  client::connection_ptr con;
   
  websocketpp::lib::error_code ec;
  con = mmc.get_connection(uri, ec);
  if (ec) {
          std::cout << " connection error: " << ec.message() << std::endl;
  }
  
  while(!status){
      
      sleep(10);
    
      mmc.connect(con);
      mmc.run();
      //spi->setWebsocket(c,c->get_hdl());
      std::cout  << "close code: "  << con->get_remote_close_reason();

      
   }

  std::cout << "wait for connection run:"<<uri << std::endl;
  

  // Start the ASIO io_service run loop
  // this will cause a single connection to be made to the server. c.run()
  // will exit when this connection is closed.
   

}

int main(int argc, char* argv[]) {
    // Create a client endpoint
  

    //std::string uri = "ws://47.96.149.27/bb/futures/websocket/cppclient";
    

    if (argc == 2) {
        uri = argv[1];
    }


  //pUserApi->Join();
  //pUserApi->Release();

  try {
        // Set logging to be pretty verbose (everything except message payloads)
    mc.set_access_channels(websocketpp::log::alevel::all);
    mc.clear_access_channels(websocketpp::log::alevel::frame_payload);

    // Initialize ASIO
    mc.init_asio();

    // Register our message handler
    mc.set_message_handler(bind(&on_message,&mc,::_1,::_2));
    mc.set_open_handler(bind(&on_open,&mc,::_1));
    // Register our close handler
    mc.set_close_handler(bind(&on_close, &mc, _1));
    mc.set_fail_handler(bind(&on_close, &mc, _1));

    websocketpp::lib::error_code ec;
    client::connection_ptr con = mc.get_connection(uri, ec);
    if (ec) {
            std::cout << "could not create connection because: " << ec.message() << std::endl;
            return 0;
    }

        // Note that connect here only requests a connection. No network messages are
        // exchanged until the event loop starts running in the next line.
    mc.connect(con);
    std::cout  << "connect: " <<  con->get_state() << " ("
              << websocketpp::close::status::get_string(con->get_remote_close_code())
              << "), close reason: " << con->get_remote_close_reason();

        // Start the ASIO io_service run loop
        // this will cause a single connection to be made to the server. c.run()
        // will exit when this connection is closed.
    mc.run();
    
    } catch (websocketpp::exception const & e) {
        std::cout << e.what() << std::endl;
    }
}


