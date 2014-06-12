/**
 * @file restclient.cpp
 * @brief implementation of the restclient class
 * @author Daniel Schauenberg <d@unwiredcouch.com>
 */

/*========================
         INCLUDES
  ========================*/
#include "RestClient.h"

#include <cstring>
#include <string>
#include <iostream>

using namespace std;

RestClient::RestClient()
: m_bVerbosity(false), m_nTimeout(0), m_pUserData(NULL)
{
   m_strUserAgent = "ajrestclient/0.0.1";
   m_WriteCallback = RestClient::write_callback;
}

RestClient& RestClient::SetVerbosity(bool bEnable)
{
   m_bVerbosity = bEnable;

   return *this;
}

RestClient& RestClient::SetUsernamePassword(std::string& strUsername, std::string& strPassword)
{
   m_strUsername = strUsername;
   m_strPassword = strPassword;

   return *this;
}

RestClient& RestClient::SetTimeout(int nSeconds)
{
   m_nTimeout = nSeconds;

   return *this;
}

RestClient& RestClient::SetCallback(WriteCallback callback /*= RestClient::write_callback*/)
{
   m_WriteCallback = callback;

   return *this;
}

RestClient& RestClient::SetUserData(void* pUserData /*= NULL*/)
{
   m_pUserData = pUserData;

   return *this;
}

CURL* RestClient::ConstructLibCurl() const
{
   CURL *curl;
   curl = curl_easy_init();
   if (curl)
   {
      /** set user agent */
      curl_easy_setopt(curl, CURLOPT_USERAGENT, m_strUserAgent.c_str());

      curl_easy_setopt(curl, CURLOPT_VERBOSE, m_bVerbosity ? 1L : 0L);

      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

      //curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
      
      //curl_easy_setopt(curl, CURLOPT_UNRESTRICTED_AUTH, 1L);

      if( m_strUsername.size() > 0 )
      {
         //cout << "Setting the user/password: " << m_strUsername << ", " << m_strPassword << endl;
         curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
         /*curl_easy_setopt(curl, CURLOPT_USERNAME, m_strUsername.c_str());
         curl_easy_setopt(curl, CURLOPT_PASSWORD, m_strPassword.c_str());*/
         std::string strCombinded = m_strUsername + ":" + m_strPassword;
         //cout << "combined: " << strCombinded << endl;
         curl_easy_setopt(curl, CURLOPT_USERPWD, strCombinded.c_str());
      }

   }
   return curl;
}

/**
 * @brief HTTP GET method
 *
 * @param url to query
 *
 * @return response struct
 */
RestClient::response RestClient::get(const std::string& url, const std::string& data /*= std::string()*/)
{
  /** create return struct */
  RestClient::response ret;

  CURL *curl = ConstructLibCurl();
  CURLcode res;

  if( curl )
  {
    /** set query URL */
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    if( data.size() > 0 )
       curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());

    /** set callback function */
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, m_WriteCallback);

    /** set data object to pass to callback function */
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, m_pUserData ? m_pUserData : &ret);

    if( m_nTimeout > 0 )
    {
      curl_easy_setopt(curl, CURLOPT_TIMEOUT, m_nTimeout);
    }
    else
    {
       curl_easy_setopt(curl, CURLOPT_TIMEOUT, 0);
    }

    /** perform the actual query */
    res = curl_easy_perform(curl);
    if( res == CURLE_OPERATION_TIMEDOUT )
    {

    }
    else if (res != 0)
    {
      ret.body = "Failed to query.";
      ret.code = -1;
      return ret;
    }
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    ret.code = static_cast<int>(http_code);

    curl_easy_cleanup(curl);
  }

  return ret;
}
/**
 * @brief HTTP POST method
 *
 * @param url to query
 * @param ctype content type as string
 * @param data HTTP POST body
 *
 * @return response struct
 */
RestClient::response RestClient::post(const std::string& url,
                                      const std::string& ctype,
                                      const std::string& data)
{
  /** create return struct */
  RestClient::response ret;
  /** build content-type header string */
  std::string ctype_header = "Content-Type: " + ctype;

  /** initialize upload object */
  RestClient::upload_object up_obj;
  up_obj.data = data.c_str();
  up_obj.length = data.size();

  // use libcurl
  CURL *curl = ConstructLibCurl();
  CURLcode res;

  if (curl)
  {
    /** set query URL */
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    /** Now specify we want to POST data */
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    /** set post fields */
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data.size());
    /** set callback function */
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, m_WriteCallback);
    /** set data object to pass to callback function */
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, m_pUserData ? m_pUserData : &ret);
    /** set content-type header */
    curl_slist* header = NULL;
    header = curl_slist_append(header, ctype_header.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
    /** perform the actual query */
    res = curl_easy_perform(curl);
    if (res != 0)
    {
      ret.body = "Failed to query.";
      ret.code = -1;
      return ret;
    }
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    ret.code = static_cast<int>(http_code);

    curl_easy_cleanup(curl);
  }

  return ret;
}
/**
 * @brief HTTP PUT method
 *
 * @param url to query
 * @param ctype content type as string
 * @param data HTTP PUT body
 *
 * @return response struct
 */
RestClient::response RestClient::put(const std::string& url,
                                     const std::string& ctype,
                                     const std::string& data)
{
  /** create return struct */
  RestClient::response ret;
  /** build content-type header string */
  std::string ctype_header = "Content-Type: " + ctype;

  /** initialize upload object */
  RestClient::upload_object up_obj;
  up_obj.data = data.c_str();
  up_obj.length = data.size();

  // use libcurl
  CURL *curl = ConstructLibCurl();
  CURLcode res;

  curl = curl_easy_init();
  if (curl)
  {
    /** set query URL */
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    /** Now specify we want to PUT data */
    curl_easy_setopt(curl, CURLOPT_PUT, 1L);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    /** set read callback function */
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, RestClient::read_callback);
    /** set data object to pass to callback function */
    curl_easy_setopt(curl, CURLOPT_READDATA, &up_obj);
    /** set callback function */
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, m_WriteCallback);
    /** set data object to pass to callback function */
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, m_pUserData ? m_pUserData : &ret);
    /** set data size */
    curl_easy_setopt(curl, CURLOPT_INFILESIZE,
                     static_cast<long>(up_obj.length));

    /** set content-type header */
    curl_slist* header = NULL;
    header = curl_slist_append(header, ctype_header.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
    /** perform the actual query */
    res = curl_easy_perform(curl);
    if (res != 0)
    {
      ret.body = "Failed to query.";
      ret.code = -1;
      return ret;
    }
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    ret.code = static_cast<int>(http_code);

    curl_easy_cleanup(curl);
  }

  return ret;
}
/**
 * @brief HTTP DELETE method
 *
 * @param url to query
 *
 * @return response struct
 */
RestClient::response RestClient::del(const std::string& url)
{
  /** create return struct */
  RestClient::response ret;

  /** we want HTTP DELETE */
  const char* http_delete = "DELETE";

  // use libcurl
  CURL *curl = ConstructLibCurl();
  CURLcode res;

  curl = curl_easy_init();
  if (curl)
  {
    /** set query URL */
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    /** set HTTP DELETE METHOD */
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, http_delete);
    /** set callback function */
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, m_WriteCallback);
    /** set data object to pass to callback function */
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, m_pUserData ? m_pUserData : &ret);
    /** perform the actual query */
    res = curl_easy_perform(curl);
    if (res != 0)
    {
      ret.body = "Failed to query.";
      ret.code = -1;
      return ret;
    }
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    ret.code = static_cast<int>(http_code);

    curl_easy_cleanup(curl);
  }

  return ret;
}

/**
 * @brief write callback function for libcurl
 *
 * @param data returned data of size (size*nmemb)
 * @param size size parameter
 * @param nmemb memblock parameter
 * @param userdata pointer to user data to save/work with return data
 *
 * @return (size * nmemb)
 */
size_t RestClient::write_callback(void *data, size_t size, size_t nmemb,
                            void *userdata)
{
  RestClient::response* r;
  r = reinterpret_cast<RestClient::response*>(userdata);
  r->body.append(reinterpret_cast<char*>(data), size*nmemb);

  return (size * nmemb);
}

/**
 * @brief read callback function for libcurl
 *
 * @param pointer of max size (size*nmemb) to write data to
 * @param size size parameter
 * @param nmemb memblock parameter
 * @param userdata pointer to user data to read data from
 *
 * @return (size * nmemb)
 */
size_t RestClient::read_callback(void *data, size_t size, size_t nmemb,
                            void *userdata)
{
  /** get upload struct */
  RestClient::upload_object* u;
  u = reinterpret_cast<RestClient::upload_object*>(userdata);
  /** set correct sizes */
  size_t curl_size = size * nmemb;
  size_t copy_size = (u->length < curl_size) ? u->length : curl_size;
  /** copy data to buffer */
  memcpy(data, u->data, copy_size);
  /** decrement length and increment data pointer */
  u->length -= copy_size;
  u->data += copy_size;
  /** return copied size */
  return copy_size;
}
