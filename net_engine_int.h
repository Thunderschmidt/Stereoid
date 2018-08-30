#ifndef NET_ENGINE_INT_H
#define NET_ENGINE_INT_H
/*
  Name: net_engine_int
  Copyright: 
  Author: HP 
  Date: 06.05.04 16:17
  Description: Network Engine internal header file for ActionCommander
*/

#include "pkt_parse.h"

/* Server */
class OurServer : public ConnectionListener 
{
    public: //typedefs
        typedef SmartPtr<OurServer> sptr;
        typedef SmartPtr<OurServer> wptr;

    protected:
        OurServer() : received(false) 
        {
            mprintf("Server listener created\n");
        }

    public:
        static sptr create() 
        {
            return sptr( new OurServer() );
        }
        
        virtual ~OurServer() 
        {
            mprintf("Server listener killed\n");
        }

  void onDisconnect( Connection& conn ) { 
    //Call receivePackets one last time to make sure we got all data.
    //It is VERY possible for data still left unread if we get this event,
    //even though we read all data from onReceive.
    receivePackets( conn );
    mprintf("ServerConnection just disconnected.\n");
    if (!received)
      mprintf("No message received.\n");
  }

  void onExit( Connection& conn ) {
    mprintf("Client gracefully disconnected.\n");
  }

  void onNewConn( SyncConnection& conn2) {
    Connection& conn = *conn2.getConnection();
    mprintf("Connection received; waiting for message...\n");
    mprintf("  From us at %s/TCP to client at %s/TCP.\n",
      conn.getLocalAddress(true).toString().c_str(),
      conn.getRemoteAddress(true).toString().c_str());
    //Test to see if we have an unreliable local address, and if so, report
    //what the addresses are.
    if (conn.getLocalAddress(false)) {
      mprintf("  From us at %s/UDP to client at %s/UDP.\n",
        conn.getLocalAddress(false).toString().c_str(),
        conn.getRemoteAddress(false).toString().c_str());
    } else {
      mprintf("  Unreliable connection refused by client.\n");
    }
  }

  void onReceive( Connection& conn ) {
    receivePackets( conn );
  }

  void onFailure( Connection& conn, const Error& error ) {
    mprintf("Socket failure: %s\n", error.toString().c_str());
  }

  void onError( Connection& conn, const Error& error ) {
    mprintf("Socket error: %s\n", error.toString().c_str());
    conn.disconnect();
  }

    /*
    * Receive and process packets for Server
    */
    inline void receivePackets( Connection& conn ){
        Packet::sptr packet;
        while ( (packet = conn.stream().getNextPacketSp() ) ) {
            thePacketParser(conn ,packet);
            }
        }    // receivePackets( Connection& conn );


private:
  bool          received;
};

/* Server Listener */
class OurServerListener : public ServerConnectionListener {
    protected:
        OurServerListener(int iRate, int oRate) 
            : ServerConnectionListener(), iRate(iRate), oRate(oRate) {}

    public:
        typedef SmartPtr<OurServerListener> sptr;
        typedef WeakPtr<OurServerListener> wptr;
        
        static sptr create(int iRate, int oRate) 
        {
            sptr ret( new OurServerListener( iRate, oRate ) );
            ret->setThisPointer( ret );
            return ret;
        }
        
        virtual ~OurServerListener() {}

        void onListenFailure(const Error& error, const Address& from, const ConnectionListener::sptr& listener) 
        {
            mprintf("Connection error: %s\n", error.toString().c_str());
            mprintf("  Error received from %s\n", from.toString().c_str());
        }
        
        void getNewConnectionParams(ConnectionParams& params);
        
        inline OurServer::sptr getServer() {return(server);} // returns Server object Pointer

    private:
        int iRate;
        int oRate;
        
        OurServer::sptr server;
        
};


/* Clinet */
class OurClient : public ConnectionListener {
public: //typedefs
  typedef SmartPtr<OurClient> sptr;
  typedef SmartPtr<OurClient> wptr;

protected:
  OurClient() {
    mprintf("Client listener created.\n");
  }

public:
  static sptr create() {
    return sptr( new OurClient() );
  }

  ~OurClient() {
    mprintf("Client listener destroyed.\n");
  }

  void onDisconnect( Connection& conn ) { 
    mprintf("Client just disconnected.\n");
  }

  void onExit( Connection& conn ) {
    mprintf("Server gracefully disconnected.\n");
  }

  void onConnect(SyncConnection& conn2) {
    Connection& conn = *conn2.getConnection();
    mprintf("Connection to server successful.\n");
    mprintf("  From us at %s/TCP to server at %s/TCP.\n",
      conn.getLocalAddress(true).toString().c_str(),
      conn.getRemoteAddress(true).toString().c_str());
    //Test to see if we have an unreliable local address, and if so, report
    //what the addresses are.
    if (conn.getLocalAddress(false)) {
      mprintf("  From us at %s/UDP to server at %s/UDP.\n",
        conn.getLocalAddress(false).toString().c_str(),
        conn.getRemoteAddress(false).toString().c_str());
    } else {
      mprintf("  Unreliable connection refused by server.\n");
    }
  }

    void onReceive( Connection& conn ) {
        Packet::sptr packet;
        while ( (packet = conn.stream().getNextPacketSp() ) ) {
            thePacketParser(conn ,packet);
        }
    }

  void onFailure( Connection& conn, const Error& error ) {
    mprintf("Socket failure: %s\n", error.toString().c_str());
    //No need to disconnect, this has already happened on a failure.
  }

  void onError( Connection& conn, const Error& error ) {
    mprintf("Socket error: %s\n", error.toString().c_str());
    conn.disconnect();//For simplicity we treat even normal errors as fatal.
  }

  void onConnectFailure( Connection& conn, const Error& error) {
    //Since we are using join in our client connecting code, this event is
    //not really needed, but we can display the error message here or in the
    //doClient function when it finds out the connection failed.  Either way
    //does the same thing.
    mprintf("Connection to server failed.\n");
    mprintf("GNE reported error: %s\n", error.toString().c_str());
  }
};

 
#endif
