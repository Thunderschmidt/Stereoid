#ifndef NET_PACKET_H
#define NET_PACKET_H
/*
  Name: net_packet
  Copyright: 
  Author: HP 
  Date: 06.05.04 16:17
  Description: Network Engine Packet header file for ActionCommander
*/
// User Packet ID definitions
#define PKT_ID_TEXT MIN_USER_ID + 1
#define PKT_ID_INT MIN_USER_ID + 2


/* Packet for a standard string Message */
class TextPacket : public Packet 
   {
   public:
      typedef SmartPtr<TextPacket> sptr;
      typedef WeakPtr<TextPacket> wptr;

   public:
      TextPacket() : Packet(ID) {}
      //copy ctor not needed
      TextPacket(string message2) : Packet(PKT_ID_TEXT), message(message2) {}

      virtual ~TextPacket() {}
      
      static const int ID = PKT_ID_TEXT;
      
      int getSize() const {
         return Packet::getSize() + Buffer::getSizeOf(message);
         }
         
      void writePacket(Buffer& raw) const {
         Packet::writePacket(raw);
         raw << message;
         }

      void readPacket(Buffer& raw) {
         Packet::readPacket(raw);
         raw >> message;
         }
      
      string getMessage() {
         return message;
         }

   private:
      string message;
   };

//const int TextPacket::ID = PKT_ID_TEXT;

#endif

