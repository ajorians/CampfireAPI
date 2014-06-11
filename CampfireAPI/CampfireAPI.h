#ifndef CAMPFIREAPI_CAMPFIREAPI_H
#define CAMPFIREAPI_CAMPFIREAPI_H

#ifdef WIN32
#define CAMPFIRE_EXTERN	extern "C" __declspec(dllexport)
#else
#define CAMPFIRE_EXTERN extern "C"
#endif

typedef void*	CampfireAPI;

#define MESSAGE_TYPE_TEXT        0x1
#define MESSAGE_TYPE_TIMESTAMP   0x2
#define MESSAGE_TYPE_ENTERLEAVE  0x3
#define MESSAGE_TYPE_UPLOAD      0x4

#define SOUND_CRICKETS           0x01
#define SOUND_RIMSHOT            0x02
#define SOUND_TROMBONE           0x03
#define SOUND_WHOOMP             0x04
#define SOUND_LOGGINS            0x05
#define SOUND_BUELLER            0x06
#define SOUND_YODEL              0x07
#define SOUND_TADA               0x08
#define SOUND_HORROR             0x09
#define SOUND_TMYK               0x0A
#define SOUND_OHYEAH             0x0B
#define SOUND_HORN               0x0C
#define SOUND_NOOOO              0x0D
#define SOUND_LIVE               0x0E
#define SOUND_HEYGIRL            0x10
#define SOUND_OHMY               0x11
#define SOUND_GREATJOB           0x12
#define SOUND_YEAH               0x13
#define SOUND_WHAT               0x14
#define SOUND_INCONCEIVABLE      0x15
#define SOUND_DANGERZONE         0x16
#define SOUND_SECRET             0x17
#define SOUND_PUSHIT             0x18
#define SOUND_DRAMA              0x19
#define SOUND_VUVUZELA           0x1A
#define SOUND_SAX                0x1B
#define SOUND_56K                0x1C
#define SOUND_TROLOLO            0x1D
#define SOUND_MAKEITSO           0x1E
#define SOUND_BELL               0x1F
#define SOUND_CLOWNTOWN          0x20
#define SOUND_BEZOS              0x21

typedef int (*CampfireCreateFunc)(CampfireAPI* api);
typedef int (*CampfireFreeFunc)(CampfireAPI* api);
typedef int (*CampfireLoginFunc)(CampfireAPI api, const char* pstrHost, const char* pstrAuthCode, int bUseSSL);
typedef int (*CampfireRoomsCountFunc)(CampfireAPI api, int& nCount);
typedef int (*CampfireGetRoomNameIDFunc)(CampfireAPI api, int nRoomIndex, char* pstrRoomName, int& nSizeOfRoomName, int& nID);
typedef int (*CampfireJoinRoomFunc)(CampfireAPI api, int nRoomIndex);
typedef int (*CampfireSayFunc)(CampfireAPI api, const char* pstrMessage, int& nMessageID);
typedef int (*CampfirePasteFunc)(CampfireAPI api, const char* pstrMessage, int& nMessageID);
typedef int (*CampfirePlaySoundFunc)(CampfireAPI api, int nSound);
typedef int (*CampfireUploadFileFunc)(CampfireAPI api, const char* pstrFilePath);

typedef int (*CampfirePingFunc)(CampfireAPI api);
typedef int (*CampfireListenFunc)(CampfireAPI api, int& nCount);
typedef int (*CampfireGetMessagesCountFunc)(CampfireAPI api, int& nCount);
typedef int (*CampfireMessageFunc)(CampfireAPI api, int nIndex, int& nType, char* pstrMessage, int& nSizeOfMessage, int& nUserID);
typedef int (*CampfireMessageClearFunc)(CampfireAPI api);

typedef int (*CampfireLeaveFunc)(CampfireAPI api);
typedef int (*CampfireChangeTopicFunc)(CampfireAPI api, const char* pstrTopic);

typedef int (*CampfireStarMessageFunc)(CampfireAPI api, int nMessageID);

CAMPFIRE_EXTERN int CampfireCreate(CampfireAPI* api);
CAMPFIRE_EXTERN int CampfireFree(CampfireAPI* api);
CAMPFIRE_EXTERN int CampfireLogin(CampfireAPI api, const char* pstrHost, const char* pstrAuthCode, int bUseSSL);
CAMPFIRE_EXTERN int CampfireRoomsCount(CampfireAPI api, int& nCount);
CAMPFIRE_EXTERN int CampfireGetRoomNameID(CampfireAPI api, int nRoomIndex, char* pstrRoomName, int& nSizeOfRoomName, int& nID);
CAMPFIRE_EXTERN int CampfireJoinRoom(CampfireAPI api, int nRoomIndex);
CAMPFIRE_EXTERN int CampfireSay(CampfireAPI api, const char* pstrMessage, int& nMessageID);
CAMPFIRE_EXTERN int CampfirePaste(CampfireAPI api, const char* pstrMessage, int& nMessageID);
CAMPFIRE_EXTERN int CampfirePlaySound(CampfireAPI api, int nSound);
CAMPFIRE_EXTERN int CampfireUploadFile(CampfireAPI api, const char* pstrFilePath);

CAMPFIRE_EXTERN int CampfireListen(CampfireAPI api, int& nCount);
CAMPFIRE_EXTERN int CampfireGetMessagesCount(CampfireAPI api, int& nCount);
CAMPFIRE_EXTERN int CampfireMessage(CampfireAPI api, int nIndex, int& nType, char* pstrMessage, int& nSizeOfMessage, int& nUserID);
CAMPFIRE_EXTERN int CampfireMessageClear(CampfireAPI api);

CAMPFIRE_EXTERN int CampfireLeave(CampfireAPI api);
CAMPFIRE_EXTERN int CampfireChangeTopic(CampfireAPI api, const char* pstrTopic);

CAMPFIRE_EXTERN int CampfireStarMessage(CampfireAPI api, int nMessageID);


#endif
