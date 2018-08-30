

#include "net_engine.h"
#include "net_packet.h"
#include "pkt_parse.h"


//-------------------------------------------------------------------------------------------
// Network recieve interface

/* 
 * Handler for Text Messages 
 */
void handleTextPacket(Connection& conn, Packet::sptr packet) {
    TextPacket::sptr textMessage = static_pointer_cast<TextPacket>( packet );                
    // TODO: Read the fucking Message                
    mprintf("got message: \"%s\"\n", textMessage->getMessage().c_str());
    //gout << "got message: " << textMessage->getMessage().c_str() << endl;
    
/*
    TextPacket response("Acknowledge!");
    conn.stream().writePacket(response, true);
*/
}

/* 
 * Packet Parser, to be called from the Net_Engine if there are Packets recieved 
 */
void thePacketParser(Connection& conn, Packet::sptr packet){
    int ID;
    ID = packet->getType();
    switch(ID) {
        case PKT_ID_TEXT:
            handleTextPacket(conn, packet);
            break;
        
        case PKT_ID_INT:
            break;
                                
        default:
            mprintf("got bad packet.\n");
    }    // switch(ID)
;
}
//-------------------------------------------------------------------------------------------


int main(int argc, char *argv[])
{
    NetEngine test;
    /*
    int i;
    cin >> i; 
    test.setPort(i);
    cout << " Port number: " << test.getPort() << endl;
    */
    
  gout << "Local address: " << getLocalAddress() << endl;
  gout << "Should we act as the server, or the client?" << endl;
  gout << "Type 1 for client, 2 for server: ";
  int type;
  gin >> type;

  if (type == 2) {
    setTitle("GNE Basic Connections Example -- Server");
    gout << "Reminder: ports <= 1024 on UNIX can only be used by the superuser." << endl;
    cout << " Port number: " << test.getPort() << endl;
    test.startServer();

    gout << "Press a key to kill Server." << endl;
    getch();
    test.stopServer();

  } else {
    setTitle("GNE Basic Connections Example -- Client");
    cout << " Port number: " << test.getPort() << endl;

    string addr;
    cout << " Enter host Adress: ";
    cin >> addr;
    test.startClient(addr);
    test.SendData("hallo");
    test.SendData("hallo popocatepetl");    
    test.SendDataSy("hallo syncrotron");    
    gout << "Press a key to kill Client." << endl;
    getch();
    test.stopClient();
  }

    gout << "Press a key to continue." << endl;
    getch();
  
  return 0;
}
