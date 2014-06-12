#ifndef CAMPFIREAPI_CAMPFIRE_H
#define CAMPFIREAPI_CAMPFIRE_H

#include <pthread.h>
#include <vector>
#include <string>
#include <curl/curl.h>
#include "CampfireAPI.h"

class RestClient;

std::string IntToString(int nValue);

struct RoomEntry
{
	RoomEntry(const std::string& strName, int nID);

	std::string m_strName;
   int m_nID;
};

enum Type
{
   Text        = MESSAGE_TYPE_TEXT,
   TimeStamp   = MESSAGE_TYPE_TIMESTAMP,
   EnterLeave  = MESSAGE_TYPE_ENTERLEAVE,
   Upload      = MESSAGE_TYPE_UPLOAD
};

struct Base_Message
{
   Base_Message(Type eType);
   virtual ~Base_Message(){}
   Type m_eType;

   Type GetType() const;
   void PutOnto(int& nType, std::string& strPerson, std::string& strMessage);
   virtual void PutOnto(std::string& strPerson, std::string& strMessage) = 0;
};

struct Text_Message : public Base_Message
{
   Text_Message(const std::string& strMessage, int nPersonID);
   virtual void PutOnto(std::string& strPerson, std::string& strMessage);
   std::string m_strMessage;
	int m_nPersonID;
};

struct TimeStamp_Message : public Base_Message
{
   TimeStamp_Message(const std::string& strDate, const std::string& strTime);
   virtual void PutOnto(std::string& strPerson, std::string& strMessage);
   std::string m_strDate;
	std::string m_strTime;
};

struct EnterLeave_Message : public Base_Message
{
   EnterLeave_Message(const std::string& strPerson, const std::string& strSubject);
   virtual void PutOnto(std::string& strPerson, std::string& strMessage);
   std::string m_strPerson;
	std::string m_strSubject;
};

struct Upload_Message : public Base_Message
{
   Upload_Message(const std::string& strPerson, const std::string& strSubject);
   virtual void PutOnto(std::string& strPerson, std::string& strURL);
   std::string m_strPerson;
	std::string m_strURL;
};

struct Messages
{
   ~Messages();
   void AddTextMessage(const std::string& strMessage, int nUserID);
   void AddTimeStamp(const std::string& strDate, const std::string& strTime);
   void AddEnterLeaveMessage(const std::string& strPerson, const std::string& strBody);
   void AddUploadFile(const std::string& strPerson, const std::string& strURL);

   int Count() const;
   void Clear();
   void PutMessageOnto(int nIndex, int& nType, std::string& strPerson, std::string& strMessage);

   std::vector<Base_Message*> m_Messages;
};

enum Sounds
{
   Crickets = 1,
   Rimshot,
   Trombone,
   whoomp,
   Loggins,
   Bueller,
   Yoel,
   Tada,
   Horror,
   Tmyk,
   Ohyeah,
   Horn,
   Noooo,
   Live,
   Heygirl,
   Ohmy,
   Greatjob,
   Yeah,
   What,
   Inconceivable,
   Dangerzone,
   Secret,
   Pushit,
   Drama,
   Vuvuzla,
   Sax,
   _56k
};

class Campfire
{
   enum MessageType
   {
      SayMessage,
      PasteMessage,
      SoundMessage
   };
public:
	Campfire();
	~Campfire();

	bool Login(const std::string& strHost, const std::string& strAuthCode, bool bUseSSL);
	int GetRoomCount();
	bool GetRoomDetails(int nRoomIndex, std::string& strRoomName, int& id);
	bool JoinRoom(int nRoomIndex);
	bool Say(const std::string& strMessage, int& nMessageID);
	bool Paste(const std::string& strMessage, int& nMessageID);
   bool PlaySound(Sounds eSound);
	bool UploadFile(const std::string& strFilePath);
   bool ClearMessages();
   int GetMessageCount();
   bool GetMessage(int nIndex, int& nType, std::string& strPerson, std::string& strMessage);

	bool Listen();
	bool GetListenMessage(std::string& strMessage);

	bool Leave();
	bool SetRoomTopic(const std::string& strTopic);
   bool StarMessage(int nMessageID);

protected:
	bool SayPaste(const std::string& strMessage, MessageType eType, int& nMessageID);
	std::string GetURL(const std::string& strHost, bool bUseSSL);
   std::string GetRoomURL(const std::string& strHost, int nRoomNum, bool bUseSSL);
   std::string GetStreamingURL(int nRoomNum, bool bUseSSL);
	bool ParseListenResponse(const std::string& strListenResponse);

   static size_t listen_callback(void *ptr, size_t size, size_t nmemb,
      void *userdata);

   static void* ListenThread(void* ptr);
   void ListenWorker();

protected:
   pthread_t m_threadListen;
   pthread_mutex_t m_mutexListen;
   std::vector<std::string> m_aListenMessages;

   bool m_bExit;

   RestClient* m_pRest;//Used for everything except for listening.

   std::string m_strAuthCode;

   int m_nLastMessageID;

   Messages m_Messages;
   std::vector<RoomEntry> m_RoomList;

	std::string m_strHost;
   int m_nRoomNum;
	bool m_bUseSSL;
};


#endif
