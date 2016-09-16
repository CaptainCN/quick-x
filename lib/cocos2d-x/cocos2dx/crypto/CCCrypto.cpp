
#include "crypto/CCCrypto.h"

#include "md5.h"
extern "C" {
#include "libbase64.h"
#include "xxtea.h"
}

#include "CCLuaEngine.h"
#include "CCLuaStack.h"

extern "C" {
#include "lua.h"
#include "tolua_fix.h"
}

NS_CC_BEGIN

unsigned char* CCCrypto::encryptXXTEA(unsigned char* plaintext,
                                      int plaintextLength,
                                      unsigned char* key,
                                      int keyLength,
                                      int* resultLength)
{
    xxtea_long len;
    unsigned char* result = xxtea_encrypt(plaintext, (xxtea_long)plaintextLength, key, (xxtea_long)keyLength, &len);
    *resultLength = (int)len;
    return result;
}

unsigned char* CCCrypto::decryptXXTEA(unsigned char* ciphertext,
                                      int ciphertextLength,
                                      unsigned char* key,
                                      int keyLength,
                                      int* resultLength)
{
    xxtea_long len;
    unsigned char* result = xxtea_decrypt(ciphertext, (xxtea_long)ciphertextLength, key, (xxtea_long)keyLength, &len);
    *resultLength = (int)len;
    return result;
}

int CCCrypto::encodeBase64Len(int inputLength)
{
    return Base64encode_len(inputLength);
}

int CCCrypto::decodeBase64Len(const char* input)
{
    return Base64decode_len(input);
}

char* CCCrypto::decodeBase64(const char *input, unsigned long& sizeInOut)
{
	size_t bufferSize = Base64decode_len((const char*)input);

	char *buffer = bufferSize ? new char[bufferSize] : NULL;

	if (!buffer) return nullptr;

	sizeInOut = Base64decode(buffer, (const char*)input);

	return buffer;
}

char* CCCrypto::encodeBase64(const char *input, unsigned long& sizeInOut)
{
	size_t bufferSize = Base64encode_len(sizeInOut);

	char *buffer = bufferSize ? new char[bufferSize] : NULL;

	if (!buffer) return nullptr;

	sizeInOut = Base64encode(buffer, (const char*)input, sizeInOut);

	return buffer;
}

void CCCrypto::MD5(void* input, int inputLength, unsigned char* output)
{
    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx, input, inputLength);
    MD5_Final(output, &ctx);
}

void CCCrypto::MD5File(const char* path, unsigned char* output)
{
    FILE *file = fopen(path, "rb");
    if (file == NULL)
        return;
    
    MD5_CTX ctx;
    MD5_Init(&ctx);
    
    int i;
    const int BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];
    while ((i = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        MD5_Update(&ctx, buffer, (unsigned) i);
    }
    
    fclose(file);
    MD5_Final(output, &ctx);
}

const std::string CCCrypto::MD5String(void* input, int inputLength)
{
    unsigned char buffer[MD5_BUFFER_LENGTH];
    MD5(static_cast<void*>(input), inputLength, buffer);

    CCLuaStack* stack = CCLuaEngine::defaultEngine()->getLuaStack();
    stack->clean();

    char* hex = bin2hex(buffer, MD5_BUFFER_LENGTH);
	std::string ret(hex);
    delete[] hex;
    return ret;
}



LUA_STRING CCCrypto::encryptXXTEALua(const char* plaintext,
                                     int plaintextLength,
                                     const char* key,
                                     int keyLength)
{
    CCLuaStack* stack = CCLuaEngine::defaultEngine()->getLuaStack();
    stack->clean();

    int resultLength;
    unsigned char* result = encryptXXTEA((unsigned char*)plaintext, plaintextLength, (unsigned char*)key, keyLength, &resultLength);
    
    if (resultLength <= 0)
    {
        lua_pushnil(stack->getLuaState());
    }
    else
    {
        lua_pushlstring(stack->getLuaState(), (const char*)result, resultLength);
        free(result);
    }
    return 1;
}

LUA_STRING CCCrypto::decryptXXTEALua(const char* plaintext,
                                     int plaintextLength,
                                     const char* key,
                                     int keyLength)
{
    CCLuaStack* stack = CCLuaEngine::defaultEngine()->getLuaStack();
    stack->clean();
    
    int resultLength;
    unsigned char* result = decryptXXTEA((unsigned char*)plaintext, plaintextLength, (unsigned char*)key, keyLength, &resultLength);
    
    if (resultLength <= 0)
    {
        lua_pushnil(stack->getLuaState());
    }
    else
    {
        lua_pushlstring(stack->getLuaState(), (const char*)result, resultLength);
        free(result);
    }
    return 1;
}

LUA_STRING CCCrypto::encodingBase64Lua(bool isDecoding,
                                       const char* input,
                                       int inputLength)
{
    CCLuaStack* stack = CCLuaEngine::defaultEngine()->getLuaStack();
    stack->clean();

    int bufferSize = isDecoding ? Base64decode_len(input) : Base64encode_len(inputLength);
    char *buffer = bufferSize ? (char*)malloc(bufferSize) : NULL;
    int size = 0;

    if (buffer)
    {
        size = isDecoding ? Base64decode(buffer, input) : (Base64encode(buffer, input, inputLength) - 1);
    }
    if (size)
    {
        stack->pushString(buffer, size);
    }
    else
    {
        stack->pushNil();
    }
    if (buffer)
    {
        free(buffer);
    }
    return 1;
}

LUA_STRING CCCrypto::MD5Lua(char* input, bool isRawOutput)
{
    unsigned char buffer[MD5_BUFFER_LENGTH];
    MD5(static_cast<void*>(input), (int)strlen(input), buffer);
    
    CCLuaStack* stack = CCLuaEngine::defaultEngine()->getLuaStack();
    stack->clean();
    
    if (isRawOutput)
    {
        stack->pushString((char*)buffer, MD5_BUFFER_LENGTH);
    }
    else
    {
        char* hex = bin2hex(buffer, MD5_BUFFER_LENGTH);
        stack->pushString(hex);
        delete[] hex;
    }
    
    return 1;
}

LUA_STRING CCCrypto::MD5FileLua(const char* path)
{
    unsigned char buffer[MD5_BUFFER_LENGTH];
    MD5File(path, buffer);
    
    CCLuaStack* stack = CCLuaEngine::defaultEngine()->getLuaStack();
    stack->clean();
    
    char* hex = bin2hex(buffer, MD5_BUFFER_LENGTH);
    stack->pushString(hex);
    delete[] hex;
    
    return 1;
}

char* CCCrypto::bin2hex(unsigned char* bin, int binLength)
{
    static const char* hextable = "0123456789abcdef";
    
    int hexLength = binLength * 2 + 1;
    char* hex = new char[hexLength];
    memset(hex, 0, sizeof(char) * hexLength);
    
    int ci = 0;
    for (int i = 0; i < 16; ++i)
    {
        unsigned char c = bin[i];
        hex[ci++] = hextable[(c >> 4) & 0x0f];
        hex[ci++] = hextable[c & 0x0f];
    }
    
    return hex;
}


NS_CC_END
