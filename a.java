package com.websockets;

import java.io.IOException;
 
import java.util.Map;
import java.util.Set;
import java.util.concurrent.CopyOnWriteArraySet;

import javax.websocket.CloseReason;
import javax.websocket.OnClose;
import javax.websocket.OnError;
import javax.websocket.OnMessage;
import javax.websocket.OnOpen;
import javax.websocket.Session;
import javax.websocket.server.ServerEndpoint;

 
//@ServerEndpoint("/websocket/{jstoken}")
@ServerEndpoint("/websocket")
public class WebSocketsMain {
	private static final Set<WebSocketsMain> connections = new CopyOnWriteArraySet<>();
	private static final Set<WebSocketsMain> connectionsOfAssistant = new CopyOnWriteArraySet<>();
	
    private Session session;
 
  @OnMessage
  public void onMessage(String message, Session session)
    throws IOException, InterruptedException {
	  WebSocketsMain.broadCast(message);
	  System.out.println("::"+message);
  }
  @OnError
  public void onError(Session session, Throwable throwable){
	  
  }
   
  @OnOpen
  public void onOpen(Session session) {
    Map<String, String> params = session.getPathParameters();
//    String jstoken = params.get("jstoken");

	this.session = session;
	connections.add(this);
	    	


  }
 
  @OnClose
  public void onClose(Session session, CloseReason reason) {
    if(connections.contains(this)){
    	connections.remove(this);
    }else{
    	connectionsOfAssistant.remove(this);
//    	WebSocketTest.assistantBroadCast("{\"online_total\":\""+(connectionsOfAssistant.size()+connections.size())+"\"}");
    }
    
  }
  
  /**
   * ·¢Ë»ò¥ÐϢ
   * 
   * @param message
   */
  public static void broadCast(String message) {
      for (WebSocketsMain chat : connections) {
          try {
              synchronized (chat) {
                  chat.session.getBasicRemote().sendText(message);
              }
          } catch (IOException e) {
              connections.remove(chat);
              try {
                  chat.session.close();
              } catch (IOException e1) {
              }
          }
      }
      WebSocketsMain.assistantBroadCast(message);
  }

  public static void assistantBroadCast(String message) {
      for (WebSocketsMain chat : connectionsOfAssistant) {
          try {
              synchronized (chat) {
            	  
                  chat.session.getBasicRemote().sendText(message);
              }
          } catch (IOException e) {
        	  connectionsOfAssistant.remove(chat);
              try {
                  chat.session.close();
              } catch (IOException e1) {
              }
              
          }
      }
  }
}
