#include "RestClientFactory.h"
#include "RestClient.h"

RestClientFactory::RestClientFactory()
: m_bVerbose(RESTCLIENT_DEFAULT_VERBOSITY)
{
}

RestClient* RestClientFactory::CreateRestClient()
{
   RestClient* pClient = new RestClient();
   pClient->SetUsernamePassword(m_strUsername, m_strPassword);
   pClient->SetVerbosity(m_bVerbose);
   return pClient;
}

RestClientFactory& RestClientFactory::SetUsernamePassword(const std::string& strUsername, const std::string strPassword)
{
   m_strUsername = strUsername;
   m_strPassword = strPassword;

   return *this;
}

RestClientFactory& RestClientFactory::SetVerbosity(bool bEnable)
{
   m_bVerbose = bEnable;
   return *this;
}

