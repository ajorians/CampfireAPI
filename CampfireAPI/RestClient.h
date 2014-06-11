/**
 * @file restclient.h
 * @brief libcurl wrapper for REST calls
 * @author Daniel Schauenberg <d@unwiredcouch.com>
 * @version
 * @date 2010-10-11
 */

#ifndef INCLUDE_RESTCLIENT_H_
#define INCLUDE_RESTCLIENT_H_

#include <curl/curl.h>
#include <string>
#include <cstdlib>

class RestClient
{
public:
   RestClient();
   RestClient& SetVerbosity(bool bEnable);
   RestClient& SetUsernamePassword(std::string& strUsername, std::string& strPassword);
   /**
   * public data definitions
   */
   /** response struct for queries */
   typedef struct
   {
      int code;
      std::string body;
   } response;
   /** struct used for uploading data */
   typedef struct
   {
      const char* data;
      size_t length;
   } upload_object;

   /** public methods */
   // HTTP GET
   response get(const std::string& url, const std::string& data = std::string());
   // HTTP POST
   response post(const std::string& url, const std::string& ctype,
      const std::string& data);
   // HTTP PUT
   response put(const std::string& url, const std::string& ctype,
      const std::string& data);
   // HTTP DELETE
   response del(const std::string& url);

   CURL* ConstructLibCurl() const;

protected:
   // writedata callback function
   static size_t write_callback(void *ptr, size_t size, size_t nmemb,
      void *userdata);
   // read callback function
   static size_t read_callback(void *ptr, size_t size, size_t nmemb,
      void *userdata);

   std::string m_strUsername;
   std::string m_strPassword;
   std::string m_strUserAgent;
   bool m_bVerbosity;
};

#endif  // INCLUDE_RESTCLIENT_H_
