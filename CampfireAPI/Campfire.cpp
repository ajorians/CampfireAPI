#include "Campfire.h"
#include "CampfireAPI.h"
#include <sstream>

#include <fstream>
#include <stdlib.h>

#ifndef WIN32
#include <string.h>
#endif

#include "RestClientFactory.h"
#include "RestClient.h"

#include <iostream>

using namespace std;

#ifndef SAFE_DELETE
#define SAFE_DELETE(x) { delete (x); (x) = NULL; }
#endif

CAMPFIRE_EXTERN int CampfireCreate(CampfireAPI* api)
{
	*api = new Campfire;

	return 0;
}

CAMPFIRE_EXTERN int CampfireFree(CampfireAPI *api)
{
	Campfire* pCampfire = (Campfire*)*api;
	delete pCampfire;
	return 0;
}

CAMPFIRE_EXTERN int CampfireLogin(CampfireAPI api, const char* pstrHost, const char* pstrAuthCode, int bUseSSL)
{
	std::string strHost(pstrHost), strAuthCode(pstrAuthCode);
	Campfire* pCampfire = (Campfire*)api;
   return pCampfire->Login(strHost, strAuthCode, bUseSSL == 1) ? 1 : 0;
}

CAMPFIRE_EXTERN int CampfireRoomsCount(CampfireAPI api, int& nCount)
{
	Campfire* pCampfire = (Campfire*)api;
	nCount = pCampfire->GetRoomCount();
	return 1;
}

CAMPFIRE_EXTERN int CampfireGetRoomNameID(CampfireAPI api, int nRoomIndex, char* pstrRoomName, int& nSizeOfRoomName, int& nID)
{
	std::string strRoomName;
   int id;
	Campfire* pCampfire = (Campfire*)api;
	bool bRet = pCampfire->GetRoomDetails(nRoomIndex, strRoomName, id);

	if( nSizeOfRoomName > 0 )
	{
		memcpy(pstrRoomName, strRoomName.c_str(), nSizeOfRoomName + 1);
	}
	else
	{
		nSizeOfRoomName = strRoomName.size();
	}

   nID = id;

	return bRet ? 1 : 0;
}

CAMPFIRE_EXTERN int CampfireJoinRoom(CampfireAPI api, int nRoomIndex)
{
	Campfire* pCampfire = (Campfire*)api;
	return pCampfire->JoinRoom(nRoomIndex) ? 1 : 0;
}

CAMPFIRE_EXTERN int CampfireSay(CampfireAPI api, const char* pstrMessage, int& nMessageID)
{
	std::string strMessage(pstrMessage);
	Campfire* pCampfire = (Campfire*)api;
	return pCampfire->Say(strMessage, nMessageID) ? 1 : 0;
}

CAMPFIRE_EXTERN int CampfirePaste(CampfireAPI api, const char* pstrMessage, int& nMessageID)
{
	std::string strMessage(pstrMessage);
	Campfire* pCampfire = (Campfire*)api;
	return pCampfire->Paste(strMessage, nMessageID) ? 1 : 0;
}

CAMPFIRE_EXTERN int CampfirePlaySound(CampfireAPI api, int nSound)
{
   Campfire* pCampfire = (Campfire*)api;
   return pCampfire->PlaySound((Sounds)nSound);
}

CAMPFIRE_EXTERN int CampfireUploadFile(CampfireAPI api, const char* pstrFilePath)
{
	std::string strFilePath(pstrFilePath);
	Campfire* pCampfire = (Campfire*)api;
	return pCampfire->UploadFile(strFilePath) ? 1 : 0;
}

CAMPFIRE_EXTERN int CampfireListen(CampfireAPI api)
{
	Campfire* pCampfire = (Campfire*)api;
	bool bRet = pCampfire->Listen();
	return bRet ? 1 : 0;
}

CAMPFIRE_EXTERN int CampfireGetListenMessage(CampfireAPI api, char* pstrMessage, int& nSizeOfMessage)
{
   Campfire* pCampfire = (Campfire*)api;
   std::string strListenMessage;
   bool bRet = pCampfire->GetListenMessage(strListenMessage);
   if( nSizeOfMessage > 0 )
   {
      memcpy(pstrMessage, strListenMessage.c_str(), min(nSizeOfMessage, (int)strListenMessage.size()) + 1);
   }
   else
   {
      nSizeOfMessage = strListenMessage.size();
   }

   return bRet ? 1 : 0;
}

CAMPFIRE_EXTERN int CampfireGetMessagesCount(CampfireAPI api, int& nCount)
{
	Campfire* pCampfire = (Campfire*)api;
	nCount = pCampfire->GetMessageCount();
	return 1;
}

CAMPFIRE_EXTERN int CampfireMessage(CampfireAPI api, int nIndex, int& nType, char* pstrMessage, int& nSizeOfMessage, int& nUserID)
{
	Campfire* pCampfire = (Campfire*)api;
	string strMessage, strPerson;
	pCampfire->GetMessage(nIndex, nType, strPerson, strMessage);

	if( nSizeOfMessage > 0 )
	{
		memcpy(pstrMessage, strMessage.c_str(), nSizeOfMessage + 1);
	}
	else
	{
		nSizeOfMessage = strMessage.size();
	}

   nUserID = atoi(strPerson.c_str());

	return 1;
}

CAMPFIRE_EXTERN int CampfireMessageClear(CampfireAPI api)
{
	Campfire* pCampfire = (Campfire*)api;
	pCampfire->ClearMessages();
	return 1;
}

CAMPFIRE_EXTERN int CampfireLeave(CampfireAPI api)
{
	Campfire* pCampfire = (Campfire*)api;
	return pCampfire->Leave() ? 1 : 0;
}

CAMPFIRE_EXTERN int CampfireChangeTopic(CampfireAPI api, const char* pstrTopic)
{
	std::string strTopic(pstrTopic);
	Campfire* pCampfire = (Campfire*)api;
	return pCampfire->SetRoomTopic(strTopic) ? 1 : 0;
}

CAMPFIRE_EXTERN int CampfireStarMessage(CampfireAPI api, int nMessageID)
{
   Campfire* pCampfire = (Campfire*)api;
   return pCampfire->StarMessage(nMessageID);
}

std::string IntToString(int nValue)
{
	ostringstream oss;
	oss << nValue;

	return oss.str();
}

RoomEntry::RoomEntry(const std::string& strName, int nID)
: m_strName(strName), m_nID(nID)
{
}

Base_Message::Base_Message(Type eType)
: m_eType(eType){}

Type Base_Message::GetType() const { return m_eType; }

void Base_Message::PutOnto(int& nType, std::string& strPerson, std::string& strMessage)
{
   nType = (int)m_eType;
   PutOnto(strPerson, strMessage);
}

Text_Message::Text_Message(const std::string& strMessage, int nPersonID)
: Base_Message(Text), m_strMessage(strMessage), m_nPersonID(nPersonID)
{
}

void Text_Message::PutOnto(std::string& strPerson, std::string& strMessage)
{
   strPerson = IntToString(m_nPersonID);
   strMessage = m_strMessage;
}

TimeStamp_Message::TimeStamp_Message(const std::string& strDate, const std::string& strTime)
: Base_Message(TimeStamp), m_strDate(strDate), m_strTime(strTime)
{
}

void TimeStamp_Message::PutOnto(std::string& strPerson, std::string& strMessage)
{
   strPerson = m_strDate;
   strMessage = m_strTime;
}

EnterLeave_Message::EnterLeave_Message(const std::string& strPerson, const std::string& strSubject)
: Base_Message(EnterLeave), m_strPerson(strPerson), m_strSubject(strSubject)
{
}

void EnterLeave_Message::PutOnto(std::string& strPerson, std::string& strMessage)
{
   strPerson = m_strPerson;
   strMessage = m_strSubject;
}

Upload_Message::Upload_Message(const std::string& strPerson, const std::string& strSubject)
: Base_Message(Upload), m_strPerson(strPerson), m_strURL(strSubject)
{
}

void Upload_Message::PutOnto(std::string& strPerson, std::string& strURL)
{
   strPerson = m_strPerson;
   strURL = m_strURL;
}

Messages::~Messages()
{
   Clear();
}

void Messages::AddTextMessage(const std::string& strMessage, int nUserID)
{
   m_Messages.push_back(new Text_Message(strMessage, nUserID));
}

void Messages::AddTimeStamp(const std::string& strDate, const std::string& strTime)
{
   m_Messages.push_back(new TimeStamp_Message(strDate, strTime));
}

void Messages::AddEnterLeaveMessage(const std::string& strPerson, const std::string& strBody)
{
   m_Messages.push_back(new EnterLeave_Message(strPerson, strBody));
}

void Messages::AddUploadFile(const std::string& strPerson, const std::string& strURL)
{
   m_Messages.push_back(new Upload_Message(strPerson, strURL));
}

int Messages::Count() const
{
   return m_Messages.size();
}

void Messages::Clear()
{
   for(std::vector<Base_Message*>::size_type i=0; i<m_Messages.size(); i++)
   {
      delete m_Messages[i];
   }

   m_Messages.clear();
}

void Messages::PutMessageOnto(int nIndex, int& nType, std::string& strPerson, std::string& strMessage)
{
   m_Messages[nIndex]->PutOnto(nType, strPerson, strMessage);
}

Campfire::Campfire()
: 
#ifdef WIN32
   m_mutexListen(PTHREAD_MUTEX_INITIALIZER),
#endif
   m_bExit(false), m_nLastMessageID(0)
{
#ifndef WIN32
   pthread_mutex_init(&m_mutexListen, NULL);
#endif
   m_threadListen = pthread_self();
	curl_global_init(CURL_GLOBAL_ALL);
}

Campfire::~Campfire()
{
   pthread_mutex_lock( &m_mutexListen );
   m_bExit = true;
   pthread_mutex_unlock( &m_mutexListen );

   pthread_join( m_threadListen, NULL);

   SAFE_DELETE(m_pRest);

	curl_global_cleanup();
}

bool Campfire::Login(const std::string& strHost, const std::string& strAuthCode, bool bUseSSL)
{
   m_strAuthCode = strAuthCode;

   RestClientFactory factory;
   m_pRest = factory.SetUsernamePassword(strAuthCode, "x").SetVerbosity(false).CreateRestClient();

   std::string strAddress = GetURL(strHost, bUseSSL);
   strAddress += "account.xml";

   RestClient::response q = m_pRest->get(strAddress);

   //TODO: check if succeeded or not

	//Find out list of rooms
   strAddress = GetURL(strHost, bUseSSL);
   strAddress += "rooms.xml";

   RestClient::response r = m_pRest->get(strAddress);

   //cout << "r.body: " << r.body << endl;

   int nPos = 0;
   while(true)
   {
      int nStartID = r.body.find("<id type=\"integer\">", nPos);
      if( nStartID == string::npos )
         break;

      int nEndID = r.body.find("</id>", nStartID);
      if( nEndID == string::npos )
         break;

      std::string strID = r.body.substr(nStartID + strlen("<id type=\"integer\">"), 
         nEndID - nStartID - strlen("<id type=\"integer\">") );

      int nID = atoi(strID.c_str());

      int nStartRoomName = r.body.find("<name>", nPos);
      if( nStartRoomName == string::npos )
         break;

      int nEndRoomName = r.body.find("</name>", nStartRoomName);
      if( nEndRoomName == string::npos )
         break;

      std::string strRoomName = r.body.substr(nStartRoomName + strlen("<name>"), 
         nEndRoomName - nStartRoomName - strlen("<name>") );

      nPos = nEndRoomName;

      m_RoomList.push_back(RoomEntry(strRoomName, nID));

      //TODO: Find out users in room:
   }

   m_strHost = strHost;
   m_bUseSSL= bUseSSL;

	return true;
}

int Campfire::GetRoomCount()
{
	return m_RoomList.size();
}

bool Campfire::GetRoomDetails(int nRoomIndex, std::string& strRoomName, int& id)
{
	if( nRoomIndex >= GetRoomCount() )
		return false;

	strRoomName = m_RoomList[nRoomIndex].m_strName;
   id = m_RoomList[nRoomIndex].m_nID;

	return true;
}

bool Campfire::JoinRoom(int nRoomIndex)
{
	if( nRoomIndex >= GetRoomCount() )
		return false;

	string strRoomName;
   int nID = 0;
	GetRoomDetails(nRoomIndex, strRoomName, nID);

   std::string strAddress = GetURL(m_strHost, m_bUseSSL);
   strAddress += "room/";
   strAddress += IntToString(nID);
   std::string strRoomURL = strAddress;
   strAddress += "/join.xml";

   RestClient::response r = m_pRest->post(strAddress, "application/xml", std::string());

   m_nLastMessageID = 0;

   //TODO: Check for error

   //TODO: Get last chat messages

   m_nRoomNum = nID;

	return true;
}

bool Campfire::Say(const std::string& strMessage, int& nMessageID)
{
	return SayPaste(strMessage, SayMessage, nMessageID);
}

bool Campfire::Paste(const std::string& strMessage, int& nMessageID)
{
	return SayPaste(strMessage, PasteMessage, nMessageID);
}

bool Campfire::PlaySound(Sounds eSound)
{
	string strData;
	switch( eSound )
   {
   default:
      //RASSERT(false);
   case Crickets:
      strData += "crickets";
      break;
   case Rimshot:
      strData += "rimshot";
      break;
   case Trombone:
      strData += "trombone";
      break;
   case SOUND_WHOOMP:
      strData += "whoomp";
      break;
   case SOUND_LOGGINS:
      strData += "loggins";
      break;
   case SOUND_BUELLER:
      strData += "bueller";
      break;
   case SOUND_YODEL:
      strData += "yodel";
      break;
   case SOUND_TADA:
      strData += "tada";
      break;
   case SOUND_HORROR:
      strData += "horror";
      break;
   case SOUND_TMYK:
      strData += "tmyk";
      break;
   case SOUND_OHYEAH:
      strData += "ohyeah";
      break;
   case SOUND_HORN:
      strData += "horn";
      break;
   case SOUND_NOOOO:
      strData += "noooo";
      break;
   case SOUND_LIVE:
      strData += "live";
      break;
   case SOUND_HEYGIRL:
      strData += "heygirl";
      break;
   case SOUND_OHMY:
      strData += "ohmy";
      break;
   case SOUND_GREATJOB:
      strData += "greatjob";
      break;
   case SOUND_YEAH:
      strData += "yeah";
      break;
   case SOUND_WHAT:
      strData += "what";
      break;
   case SOUND_INCONCEIVABLE:
      strData += "inconceivable";
      break;
   case SOUND_DANGERZONE:
      strData += "dangerzone";
      break;
   case SOUND_SECRET:
      strData += "secret";
      break;
   case SOUND_PUSHIT:
      strData += "pushit";
      break;
   case SOUND_DRAMA:
      strData += "drama";
      break;
   case SOUND_VUVUZELA:
      strData += "vuvuzela";
      break;
   case SOUND_SAX:
      strData += "sax";
      break;
   case SOUND_56K:
      strData += "56k";
      break;
   case SOUND_TROLOLO:
      strData += "trololo";
      break;
   case SOUND_MAKEITSO:
      strData += "makeitso";
      break;
   case SOUND_BELL:
      strData += "bell";
      break;
   case SOUND_CLOWNTOWN:
      strData += "clowntown";
      break;
   case SOUND_BEZOS:
      strData += "bezos";
      break;
   }
   int nMessageID;
   return SayPaste(strData, SoundMessage, nMessageID);
}

bool Campfire::UploadFile(const std::string& strFilePath)
{
	struct curl_httppost *formpost=NULL;
	struct curl_httppost *lastptr=NULL;

	std::string strAddress = GetRoomURL(m_strHost, m_nRoomNum, m_bUseSSL) + "/uploads.xml";

   CURL* pCurl = m_pRest->ConstructLibCurl();

	curl_easy_setopt(pCurl, CURLOPT_URL, strAddress.c_str());

	curl_formadd(&formpost,
				&lastptr,
				CURLFORM_COPYNAME, "upload",
				CURLFORM_FILE, strFilePath.c_str(),
				CURLFORM_END);

	curl_formadd(&formpost,
				&lastptr,
				CURLFORM_COPYNAME, "submit",
				CURLFORM_COPYCONTENTS, "Upload",
				CURLFORM_END);

	curl_easy_setopt(pCurl, CURLOPT_HTTPPOST, formpost);

	struct curl_slist *pHeader = curl_slist_append(NULL, "X-Requested-With: XMLHttpRequest");

	curl_easy_setopt(pCurl, CURLOPT_HTTPHEADER, pHeader);

   CURLcode res = curl_easy_perform(pCurl);

   curl_formfree(formpost);
   curl_slist_free_all(pHeader);
   curl_easy_cleanup(pCurl);

	return false;
}

bool Campfire::ClearMessages()
{
   m_Messages.Clear();
   return true;
}

int Campfire::GetMessageCount()
{
   return m_Messages.Count();
}

bool Campfire::GetMessage(int nIndex, int& nType, std::string& strPerson, std::string& strMessage)
{
   m_Messages.PutMessageOnto(nIndex, nType, strPerson, strMessage);
   return true;
}

bool Campfire::Listen()
{
   //TODO: Check if already started thread!

   int iRet;
   iRet = pthread_create( &m_threadListen, NULL, Campfire::ListenThread, (void*)this);

   return true;
}

bool Campfire::GetListenMessage(std::string& strMessage)
{
   bool bRet = false;
   pthread_mutex_lock( &m_mutexListen );
   if( m_aListenMessages.size() > 0 )
   {
      strMessage = m_aListenMessages[0];
      m_aListenMessages.erase(m_aListenMessages.begin());
      bRet = true;
   }
   pthread_mutex_unlock( &m_mutexListen );

   return bRet;
}

bool Campfire::Leave()
{
	std::string strAddress = GetRoomURL(m_strHost, m_nRoomNum, m_bUseSSL);
	strAddress += "/leave.xml";

   RestClient::response r = m_pRest->post(strAddress, "application/xml", std::string());

   return true;
}

bool Campfire::SetRoomTopic(const std::string& strTopic)
{
   std::string strAddress = GetRoomURL(m_strHost, m_nRoomNum, m_bUseSSL);
   strAddress += ".xml";

   std::string strData = "<room><topic>" + strTopic + "</topic></room>";

   RestClient::response r = m_pRest->put(strAddress, "application/xml", strData);

   return true;
}

bool Campfire::StarMessage(int nMessageID)
{
   std::string strAddress = GetURL(m_strHost, m_bUseSSL) + "messages/";
   strAddress += IntToString(nMessageID);
   strAddress += "/star.xml";

   RestClient::response r = m_pRest->post(strAddress, "application/xml", std::string());

   return true;
}

bool Campfire::SayPaste(const std::string& strMessage, MessageType eType, int& nMessageID)
{
	std::string strAddress = GetRoomURL(m_strHost, m_nRoomNum, m_bUseSSL);
	strAddress += "/speak.xml";

   std::string strData = "<message><type>";
   switch(eType)
   {
   default:
   case SayMessage:
      strData += "TextMessage";
      break;
   case PasteMessage:
      strData += "PasteMessage";
      break;
   case SoundMessage:
      strData += "SoundMessage";
      break;
   }
   strData += "</type><body>" + strMessage + "</body></message>";

   RestClient::response r = m_pRest->post(strAddress, "application/xml", strData.c_str());

   int nStartID = r.body.find("<id type=\"integer\">");
   if( nStartID == std::string::npos )
      return false;
   int nEndID = r.body.find("</id>", nStartID);
   if( nEndID == std::string::npos )
      return false;

   std::string strIDLine = r.body.substr(nStartID + strlen("<id type=\"integer\">"), nEndID-nStartID-strlen("<id type=\"integer\">"));
   nMessageID = atoi(strIDLine.c_str());

	return true;
}

std::string Campfire::GetURL(const std::string& strHost, bool bUseSSL)
{
	string strReturn;

	if( bUseSSL )
	{
		strReturn += "https://";
	}
	else
	{
		strReturn += "http://";
	}

	strReturn += strHost;

	strReturn += ".campfirenow.com/";

	return strReturn;
}

std::string Campfire::GetRoomURL(const std::string& strHost, int nRoomNum, bool bUseSSL)
{
   return GetURL(strHost, bUseSSL) + "room/" + IntToString(nRoomNum);
}

std::string Campfire::GetStreamingURL(int nRoomNum, bool bUseSSL)
{
   string strReturn;

   if( bUseSSL )
   {
      strReturn += "https://";
   }
   else
   {
      strReturn += "http://";
   }

   strReturn += "streaming.campfirenow.com/room/";
   strReturn += IntToString(nRoomNum);
   strReturn += "/live.json";

   return strReturn;
}

string GetNonTagData(const std::string& strLine)
{
	string strReturn = strLine;
	while(true)
	{
		int nStart = strReturn.find("<");
		int nEnd = strReturn .find(">");
		if( nStart == string::npos || nEnd == string::npos ) break;
		//assert(nEnd > nStart);

		strReturn.replace(nStart, nEnd - nStart + 1, "");
	}
	return strReturn;
}

bool Campfire::ParseListenResponse(const std::string& strListenResponse)
{
	int nPos = -1;
	while(true)
	{
		nPos = strListenResponse.find("<message>", nPos+1);
		if( nPos == string::npos )
			break;

		int nEndEntry = strListenResponse.find("</message>", nPos);
		if( nEndEntry == string::npos )
			break;

		string strEntry = strListenResponse.substr(nPos, nEndEntry - nPos );

		int nClassPos = strEntry.find("<type>");
		if( nClassPos == string::npos )
			break;

		int nEndClassPos = strEntry.find("</type>", nClassPos);
		if( nEndClassPos == string::npos )
			break;

		string strClassType = strEntry.substr(nClassPos + strlen("<type>"), nEndClassPos - nClassPos - strlen("<type>"));

      int nIDPos = strEntry.find("<id type=\"integer\">");
      if( nIDPos == string::npos )
         break;

      int nEndIDPos = strEntry.find("</id>", nIDPos);
      if( nEndIDPos == string::npos )
         break;

      string strID = strEntry.substr(nIDPos + strlen("<id type=\"integer\">"), nEndIDPos - nIDPos - strlen("<id type=\"integer\">") );
      int nID = atoi(strID.c_str());
      if( nID > m_nLastMessageID )
         m_nLastMessageID = nID+1;

		if( strClassType == "TextMessage" )
		{
			int nMessageStart = strEntry.find("<body>");
			int nMessageEnd = strEntry.find("</body>", nMessageStart+strlen("<body>"));
			if( nMessageStart == string::npos || nMessageEnd == string::npos || nMessageStart >= nMessageEnd )
				continue;
			string strMessage = strEntry.substr(nMessageStart + strlen("<body>"), nMessageEnd - nMessageStart - strlen("<body>"));

         int nUserStart = strEntry.find("<user-id type=\"integer\">");
         int nUserEnd = strEntry.find("</user-id>", nUserStart+strlen("<user-id type=\"integer\">"));
         if( nUserStart == string::npos || nUserEnd == string::npos || nUserStart >= nUserEnd )
            continue;
         string strUser = strEntry.substr(nUserStart + strlen("<user-id type=\"integer\">"), nUserEnd - nUserStart - strlen("<user-id type=\"integer\">"));
         int nUserID = atoi(strUser.c_str());

			///*int nUserStart = strEntry.find("user_");
			//int nUserEnd = strEntry.find("\\\"", nUserStart+strlen("user_"));
			//if( nUserStart == string::npos || nUserEnd == string::npos || nUserStart >= nUserEnd )
			//	continue;
			//string strUser = strEntry.substr(nUserStart + strlen("user_"), nUserEnd - nUserStart - strlen("user_"));

			//int nPersonStart = strEntry.find("class=\\\"author\\\"\\u003E");
			//int nPersonEnd = strEntry.find("\\u003C/span\\u003E\\u003C/td\\u003E\\n", nPersonStart+strlen("class=\\\"author\\\"\\u003E"));
			//if( nPersonStart == string::npos || nPersonEnd == string::npos || nPersonStart >= nPersonEnd )
			//	continue;
			//string strPerson = strEntry.substr(nPersonStart + strlen("class=\\\"author\\\"\\u003E"), nPersonEnd - nPersonStart - strlen("class=\\\"author\\\"\\u003E"));

			//int nBodyStart = strEntry.find("\\u003Cdiv class=\\\"body\\\"\\u003E");
			//int nBodyEnd = strEntry.find("\\u003C/div\\u003E\\n", nBodyStart+strlen("\\u003Cdiv class=\\\"body\\\"\\u003E"));
			//if( nBodyStart == string::npos || nBodyEnd == string::npos || nBodyStart >= nBodyEnd )
			//	continue;
			//string strBody = strEntry.substr(nBodyStart + strlen("\\u003Cdiv class=\\\"body\\\"\\u003E"), nBodyEnd - nBodyStart - strlen("\\u003Cdiv class=\\\"body\\\"\\u003E"));*/

         m_Messages.AddTextMessage(strMessage, nUserID);
		}
		/*else if( strClassType == "timestamp_message" )
		{
			int nMessageStart = strEntry.find("id=\\\"message_");
			int nMessageEnd = strEntry.find("\\\"", nMessageStart+strlen("id=\\\"message_"));
			if( nMessageStart == string::npos || nMessageEnd == string::npos || nMessageStart >= nMessageEnd )
				continue;
			string strMessage = strEntry.substr(nMessageStart + strlen("id=\\\"message_"), nMessageEnd - nMessageStart - strlen("id=\\\"message_"));

			int nDateStart = strEntry.find("class=\\\"author\\\"\\u003E");
			int nDateEnd = strEntry.find("\\u003C/span\\u003E\\u003C/td\\u003E\\n", nDateStart+strlen("class=\\\"author\\\"\\u003E"));
			if( nDateStart == string::npos || nDateEnd == string::npos || nDateStart >= nDateEnd )
				continue;
			string strDate = strEntry.substr(nDateStart + strlen("class=\\\"author\\\"\\u003E"), nDateEnd - nDateStart - strlen("class=\\\"author\\\"\\u003E"));

			int nTimeStart = strEntry.find("class=\\\"body\\\"\\u003E");
			int nTimeEnd = strEntry.find("\\u003C", nTimeStart+strlen("class=\\\"body\\\"\\u003E"));
			if( nTimeStart == string::npos || nTimeEnd == string::npos || nTimeStart >= nTimeEnd )
				continue;
			string strTime = strEntry.substr(nTimeStart + strlen("class=\\\"body\\\"\\u003E"), nTimeEnd - nTimeStart - strlen("class=\\\"body\\\"\\u003E"));
         m_Messages.AddTimeStamp(strDate, strTime);
		}
		else if( strClassType == "kick_message" )
		{
			int nMessageStart = strEntry.find("id=\\\"message_");
			int nMessageEnd = strEntry.find("\\\"", nMessageStart+strlen("id=\\\"message_"));
			if( nMessageStart == string::npos || nMessageEnd == string::npos || nMessageStart >= nMessageEnd )
				continue;
			string strMessage = strEntry.substr(nMessageStart + strlen("id=\\\"message_"), nMessageEnd - nMessageStart - strlen("id=\\\"message_"));

			int nPersonStart = strEntry.find("class=\\\"person\\\"\\u003E");
			int nPersonEnd = strEntry.find("\\u003C/td\\u003E\\n", nPersonStart+strlen("class=\\\"person\\\"\\u003E"));
			if( nPersonStart == string::npos || nPersonEnd == string::npos || nPersonStart >= nPersonEnd )
				continue;
			string strPerson = strEntry.substr(nPersonStart + strlen("class=\\\"person\\\"\\u003E"), nPersonEnd - nPersonStart - strlen("class=\\\"person\\\"\\u003E"));

			int nBodyStart = strEntry.find("class=\\\"body\\\"\\u003E\\u003Cdiv\\u003E");
			int nBodyEnd = strEntry.find("\\u003C/div\\u003E\\u003C/td\\u003E\\n\\u003C/tr\\u003E\\n", nBodyStart+strlen("class=\\\"body\\\"\\u003E\\u003Cdiv\\u003E"));
			if( nBodyStart == string::npos || nBodyEnd == string::npos || nBodyStart >= nBodyEnd )
				continue;
			string strBody = strEntry.substr(nBodyStart + strlen("class=\\\"body\\\"\\u003E\\u003Cdiv\\u003E"), nBodyEnd - nBodyStart - strlen("class=\\\"body\\\"\\u003E\\u003Cdiv\\u003E"));

         m_Messages.AddEnterLeaveMessage(strPerson, strBody);
		}
		else if( strClassType == "leave_message" )
		{
			int nMessageStart = strEntry.find("id=\\\"message_");
			int nMessageEnd = strEntry.find("\\\"", nMessageStart+strlen("id=\\\"message_"));
			if( nMessageStart == string::npos || nMessageEnd == string::npos || nMessageStart >= nMessageEnd )
				continue;
			string strMessage = strEntry.substr(nMessageStart + strlen("id=\\\"message_"), nMessageEnd - nMessageStart - strlen("id=\\\"message_"));

			int nPersonStart = strEntry.find("class=\\\"person\\\"\\u003E");
			int nPersonEnd = strEntry.find("\\u003C/td\\u003E\\n", nPersonStart+strlen("class=\\\"person\\\"\\u003E"));
			if( nPersonStart == string::npos || nPersonEnd == string::npos || nPersonStart >= nPersonEnd )
				continue;
			string strPerson = strEntry.substr(nPersonStart + strlen("class=\\\"person\\\"\\u003E"), nPersonEnd - nPersonStart - strlen("class=\\\"person\\\"\\u003E"));

			int nBodyStart = strEntry.find("class=\\\"body\\\"\\u003E\\u003Cdiv\\u003E");
			int nBodyEnd = strEntry.find("\\u003C/div\\u003E\\u003C/td\\u003E\\n\\u003C/tr\\u003E", nBodyStart+strlen("class=\\\"body\\\"\\u003E\\u003Cdiv\\u003E"));
			if( nBodyStart == string::npos || nBodyEnd == string::npos || nBodyStart >= nBodyEnd )
				continue;
			string strBody = strEntry.substr(nBodyStart + strlen("class=\\\"body\\\"\\u003E\\u003Cdiv\\u003E"), nBodyEnd - nBodyStart - strlen("class=\\\"body\\\"\\u003E\\u003Cdiv\\u003E"));

         m_Messages.AddEnterLeaveMessage(strPerson, strBody);
		}
      else if( strClassType == "enter_message" )
		{
			int nMessageStart = strEntry.find("id=\\\"message_");
			int nMessageEnd = strEntry.find("\\\"", nMessageStart+strlen("id=\\\"message_"));
			if( nMessageStart == string::npos || nMessageEnd == string::npos || nMessageStart >= nMessageEnd )
				continue;
			string strMessage = strEntry.substr(nMessageStart + strlen("id=\\\"message_"), nMessageEnd - nMessageStart - strlen("id=\\\"message_"));

			int nPersonStart = strEntry.find("class=\\\"person\\\"\\u003E");
			int nPersonEnd = strEntry.find("\\u003C/td\\u003E\\n", nPersonStart+strlen("class=\\\"person\\\"\\u003E"));
			if( nPersonStart == string::npos || nPersonEnd == string::npos || nPersonStart >= nPersonEnd )
				continue;
			string strPerson = strEntry.substr(nPersonStart + strlen("class=\\\"person\\\"\\u003E"), nPersonEnd - nPersonStart - strlen("class=\\\"person\\\"\\u003E"));

			int nBodyStart = strEntry.find("class=\\\"body\\\"\\u003E\\u003Cdiv\\u003E");
			int nBodyEnd = strEntry.find("\\u003C/div\\u003E\\u003C/td\\u003E\\n\\u003C/tr\\u003E", nBodyStart+strlen("class=\\\"body\\\"\\u003E\\u003Cdiv\\u003E"));
			if( nBodyStart == string::npos || nBodyEnd == string::npos || nBodyStart >= nBodyEnd )
				continue;
			string strBody = strEntry.substr(nBodyStart + strlen("class=\\\"body\\\"\\u003E\\u003Cdiv\\u003E"), nBodyEnd - nBodyStart - strlen("class=\\\"body\\\"\\u003E\\u003Cdiv\\u003E"));

         m_Messages.AddEnterLeaveMessage(strPerson, strBody);
		}
      else if( strClassType == "upload_message" )
		{
			int nMessageStart = strEntry.find("id=\\\"message_");
			int nMessageEnd = strEntry.find("\\\"", nMessageStart+strlen("id=\\\"message_"));
			if( nMessageStart == string::npos || nMessageEnd == string::npos || nMessageStart >= nMessageEnd )
				continue;
			string strMessage = strEntry.substr(nMessageStart + strlen("id=\\\"message_"), nMessageEnd - nMessageStart - strlen("id=\\\"message_"));

			int nPersonStart = strEntry.find("class=\\\"author\\\"\\u003E");
			int nPersonEnd = strEntry.find("\\u003C/span\\u003E\\u003C/td\\u003E\\n", nPersonStart+strlen("class=\\\"author\\\"\\u003E"));
			if( nPersonStart == string::npos || nPersonEnd == string::npos || nPersonStart >= nPersonEnd )
				continue;
			string strPerson = strEntry.substr(nPersonStart + strlen("class=\\\"author\\\"\\u003E"), nPersonEnd - nPersonStart - strlen("class=\\\"author\\\"\\u003E"));

			int nBodyStart = strEntry.find("\\u003Ca href=\\\"");
			int nBodyEnd = strEntry.find("\\\"", nBodyStart+strlen("\\u003Ca href=\\\""));
			if( nBodyStart == string::npos || nBodyEnd == string::npos || nBodyStart >= nBodyEnd )
				continue;
			string strBody = strEntry.substr(nBodyStart + strlen("\\u003Ca href=\\\""), nBodyEnd - nBodyStart - strlen("\\u003Ca href=\\\""));

         m_Messages.AddUploadFile(strPerson, strBody);
		}
		else if( strClassType == "advertisement_message" )
		{
			int nMessageStart = strEntry.find("id=\\\"message_");
			int nMessageEnd = strEntry.find("\\\"", nMessageStart+strlen("id=\\\"message_"));
			if( nMessageStart == string::npos || nMessageEnd == string::npos || nMessageStart >= nMessageEnd )
				continue;
			string strMessage = strEntry.substr(nMessageStart + strlen("id=\\\"message_"), nMessageEnd - nMessageStart - strlen("id=\\\"message_"));

			int nFromStart = strEntry.find("\\u003Ctd class=\\\"person\\\"\\u003E");
			int nFromEnd = strEntry.find("\\u003C/td\\u003E\\n", nFromStart+strlen("\\u003Ctd class=\\\"person\\\"\\u003E"));
			if( nFromStart == string::npos || nFromEnd == string::npos || nFromStart >= nFromEnd )
				continue;
			string strFrom = strEntry.substr(nFromStart + strlen("\\u003Ctd class=\\\"person\\\"\\u003E"), nFromEnd - nFromStart - strlen("\\u003Ctd class=\\\"person\\\"\\u003E"));
		}*/
		else
		{
		}
	}

	return true;
}

int is_empty_str(const char *s) {
   while (*s != '\0') {
      if (!isspace(*s))
         return 0;
      s++;
   }
   return 1;
}

size_t Campfire::listen_callback(void *ptr, size_t size, size_t nmemb, void *userdata)
{
   Campfire* pThis = (Campfire*)userdata;
   if( pThis->m_bExit )
      return 0;

   const char* pstr = reinterpret_cast<char*>(ptr);

   if( !is_empty_str(pstr) )
   {
      pthread_mutex_lock( &pThis->m_mutexListen );
      pThis->m_aListenMessages.push_back(pstr);
      pthread_mutex_unlock( &pThis->m_mutexListen );
      //cout << "Message: " << pstr << endl;
   }

   return (size * nmemb);
}

void* Campfire::ListenThread(void* ptr)
{
   Campfire* pThis = (Campfire*)ptr;
   pThis->ListenWorker();

   return NULL;
}

void Campfire::ListenWorker()
{
   void******* m_pRest = NULL;//This is a thread; so don't use m_pRest!

   std::string strAddress = GetStreamingURL(m_nRoomNum, m_bUseSSL);

   RestClientFactory factory;
   RestClient* pClient = factory.SetUsernamePassword(m_strAuthCode, "x").SetVerbosity(false).CreateRestClient();
   pClient->SetCallback(Campfire::listen_callback);
   pClient->SetUserData((void*)this);
   RestClient::response r = pClient->get(strAddress);
   SAFE_DELETE(pClient);//Done with this so no seem to return the callback/userdata
}

