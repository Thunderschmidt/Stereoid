/* 
 * Network Engine Class source file for ActionCommander 
 * May 6th 2004, by HP 
 *
 */

#include "net_engine.h"
#include "net_packet.h"


/* 
 * Constructor
 */
NetEngine::NetEngine()
{
    if (initGNE(NL_IP, atexit)) 
    {
        exit(1);
    }
    
    setGameInformation("Action Commander", 1);

    initConsole();
    setTitle("Action Commander Network Engine");

    // register Packets using default behavior and allocators.
    defaultRegisterPacket<TextPacket>();

    gout << GNE::VER_STR << endl;
    gout << "Local address: " << getLocalAddress() << endl;
    
    State       = IS_NONE;
    Connected   = false;
    Port        = 1055;
    errortext   = " Nothing";
       
}    // Constructor()


void NetEngine::setPort(int PortNr)
{
    if ((PortNr < 1) || (PortNr > 65535)) 
    {
        gout << PortNr << ": Ports range from 1 to 65535, please select one in this range! ";
    }
    else
        Port = PortNr;

}   // setPort(int PortNr);


//-------------------------------------------------------------------------------------------
// Server specific functions
/*
 * Setting Parameters and start Server
 */
void OurServerListener::getNewConnectionParams(ConnectionParams& params) 
{
    params.setInRate(iRate);
    params.setOutRate(oRate);
    
    // Set UPD channel
    params.setUnrel(true);
    // Register the Server
    server = OurServer::create();
    params.setListener( server );
}

/* 
 * Start Server functionality
 */
int NetEngine::startServer()
{
    int Return = 0;
    
    #ifdef _DEBUG
    //Generate debugging logs to server.log if in debug mode.
    initDebug(DLEVEL1 | DLEVEL2 | DLEVEL3 | DLEVEL4 | DLEVEL5, "server.log");
    #endif
    
    if (State == IS_NONE) {
        listener = OurServerListener::create(0, 0);
        Return = listener->open(Port);
        if (Return)
            errortext = "Cannot open server socket.";
        else {
            Return = listener->listen();
            if (Return)
                errortext = "Cannot listen on server socket.";
            else {    // Server is listening
                Connected = true;
                State = IS_SERVER;
                gout << "Server is listening on: " << listener->getLocalAddress() << endl;
            }
        }
        
        // Server failure
        if (Return)
            listener->close();
    } else {
        errortext = "Client or Server is already open! ";
        Return = 1;
    }
    
    return(Return);
}    // startServer();

/* 
 * Stop Server functionality
 */
int NetEngine::stopServer() {    
    if (State == IS_SERVER) {
        listener->close();
        State = IS_NONE;
        gout << " Server is closed. " << endl;
    } else {
        errortext = "No Server to close! ";
        return(1);
    }
    return(0);
}    // stopServer();
//-------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------
// Client specific functions
/* 
 * Start Client functionality
 */
int NetEngine::startClient(string host) {
    #ifdef _DEBUG
      initDebug(DLEVEL1 | DLEVEL2 | DLEVEL3 | DLEVEL4 | DLEVEL5, "client.log");
    #endif

    Address address(host);
    address.setPort(Port);
    if (!address){
        errortext = "Invalid address.";
        return(1);
        }
    gout << "Connecting to: " << address << endl;

  //uncomment the loop and reduce sleep time to perform a stress test.
  //for (int i=0; i < 100; i++) {
    ConnectionParams params( OurClient::create() );
    params.setUnrel(true);
    params.setOutRate(0);
    params.setInRate(0);
    client = ClientConnection::create();
    if (client->open(address, params)) {
        errortext = "Cannot open client socket.";
        return(1);
        }
    
    client->connect();
    client->waitForConnect();
    
    //Check if our connection was successful.
    if (client->isConnected()) {
        Connected = true;
        State = IS_CLIENT;
        gout << " Client is active " << endl;
      
/*
      //Send our information
      HelloPacket message("Hello, server!  I'm the event-driven client!");
      client->stream().writePacket(message, true);
      
      //This sends another hello over the unreliable connection.  exsynchello
      //can't use unreliable connections because it uses SyncConnection, but
      //it chose to refuse an unreliable connection when we connected.  When
      //no unreliable connection exists, unreliable packets are sent reliably
      //so doing this will send it over UDP (or IPX) when connecting to an
      //exhello server, but over TCP (or SPX) when connecting to exsynchello.
      client->stream().writePacket(message, false);

      //Wait a little for any responses.
      gout << "Waiting a couple of seconds for any responses..." << endl;
      Thread::sleep(2000);
*/
    }
    return(0);
}    // startClient(string host);

/* 
 * Stop Client functionality
 */
int NetEngine::stopClient() {    
    if (State == IS_CLIENT) {
        client->disconnectSendAll();      
        //client->close();
        State = IS_NONE;
        gout << " Client is closed. " << endl;
    } else {
        errortext = "No Client to close! ";
        return(1);
    }
    return(0);
}    // stopClient();
//-------------------------------------------------------------------------------------------


 
//-------------------------------------------------------------------------------------------
// Send Data functions for Client
/* 
 * Send string Data over UPD (unreliable)
 */
int NetEngine::SendData(string msg) 
{
    if(State == IS_CLIENT && Connected) {
        // Generate Packet
        TextPacket msg_pack(msg);
        client->stream().writePacket(msg_pack, false);
    } else {
        errortext = "No Client open! ";
        return(1);
    }
    return(0);
}    // SendData(string msg);

/* 
 * Send string Data over TCP (reliable),
 * causes a response
 */
int NetEngine::SendDataSy(string msg) 
{
    if(State == IS_CLIENT && Connected) {
        // Generate Packet
        TextPacket msg_pack(msg);
        client->stream().writePacket(msg_pack, true);
    } else {
        errortext = "No Client open! ";
        return(1);
    }
    return(0);
}    // SendDataSy(string msg);
//-------------------------------------------------------------------------------------------
