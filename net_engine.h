#ifndef NET_ENGINE_H
#define NET_ENGINE_H

/*
  Name: net_engine
  Copyright: 
  Author: HP 
  Date: 06.05.04 16:17
  Description: Network Engine Class header file for ActionCommander
*/

// GNE Header
#include <gnelib.h>

// CPP Headers
#include <iostream>
#include <string>

// Get namespaces
using namespace std;
using namespace GNE;
using namespace GNE::Console;
using namespace GNE::PacketParser;

#include "net_engine_int.h"

typedef enum {
    IS_NONE = 0,
    IS_SERVER,
    IS_CLIENT
} ENM_STATE;


class NetEngine 
{
   protected:
   
   public:
      NetEngine();                 // Constructor of the Engine
      ~NetEngine() {}                  // Destructor of the Engine
      
      // Connection management
      void setPort(int PortNr);
      
      int startServer();
      int stopServer();
      
      int startClient(string host);
      int stopClient();   

      // Data transfer
      int SendData(string msg);       // Send string Data over UPD (unreliable)
      int SendDataSy(string msg);     // Send string Data over TCP (reliable)
         
      
      // Info functions
      inline int        getPort() {return(Port);}
      inline ENM_STATE  getState() {return(State);}
      inline bool       isConnected() {return(Connected);}
   
   private:
      int           Port;
      string        errortext;
      ENM_STATE     State;
      bool          Connected;
      
      OurServerListener::sptr listener;
      OurServer::sptr server;
      
      ClientConnection::sptr client;         
};
   
#endif
