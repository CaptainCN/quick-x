#ifndef __CCDEVICE_H__
#define __CCDEVICE_H__

#include "CCPlatformMacros.h"

#include <string>

NS_CC_BEGIN


/**
 * @addtogroup platform
 * @{
 */


/**
 @brief Enum the language type supported now
 */
typedef enum LanguageType
{
    kLanguageEnglish = 0,
    kLanguageChinese,
    kLanguageFrench,
    kLanguageItalian,
    kLanguageGerman,
    kLanguageSpanish,
    kLanguageDutch,
    kLanguageRussian,
    kLanguageKorean,
    kLanguageJapanese,
    kLanguageHungarian,
    kLanguagePortuguese,
    kLanguageArabic,
    kLanguageChineseTW,
} ccLanguageType;


enum TargetPlatform
{
	kTargetWindows,
	kTargetLinux,
	kTargetMacOS,
	kTargetAndroid,
	kTargetIphone,
	kTargetIpad,
	kTargetBlackBerry,
	kTargetNaCl,
	kTargetEmscripten,
	kTargetTizen,
	kTargetWinRT,
	kTargetWP8
};


/**
 @js NA
 @lua NA
 */
class CC_DLL CCDevice
{
public:

    /**
     @brief Output Debug message.
     */
    static void log(const char * fmt, ...);
	static void logV(const char * fmt, va_list args);
	static void logS(const char * str);

    /**
     @brief Pop out a message box
     */
    static void showMessageBox(const char * pszMsg, const char * pszTitle);


	static bool isIOS64bit();

	/**
	@brief Get target platform
	*/
	static TargetPlatform getTargetPlatform();

	/**
	@brief Get current language config
	@return Current language config
	*/
	static ccLanguageType getCurrentLanguage();

	/**
     *  Gets the DPI of device
     *  @return The DPI of device.
     */
    static int getDPI();
    static std::string getWritablePath();
    static std::string getResourcePath();


    /** @brief Show activity indicator */
    static void showActivityIndicator(void);

    /** @brief Hide activity indicator */
    static void hideActivityIndicator(void);

    /** @brief Create alert view */
    static void createAlert(const char* title,
                            const char* message,
                            const char* cancelButtonTitle);

    /** @brief Add button to alert view, return button index */
    static int addAlertButton(const char* buttonTitle);

    /** @brief Show alert view */
    static void showAlert();
    /** @brief Hide and remove alert view */
    static void cancelAlert(void);

    /** @brief Open a web page in the browser; create an email; or call a phone number. */
    static void openURL(const char* url);

    /** @brief Show alert view, and get user input */
    static const std::string getInputText(const char* title, const char* message, const char* defaultValue);

    static const std::string getDeviceName(void);
    static void vibrate();

private:
    CCDevice();
};

CC_DLL void CCLog(const char* fmt, ...);

NS_CC_END

#endif /* __CCDEVICE_H__ */
