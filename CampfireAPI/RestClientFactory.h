#ifndef RESTCLIENTFACOTRY_H
#define RESTCLIENTFACOTRY_H

#include <curl/curl.h>
#include <string>

#define RESTCLIENT_DEFAULT_VERBOSITY  0L

class RestClient;

class RestClientFactory
{
public:
   RestClientFactory();

   RestClient* CreateRestClient();

   RestClientFactory& SetUsernamePassword(const std::string& strUsername, const std::string strPassword);
   RestClientFactory& SetVerbosity(bool bEnable);

protected:
   std::string m_strUsername;
   std::string m_strPassword;
   bool m_bVerbose;
};

#endif  // RESTCLIENTFACOTRY_H
