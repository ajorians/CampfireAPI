#include <iostream>
#include <string>
#include <fstream>
#include "CampfireAPI.h"
#include "Library.h"

#ifdef WIN32
#include <windows.h>
#endif

using namespace std;

int main(int argc, char *argv[])
{
   bool bUseSSL = false, bListen = false, bPaste = false, bStarMessage = false, bLeave = true;
   string strCamp, strRoom, strAuthCode, strUploadFile, strMessage, strTopic, strOutputTranscriptFile;
   for(int i=0; i<argc; i++)
   {
      string str = argv[i];
      if( str == "--camp" )
         strCamp = argv[i+1];

      if( str == "--room" )
         strRoom = argv[i+1];

      if( str == "--auth" )
         strAuthCode = argv[i+1];

      if( str == "--upload" )
         strUploadFile = argv[i+1];

      if( str == "--say" )
         strMessage = argv[i+1];

      if( str == "--paste" )
         bPaste = true;

      if( str == "--star" )
         bStarMessage = true;

      if( str == "--noleave" )
         bLeave = false;

      if( str == "--settopic" )
         strTopic = argv[i+1];

      if( str == "--ssl" )
         bUseSSL = true;

      if( str == "--outputtranscript" )
         strOutputTranscriptFile = argv[i+1];

      if( str == "--listen" )
         bListen = true;
   }
   if( argc < 2 || strCamp.length() == 0 || strAuthCode.length() == 0 )
   {
      cout << "Usage: " << argv[0] << " --camp testwith --auth blahblahblah" << endl;
      cout << "--Room RoomName --upload file or --say message" << endl;
      return 0;
   }

   RLibrary library(
#ifdef WIN32
      "..\\..\\CampfireAPI\\Debug\\CampfireAPI.dll"
#else
      "/home/aorians/Scripts/Build/CampfireAPI/libCampfireAPI.so"
#endif
      );

   if( !library.Load() )
      return 0;

   CampfireCreateFunc CreateAPI = (CampfireCreateFunc)library.Resolve("CampfireCreate");
   if( !CreateAPI )
      return 0;

   CampfireAPI pCampfire = NULL;
   CreateAPI(&pCampfire);

   CampfireLoginFunc Login = (CampfireLoginFunc)library.Resolve("CampfireLogin");
   if( !Login )
      return 0;

   bool bLoggedOn = Login(pCampfire, strCamp.c_str(), strAuthCode.c_str(), bUseSSL) == 1;

   CampfireRoomsCountFunc GetRoomCount = (CampfireRoomsCountFunc)library.Resolve("CampfireRoomsCount");
   if( !GetRoomCount )
      return 0;

   int nRoomCount = -1;
   GetRoomCount(pCampfire, nRoomCount);

   ///

   CampfireGetRoomNameIDFunc GetRoomName = (CampfireGetRoomNameIDFunc)library.Resolve("CampfireGetRoomNameID");
   if( !GetRoomName )
      return 0;

   int nRoomID;

   int nRoomNum = 0;
   for(nRoomNum=0; nRoomNum<nRoomCount; nRoomNum++)
   {
      char * pstrRoomName;
      pstrRoomName = NULL;

      int nSizeOfRoomName = 0;
      GetRoomName(pCampfire, nRoomNum, pstrRoomName, nSizeOfRoomName, nRoomID);

      pstrRoomName = new char[nSizeOfRoomName + 1];

      GetRoomName(pCampfire, nRoomNum, pstrRoomName, nSizeOfRoomName, nRoomID);

      std::string strRoomName(pstrRoomName);

      delete[] pstrRoomName;

      cout << "Room: " << strRoomName << endl;

      if( strRoomName == strRoom )
         break;
   }

   if( nRoomNum >= nRoomCount )
   {
      cout << "Room not found" << endl;
      return 0;
   }

   ///

   CampfireJoinRoomFunc JoinRoom = (CampfireJoinRoomFunc)library.Resolve("CampfireJoinRoom");
   if( !JoinRoom )
      return 0;

   JoinRoom(pCampfire, nRoomNum);

   ///

   if( strMessage.length() > 0 )
   {
      int nMessageID = -1;
      if( bPaste )
      {
         CampfirePasteFunc Paste = (CampfirePasteFunc)library.Resolve("CampfirePaste");
         if( !Paste )
            return 0;

         Paste(pCampfire, strMessage.c_str(), nMessageID);
      }
      else
      {
         CampfireSayFunc Say = (CampfireSayFunc)library.Resolve("CampfireSay");
         if( !Say )
            return 0;

         Say(pCampfire, strMessage.c_str(), nMessageID);
      }

      if( bStarMessage )
      {
         CampfireStarMessageFunc Star = (CampfireStarMessageFunc)library.Resolve("CampfireStarMessage");
         if( !Star )
            return 0;

         Star(pCampfire, nMessageID);
      }
   }

   ///

   if( strTopic.length() > 0 )
   {
      CampfireChangeTopicFunc SetTopic = (CampfireChangeTopicFunc)library.Resolve("CampfireChangeTopic");
      if( !SetTopic )
         return 0;

      SetTopic(pCampfire, strTopic.c_str());
   }

   if( strUploadFile.length() > 0 )
   {
      CampfireUploadFileFunc Upload = (CampfireUploadFileFunc)library.Resolve("CampfireUploadFile");
      if( !Upload )
         return 0;

      Upload(pCampfire, strUploadFile.c_str());
   }

   if( bListen )
   {
      CampfireListenFunc Listen = (CampfireListenFunc)library.Resolve("CampfireListen");
      if( !Listen )
         return 0;

      CampfireGetListenMessageFunc GetListenMessage = (CampfireGetListenMessageFunc)library.Resolve("CampfireGetListenMessage");
      if( !GetListenMessage )
         return 0;

      Listen(pCampfire);

      char strBuffer[1024*100];
      while( true )
      {
         int nSizeOfMessage = 1024*100;
         bool bHasMessage = GetListenMessage(pCampfire, strBuffer, nSizeOfMessage) == 1;

         if( bHasMessage )
         {
            cout << "Message: " << strBuffer << endl;
         }

#ifdef _WIN32
         Sleep(200);
#else
         usleep(200*1000);
#endif
      }
   }

   if( bLeave )
   {
      CampfireLeaveFunc Leave = (CampfireLeaveFunc)library.Resolve("CampfireLeave");
      if( !Leave )
         return 0;

      Leave(pCampfire);
   }

   ///

   CampfireFreeFunc FreeAPI = (CampfireFreeFunc)library.Resolve("CampfireFree");
   if( !FreeAPI )
      return 0;

   FreeAPI(&pCampfire);

   return 0;
}

