#include "CCHTTPRequest.h"
#include <stdio.h>
#include <iostream>
#include "CCScheduler.h"
#include <sstream>
#include "engine/CCEngineEvents.h"
#include "CCNetworkEvents.h"

using namespace cocos2d;

NS_CC_BEGIN

unsigned int CCHTTPRequest::s_id = 0;

CCHTTPRequest *CCHTTPRequest::createWithUrl(const char *url, int method)
{
    CCHTTPRequest *request = new CCHTTPRequest();
    request->initWithUrl(url, method);
    request->autorelease();
    return request;
}

bool CCHTTPRequest::initWithUrl(const char *url, int method)
{
#if CC_CURL_ENABLED > 0
    CCAssert(url, "CCHTTPRequest::initWithUrl() - invalid url");
    m_curl = curl_easy_init();
    curl_easy_setopt(m_curl, CURLOPT_URL, url);
    curl_easy_setopt(m_curl, CURLOPT_USERAGENT, "libcurl");
    curl_easy_setopt(m_curl, CURLOPT_CONNECTTIMEOUT, DEFAULT_TIMEOUT);
    curl_easy_setopt(m_curl, CURLOPT_TIMEOUT, DEFAULT_TIMEOUT);
    curl_easy_setopt(m_curl, CURLOPT_NOSIGNAL, 1L);

    curl_easy_setopt(m_curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(m_curl, CURLOPT_SSL_VERIFYPEER, 0L);

    if (method == kCCHTTPRequestMethodPOST)
    {
        curl_easy_setopt(m_curl, CURLOPT_POST, 1L);
        curl_easy_setopt(m_curl, CURLOPT_COPYPOSTFIELDS, "");
    }
    
    ++s_id;
    CCLOG("CCHTTPRequest[0x%04x] - create request with url: %s", s_id, url);
    return true;
#else
    return false;
#endif
}

CCHTTPRequest::~CCHTTPRequest(void)
{
    cleanup();
    CCLOG("CCHTTPRequest[0x%04x] - request removed", s_id);
}

void CCHTTPRequest::setRequestUrl(const char *url)
{
#if CC_CURL_ENABLED > 0
    CCAssert(url, "CCHTTPRequest::setRequestUrl() - invalid url");
    m_url = url;
    curl_easy_setopt(m_curl, CURLOPT_URL, m_url.c_str());
#endif
}

const std::string CCHTTPRequest::getRequestUrl(void)
{
    return m_url;
}

void CCHTTPRequest::addRequestHeader(const char *header)
{
    CCAssert(m_state == kCCHTTPRequestStateIdle, "CCHTTPRequest::addRequestHeader() - request not idle");
    CCAssert(header, "CCHTTPRequest::addRequestHeader() - invalid header");
    m_headers.push_back(std::string(header));
}

void CCHTTPRequest::addPOSTValue(const char *key, const char *value)
{
    CCAssert(m_state == kCCHTTPRequestStateIdle, "CCHTTPRequest::addPOSTValue() - request not idle");
    CCAssert(key, "CCHTTPRequest::addPOSTValue() - invalid key");
    m_postFields[std::string(key)] = std::string(value ? value : "");
}

void CCHTTPRequest::setPOSTData(const char *data, size_t len)
{
#if CC_CURL_ENABLED > 0
    CCAssert(m_state == kCCHTTPRequestStateIdle, "CCHTTPRequest::setPOSTData() - request not idle");
    CCAssert(data, "CCHTTPRequest::setPOSTData() - invalid post data");
    m_postFields.clear();
    if (0 == len) {
        len = strlen(data);
    }
    if (0 == len) {
        return;
    }
    if (m_postData)
    {
        free(m_postData);
        m_postDataLen = 0;
        m_postData = NULL;
    }
    m_postData = malloc(len + 1);
    memset(m_postData, 0, len + 1);
    if (NULL == m_postData)
    {
        return;
    }
    memcpy(m_postData, data, len);
    m_postDataLen = len;
    curl_easy_setopt(m_curl, CURLOPT_POST, 1L);
    //curl_easy_setopt(m_curl, CURLOPT_COPYPOSTFIELDS, data);
    curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, m_postData);
    curl_easy_setopt(m_curl, CURLOPT_POSTFIELDSIZE, m_postDataLen);
#endif
}

void CCHTTPRequest::addFormFile(const char *name, const char *filePath, const char *contentType)
{
#if CC_CURL_ENABLED > 0
	curl_formadd(&m_formPost, &m_lastPost,
		CURLFORM_COPYNAME, name,
		CURLFORM_FILE, filePath,
		CURLFORM_CONTENTTYPE, contentType,
		CURLFORM_END);
	//CCLOG("addFormFile %s %s %s", name, filePath, contentType);
#endif
}

void CCHTTPRequest::addFormContents(const char *name, const char *value)
{
#if CC_CURL_ENABLED > 0
	curl_formadd(&m_formPost, &m_lastPost,
		CURLFORM_COPYNAME, name,
		CURLFORM_COPYCONTENTS, value,
		CURLFORM_END);
	//CCLOG("addFormContents %s %s", name, value);
#endif
}

void CCHTTPRequest::setCookieString(const char *cookie)
{
#if CC_CURL_ENABLED > 0
    CCAssert(m_state == kCCHTTPRequestStateIdle, "CCHTTPRequest::setAcceptEncoding() - request not idle");
    curl_easy_setopt(m_curl, CURLOPT_COOKIE, cookie ? cookie : "");
#endif
}

const std::string CCHTTPRequest::getCookieString(void)
{
    CCAssert(m_state == kCCHTTPRequestStateCompleted, "CCHTTPRequest::getResponseData() - request not completed");
    return m_responseCookies;
}

void CCHTTPRequest::setAcceptEncoding(int acceptEncoding)
{
#if CC_CURL_ENABLED > 0
    CCAssert(m_state == kCCHTTPRequestStateIdle, "CCHTTPRequest::setAcceptEncoding() - request not idle");
    switch (acceptEncoding)
    {
        case kCCHTTPRequestAcceptEncodingGzip:
            curl_easy_setopt(m_curl, CURLOPT_ACCEPT_ENCODING, "gzip");
            break;
            
        case kCCHTTPRequestAcceptEncodingDeflate:
            curl_easy_setopt(m_curl, CURLOPT_ACCEPT_ENCODING, "deflate");
            break;
            
        default:
            curl_easy_setopt(m_curl, CURLOPT_ACCEPT_ENCODING, "identity");
    }
#endif
}

void CCHTTPRequest::setTimeout(int timeout)
{
#if CC_CURL_ENABLED > 0
    CCAssert(m_state == kCCHTTPRequestStateIdle, "CCHTTPRequest::setTimeout() - request not idle");
    curl_easy_setopt(m_curl, CURLOPT_CONNECTTIMEOUT, timeout);
    curl_easy_setopt(m_curl, CURLOPT_TIMEOUT, timeout);
#endif
}

bool CCHTTPRequest::start(void)
{
#if CC_CURL_ENABLED > 0
    CCAssert(m_state == kCCHTTPRequestStateIdle, "CCHTTPRequest::start() - request not idle");

    m_state = kCCHTTPRequestStateInProgress;
    m_curlState = kCCHTTPRequestCURLStateBusy;
    retain();

    curl_easy_setopt(m_curl, CURLOPT_HTTP_CONTENT_DECODING, 1L);
    curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, writeDataCURL);
    curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(m_curl, CURLOPT_HEADERFUNCTION, writeHeaderCURL);
    curl_easy_setopt(m_curl, CURLOPT_WRITEHEADER, this);
    curl_easy_setopt(m_curl, CURLOPT_NOPROGRESS, false);
    curl_easy_setopt(m_curl, CURLOPT_PROGRESSFUNCTION, progressCURL);
    curl_easy_setopt(m_curl, CURLOPT_PROGRESSDATA, this);
    curl_easy_setopt(m_curl, CURLOPT_COOKIEFILE, "");

#ifdef _WINDOWS_
    CreateThread(NULL,          // default security attributes
                 0,             // use default stack size
                 requestCURL,   // thread function name
                 this,          // argument to thread function
                 0,             // use default creation flags
                 NULL);
#else
    pthread_create(&m_thread, NULL, requestCURL, this);
    pthread_detach(m_thread);
#endif
    this->subscribeToEvent<UpdateEvent>(Handler(this, &CCHTTPRequest::update));
//    SubSystem::get<CCScheduler>()->scheduleUpdateForTarget(this, 0, false);
    // CCLOG("CCHTTPRequest[0x%04x] - request start", s_id);
    return true;
#else
    return false;
#endif
}

void CCHTTPRequest::cancel(void)
{
    if (m_state == kCCHTTPRequestStateIdle || m_state == kCCHTTPRequestStateInProgress)
    {
        m_state = kCCHTTPRequestStateCancelled;
    }
}

int CCHTTPRequest::getState(void)
{
    return m_state;
}

int CCHTTPRequest::getResponseStatusCode(void)
{
    CCAssert(m_state == kCCHTTPRequestStateCompleted, "Request not completed");
    return static_cast<int>(m_responseCode);
}

const CCHTTPRequestHeaders &CCHTTPRequest::getResponseHeaders(void)
{
    CCAssert(m_state == kCCHTTPRequestStateCompleted, "CCHTTPRequest::getResponseHeaders() - request not completed");
    return m_responseHeaders;
}

const std::string CCHTTPRequest::getResponseHeadersString()
{
	std::string buf;
    for (CCHTTPRequestHeadersIterator it = m_responseHeaders.begin(); it != m_responseHeaders.end(); ++it)
    {
        buf.append(*it);
    }
    return buf;
}

bool CCHTTPRequest::hasResponseData()
{
	return !!m_responseBuffer;
}

const std::string CCHTTPRequest::getResponseString(void)
{
    CCAssert(m_state == kCCHTTPRequestStateCompleted, "CCHTTPRequest::getResponseString() - request not completed");
    return std::string(m_responseBuffer ? static_cast<char*>(m_responseBuffer) : "");
}

void *CCHTTPRequest::getResponseData(void)
{
    CCAssert(m_state == kCCHTTPRequestStateCompleted, "CCHTTPRequest::getResponseData() - request not completed");
    void *buff = malloc(m_responseDataLength);
    memcpy(buff, m_responseBuffer, m_responseDataLength);
    return buff;
}

int CCHTTPRequest::getResponseDataLength(void)
{
    CCAssert(m_state == kCCHTTPRequestStateCompleted, "Request not completed");
    return (int)m_responseDataLength;
}

size_t CCHTTPRequest::saveResponseData(const char *filename)
{
    CCAssert(m_state == kCCHTTPRequestStateCompleted, "CCHTTPRequest::saveResponseData() - request not completed");
    
    FILE *fp = fopen(filename, "wb");
    CCAssert(fp, "CCHTTPRequest::saveResponseData() - open file failure");
    
    size_t writedBytes = m_responseDataLength;
    if (writedBytes > 0)
    {
        fwrite(m_responseBuffer, m_responseDataLength, 1, fp);
    }
    fclose(fp);
    return writedBytes;
}

int CCHTTPRequest::getErrorCode(void)
{
    return m_errorCode;
}

const std::string CCHTTPRequest::getErrorMessage(void)
{
    return m_errorMessage;
}

void CCHTTPRequest::checkCURLState(EventData& data)
{
    CC_UNUSED_PARAM(data);
    if (m_curlState != kCCHTTPRequestCURLStateBusy)
    {
//		SubSystem::get<CCScheduler>()->unscheduleAllForTarget(this);
		release();
    }
}

void CCHTTPRequest::update(EventData& data)
{
    float dt = data[UpdateEvent::timeStep].GetFloat();
    if (m_state == kCCHTTPRequestStateInProgress)
    {
		HttpRequestEvent eventData;
		eventData[HttpRequestEvent::name] = "inprogress";
		eventData[HttpRequestEvent::download_progress] = m_dlnow / m_dltotal;
		eventData[HttpRequestEvent::unload_progress] = m_ultotal / m_ulnow;
		sendEvent(eventData);
        return;
    }
	//SubSystem::get<CCScheduler>()->unscheduleAllForTarget(this);
    this->unsubscribeFromEvent(UpdateEvent::_ID);

    if (m_curlState != kCCHTTPRequestCURLStateIdle)
    {
        this->subscribeToEvent<UpdateEvent>(Handler(this, &CCHTTPRequest::checkCURLState));
//		SubSystem::get<CCScheduler>()->scheduleSelector(schedule_selector(CCHTTPRequest::checkCURLState), this, 0, false);
    }

	const char* name = nullptr;

    switch (m_state)
    {
        case kCCHTTPRequestStateCompleted: name = "completed"; break;
		case kCCHTTPRequestStateCancelled: name = "cancelled"; break;
		case kCCHTTPRequestStateFailed:    name = "failed";    break;
        default:name = "unknown";
    }

	HttpRequestEvent eventData;
	eventData[HttpRequestEvent::name] = name;
	eventData[HttpRequestEvent::download_progress] = 0;
	eventData[HttpRequestEvent::unload_progress] = 0;
	sendEvent(eventData);
}

// instance callback

void CCHTTPRequest::onRequest(void)
{
#if CC_CURL_ENABLED > 0
    if (m_postFields.size() > 0)
    {
        curl_easy_setopt(m_curl, CURLOPT_POST, 1L);
		std::stringbuf buf;
        for (Fields::iterator it = m_postFields.begin(); it != m_postFields.end(); ++it)
        {
            char *part = curl_easy_escape(m_curl, it->first.c_str(), 0);
            buf.sputn(part, strlen(part));
            buf.sputc('=');
            curl_free(part);
            
            part = curl_easy_escape(m_curl, it->second.c_str(), 0);
            buf.sputn(part, strlen(part));
            curl_free(part);
            
            buf.sputc('&');
        }
        curl_easy_setopt(m_curl, CURLOPT_COPYPOSTFIELDS, buf.str().c_str());
    }

    struct curl_slist *chunk = NULL;
    for (CCHTTPRequestHeadersIterator it = m_headers.begin(); it != m_headers.end(); ++it)
    {
        chunk = curl_slist_append(chunk, (*it).c_str());
    }

	if (m_formPost)
	{
		curl_easy_setopt(m_curl, CURLOPT_HTTPPOST, m_formPost);
	}

    curl_slist *cookies = NULL;
    curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, chunk);
    CURLcode code = curl_easy_perform(m_curl);
    curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE, &m_responseCode);
    curl_easy_getinfo(m_curl, CURLINFO_COOKIELIST, &cookies);

    if (cookies)
    {
        struct curl_slist *nc = cookies;
		std::stringbuf buf;
        while (nc)
        {
            buf.sputn(nc->data, strlen(nc->data));
            buf.sputc('\n');
            nc = nc->next;
        }
        m_responseCookies = buf.str();
        curl_slist_free_all(cookies);
        cookies = NULL;
    }

    curl_easy_cleanup(m_curl);
    m_curl = NULL;
	if (m_formPost)
	{
		curl_formfree(m_formPost);
		m_formPost = NULL;
	}
    curl_slist_free_all(chunk);
    
    m_errorCode = code;
    m_errorMessage = (code == CURLE_OK) ? "" : curl_easy_strerror(code);
    m_state = (code == CURLE_OK) ? kCCHTTPRequestStateCompleted : kCCHTTPRequestStateFailed;
    m_curlState = kCCHTTPRequestCURLStateClosed;
#endif
}

size_t CCHTTPRequest::onWriteData(void *buffer, size_t bytes)
{
    if (m_responseDataLength + bytes + 1 > m_responseBufferLength)
    {
        m_responseBufferLength += BUFFER_CHUNK_SIZE;
        m_responseBuffer = realloc(m_responseBuffer, m_responseBufferLength);
    }

    memcpy(static_cast<char*>(m_responseBuffer) + m_responseDataLength, buffer, bytes);
    m_responseDataLength += bytes;
    static_cast<char*>(m_responseBuffer)[m_responseDataLength] = 0;
    return bytes;
}

size_t CCHTTPRequest::onWriteHeader(void *buffer, size_t bytes)
{
    char *headerBuffer = new char[bytes + 1];
    headerBuffer[bytes] = 0;
    memcpy(headerBuffer, buffer, bytes);    
    m_responseHeaders.push_back(std::string(headerBuffer));
    delete []headerBuffer;
    return bytes;
}

int CCHTTPRequest::onProgress(double dltotal, double dlnow, double ultotal, double ulnow)
{
    m_dltotal = dltotal;
    m_dlnow = dlnow;
    m_ultotal = ultotal;
    m_ulnow = ulnow;
    
    return m_state == kCCHTTPRequestStateCancelled ? 1: 0;
}

void CCHTTPRequest::cleanup(void)
{
#if CC_CURL_ENABLED > 0
    m_state = kCCHTTPRequestStateCleared;
    m_responseBufferLength = 0;
    m_responseDataLength = 0;
    m_postDataLen = 0;
    if (m_postData)
    {
        free(m_postData);
        m_postData = NULL;
    }
    if (m_responseBuffer)
    {
        free(m_responseBuffer);
        m_responseBuffer = NULL;
    }
    if (m_curl)
    {
        curl_easy_cleanup(m_curl);
        m_curl = NULL;
    }
#endif
}

// curl callback

#ifdef _WINDOWS_
DWORD WINAPI CCHTTPRequest::requestCURL(LPVOID userdata)
{
    static_cast<CCHTTPRequest*>(userdata)->onRequest();
    return 0;
}
#else // _WINDOWS_
void *CCHTTPRequest::requestCURL(void *userdata)
{
    static_cast<CCHTTPRequest*>(userdata)->onRequest();
    return NULL;
}
#endif // _WINDOWS_

size_t CCHTTPRequest::writeDataCURL(void *buffer, size_t size, size_t nmemb, void *userdata)
{
    return static_cast<CCHTTPRequest*>(userdata)->onWriteData(buffer, size *nmemb);
}

size_t CCHTTPRequest::writeHeaderCURL(void *buffer, size_t size, size_t nmemb, void *userdata)
{
    return static_cast<CCHTTPRequest*>(userdata)->onWriteHeader(buffer, size *nmemb);
}

int CCHTTPRequest::progressCURL(void *userdata, double dltotal, double dlnow, double ultotal, double ulnow)
{
    return static_cast<CCHTTPRequest*>(userdata)->onProgress(dltotal, dlnow, ultotal, ulnow);
}

NS_CC_END

#include "engine/CCEventImpl.h"
#include "CCNetworkEvents.h"

