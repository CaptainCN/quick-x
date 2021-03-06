
#ifndef __CC_HTTP_REQUEST_H_
#define __CC_HTTP_REQUEST_H_

#include "scripting/CCLuaEngine.h"
#include "CCStdC.h"

#ifdef _WINDOWS_
#include <Windows.h>
#else
#include <pthread.h>
#endif

#include <stdio.h>
#include <vector>
#include <map>
#include <string>

#if ((CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) && (CC_CURL_ENABLED == 0))
#include <jni.h>
#else
#include "curl/curl.h"
#endif

NS_CC_BEGIN

#define kCCHTTPRequestMethodGET                 0
#define kCCHTTPRequestMethodPOST                1

#define kCCHTTPRequestAcceptEncodingIdentity    0
#define kCCHTTPRequestAcceptEncodingGzip        1
#define kCCHTTPRequestAcceptEncodingDeflate     2

#define kCCHTTPRequestStateIdle                 0
#define kCCHTTPRequestStateCleared              1
#define kCCHTTPRequestStateInProgress           2
#define kCCHTTPRequestStateCompleted            3
#define kCCHTTPRequestStateCancelled            4
#define kCCHTTPRequestStateFailed               5

#define kCCHTTPRequestCURLStateIdle             0
#define kCCHTTPRequestCURLStateBusy             1
#define kCCHTTPRequestCURLStateClosed           2

typedef std::vector<std::string> CCHTTPRequestHeaders;
typedef CCHTTPRequestHeaders::iterator CCHTTPRequestHeadersIterator;

class CCHTTPRequest : public CCObject
{
public:

    static CCHTTPRequest* createWithUrl(const char *url, int method = kCCHTTPRequestMethodGET);

    ~CCHTTPRequest(void);

    /** @brief Set request url. */
    void setRequestUrl(const char *url);

    /** @brief Get request url. */
    const std::string getRequestUrl(void);

    /** @brief Add a custom header to the request. */
    void addRequestHeader(const char *header);

    /** @brief Add a POST variable to the request, POST only. */
    void addPOSTValue(const char *key, const char *value);

    /** @brief Set POST data to the request body, POST only. */
    void setPOSTData(const char *data, size_t len = 0);


	void addFormFile(const char *name, const char *filePath, const char *fileType="application/octet-stream");
	void addFormContents(const char *name, const char *value);

    /** @brief Set/Get cookie string. */
    void setCookieString(const char *cookie);
    const std::string getCookieString(void);

    /** @brief Set accept encoding. */
    void setAcceptEncoding(int acceptEncoding);

    /** @brief Number of seconds to wait before timing out - default is 10. */
    void setTimeout(int timeout);

    /** @brief Execute an asynchronous request. */
    bool start(void);

    /** @brief Cancel an asynchronous request, clearing all delegates first. */
    void cancel(void);

    /** @brief Get the request state. */
    int getState(void);

    /** @brief Return HTTP status code. */
    int getResponseStatusCode(void);

    /** @brief Return HTTP response headers. */
    const CCHTTPRequestHeaders &getResponseHeaders(void);
    const std::string getResponseHeadersString(void);

	bool hasResponseData();
    /** @brief Returns the contents of the result. */
    const std::string getResponseString(void);
    /** @brief Alloc memory block, return response data. use free() release memory block */
	void *getResponseData(void);

    /** @brief Get response data length (bytes). */
    int getResponseDataLength(void);

    /** @brief Save response data to file. */
    size_t saveResponseData(const char *filename);

    /** @brief Get error code. */
    int getErrorCode(void);

    /** @brief Get error message. */
    const std::string getErrorMessage(void);

    /** @brief timer function. */
    void checkCURLState(EventData& map);
    virtual void update(EventData& map);

private:
    CCHTTPRequest(void)
    : m_state(kCCHTTPRequestStateIdle)
    , m_errorCode(0)
    , m_responseCode(0)
    , m_responseBuffer(NULL)
    , m_responseBufferLength(0)
    , m_responseDataLength(0)
    , m_curlState(kCCHTTPRequestCURLStateIdle)
    , m_postData(NULL)
    , m_postDataLen(0)
#if ((CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) && (CC_CURL_ENABLED == 0))
    , m_httpConnect(NULL)
    , m_cookies(NULL)
    , m_nTimeOut(0)
#else
    , m_formPost(NULL)
    , m_lastPost(NULL)
#endif
    , m_dltotal(0)
    , m_dlnow(0)
    , m_ultotal(0)
    , m_ulnow(0)
    {
    }
    bool initWithUrl(const char *url, int method);

    enum {
        DEFAULT_TIMEOUT = 10, // 10 seconds
        BUFFER_CHUNK_SIZE = 32768, // 32 KB
    };

    static unsigned int s_id;
	std::string m_url;
    int m_curlState;

#if !((CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) && (CC_CURL_ENABLED == 0))
    CURL *m_curl;
	curl_httppost *m_formPost;
	curl_httppost *m_lastPost;
#endif

    int     m_state;
    int     m_errorCode;
	std::string  m_errorMessage;

    // request
    typedef std::map<std::string, std::string> Fields;
    Fields m_postFields;
    CCHTTPRequestHeaders m_headers;
    
    void* m_postData;
    size_t m_postDataLen;

#if ((CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) && (CC_CURL_ENABLED == 0))
    jobject m_httpConnect;
    const char* m_httpMethod;
    Fields m_postFile;
    Fields m_postContent;
    int m_nTimeOut;
    const char* m_cookies;
#endif

    // response
    long m_responseCode;
    CCHTTPRequestHeaders m_responseHeaders;
    void *m_responseBuffer;
    size_t m_responseBufferLength;
    size_t m_responseDataLength;
	std::string m_responseCookies;
    
    double m_dltotal;
    double m_dlnow;
    double m_ultotal;
    double m_ulnow;
    
    // private methods
    void cleanup(void);
    void cleanupRawResponseBuff(void);

    // instance callback
    void onRequest(void);
    size_t onWriteData(void *buffer, size_t bytes);
    size_t onWriteHeader(void *buffer, size_t bytes);
    int onProgress(double dltotal, double dlnow, double ultotal, double ulnow);

    // curl callback
#ifdef _WINDOWS_
    static DWORD WINAPI requestCURL(LPVOID userdata);
#else
    pthread_t m_thread;
    static void *requestCURL(void *userdata);
#endif
    static size_t writeDataCURL(void *buffer, size_t size, size_t nmemb, void *userdata);
    static size_t writeHeaderCURL(void *buffer, size_t size, size_t nmemb, void *userdata);
    static int progressCURL(void *userdata, double dltotal, double dlnow, double ultotal, double ulnow);

#if ((CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) && (CC_CURL_ENABLED == 0))

    pthread_attr_t m_threadAttr;

    bool isNeedBoundary();

    void createURLConnectJava();
    void setRequestMethodJava();
    void addRequestHeaderJava(const char* key, const char* value, bool bBoundary);
    void setTimeoutJava(int msTime);
    int connectJava();
    void postContentJava(const char* key, const char* value, bool bConnectSym);
    void postContentByteArrayJava(void* val, size_t len);
    void postFromContentJava(const char* key, const char* value);
    void postFromFileJava(const char* fileName, const char* filePath);
    void postFormEndJava(bool bBoundary);
    int getResponedCodeJava();
    char* getResponedErrJava();
    char* getResponedHeaderJava();
    char* getResponedHeaderByIdxJava(int idx);
    char* getResponedHeaderByKeyJava(const char* key);
    int getResponedHeaderByKeyIntJava(const char* key);
    int   getResponedStringJava(char** ppData);
    void closeJava();

    int   getCStrFromJByteArray(jbyteArray jba, JNIEnv* env, char** ppData);
    char* getCStrFromJString(jstring jstr, JNIEnv* env);
#endif

};

NS_CC_END

#endif /* __CC_HTTP_REQUEST_H_ */
