/****************************************************************************
 Copyright (c) 2013 cocos2d-x.org
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#ifndef __UISLIDER_H__
#define __UISLIDER_H__

#include "UIWidget.h"

NS_CC_BEGIN

class CCSprite;

namespace ui {

typedef enum
{
    SLIDER_PERCENTCHANGED
}SliderEventType;

typedef void (CCObject::*SEL_SlidPercentChangedEvent)(CCObject*,SliderEventType);
#define sliderpercentchangedselector(_SELECTOR) (SEL_SlidPercentChangedEvent)(&_SELECTOR)

/**
*   @js NA
*   @lua NA
*/
class CC_DLL Slider : public Widget
{
    
public:
    /**
     * Default constructor
     */
    Slider();
    
    /**
     * Default destructor
     */
    virtual ~Slider();
    
    /**
     * Allocates and initializes.
     */
    static Slider* create();
    
    /**
     * Load texture for slider bar.
     *
     * @param fileName   file name of texture.
     *
     * @param texType    @see UI_TEX_TYPE_LOCAL
     */
    void loadBarTexture(const char* fileName);
    
    /**
     * Sets if slider is using scale9 renderer.
     *
     * @param true that using scale9 renderer, false otherwise.
     */
    void setScale9Enabled(bool able);
    
    bool isScale9Enabled();
    
    /**
     * Sets capinsets for slider, if slider is using scale9 renderer.
     *
     * @param capInsets    capinsets for slider
     */
    void setCapInsets(const CCRect &capInsets);

    /**
     * Sets capinsets for slider, if slider is using scale9 renderer.
     *
     * @param capInsets    capinsets for slider
     */
    void setCapInsetsBarRenderer(const CCRect &capInsets);
    
    const CCRect& getCapInsetBarRenderer();
    
    /**
     * Sets capinsets for slider, if slider is using scale9 renderer.
     *
     * @param capInsets    capinsets for slider
     */
    void setCapInsetProgressBarRebderer(const CCRect &capInsets);
    
    const CCRect& getCapInsetProgressBarRebderer();
    
    /**
     * Load textures for slider ball.
     *
     * @param slider ball normal    normal state texture.
     *
     * @param slider ball selected    selected state texture.
     *
     * @param slider ball disabled    dark state texture.
     *
     * @param texType    @see UI_TEX_TYPE_LOCAL
     */
    void loadSlidBallTextures(const char* normal,const char* pressed,const char* disabled);
    
    /**
     * Load normal state texture for slider ball.
     *
     * @param normal    normal state texture.
     *
     * @param texType    @see UI_TEX_TYPE_LOCAL
     */
    void loadSlidBallTextureNormal(const char* normal);
    
    /**
     * Load selected state texture for slider ball.
     *
     * @param selected    selected state texture.
     *
     * @param texType    @see UI_TEX_TYPE_LOCAL
     */
    void loadSlidBallTexturePressed(const char* pressed);
    
    /**
     * Load dark state texture for slider ball.
     *
     * @param disabled    dark state texture.
     *
     * @param texType    @see UI_TEX_TYPE_LOCAL
     */
    void loadSlidBallTextureDisabled(const char* disabled);
    
    /**
     * Load dark state texture for slider progress bar.
     *
     * @param fileName    file path of texture.
     *
     * @param texType    @see UI_TEX_TYPE_LOCAL
     */
    void loadProgressBarTexture(const char* fileName);
    const std::string& getTexture() { return _textureFile; }
    const std::string& getProgressBarTexture() { return _progressBarTextureFile; }
    const std::string& getSlidBallNormalTexture() { return _slidBallNormalTextureFile; }
    const std::string& getSlidBallPressedTexture() { return _slidBallPressedTextureFile; }
    const std::string& getSlidBallDisabledTexture() { return _slidBallDisabledTextureFile; }

    /**
     * Changes the progress direction of slider.
     *
     * @param percent    percent value from 1 to 100.
     */
    void setPercent(int percent);
    
    /**
     * Gets the progress direction of slider.
     *
     * @return percent    percent value from 1 to 100.
     */
    int getPercent();
    
    /**
     * Add call back function called when slider's percent has changed to slider.
     */
    void addEventListenerSlider(CCObject* target,SEL_SlidPercentChangedEvent selector);
    
	virtual bool onTouchBegan(int id, const Vec2& pos);
	virtual void onTouchMoved(int id, const Vec2& pos);
	virtual void onTouchEnded(int id, const Vec2& pos);
    
    //override "getContentSize" method of widget.
    virtual const CCSize& getSize() const;
    
    //override "getVirtualRenderer" method of widget.
    virtual CCNode* getVirtualRenderer();
    
    //override "ignoreContentAdaptWithSize" method of widget.
    virtual void ignoreContentAdaptWithSize(bool ignore);
    
    /**
     * Returns the "class name" of widget.
     */
    virtual std::string getDescription() const;

protected:
    virtual bool init();
    virtual void initRenderer();
    float getPercentWithBallPos(float location);
    void percentChangedEvent();
    virtual void onPressStateChangedToNormal();
    virtual void onPressStateChangedToPressed();
    virtual void onPressStateChangedToDisabled();
    virtual void onSizeChanged();
    virtual void updateTextureColor();
    virtual void updateTextureOpacity();
    virtual void updateTextureRGBA();
    void barRendererScaleChangedWithSize();
    void progressBarRendererScaleChangedWithSize();
    virtual Widget* createCloneInstance();
    virtual void copySpecialProperties(Widget* model);
	virtual void draw();
protected:
    SharedPtr<CCNode>  _barRenderer;
    SharedPtr<CCNode> _progressBarRenderer;
    CCSize _progressBarTextureSize;
    
    WeakPtr<CCSprite> _slidBallNormalRenderer;
    WeakPtr<CCSprite> _slidBallPressedRenderer;
    WeakPtr<CCSprite> _slidBallDisabledRenderer;
    SharedPtr<CCNode> _slidBallRenderer;
    
    float _barLength;
    int _percent;
    
    bool _scale9Enabled;
    bool _prevIgnoreSize;
    std::string _textureFile;
    std::string _progressBarTextureFile;
    std::string _slidBallNormalTextureFile;
    std::string _slidBallPressedTextureFile;
    std::string _slidBallDisabledTextureFile;

    CCRect _capInsetsBarRenderer;
    CCRect _capInsetsProgressBarRenderer;

    CCObject*       _sliderEventListener;
    SEL_SlidPercentChangedEvent    _sliderEventSelector;
};

}
NS_CC_END

#endif /* defined(__CocoGUI__Slider__) */
