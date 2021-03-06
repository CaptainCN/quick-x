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

#include "UISlider.h"
#include "nodes/CCScale9Sprite.h"
#include "nodes/CCSprite.h"

NS_CC_BEGIN

namespace ui {

Slider::Slider():
_progressBarTextureSize(CCSizeZero),
_barLength(0.0),
_percent(0),
_scale9Enabled(false),
_prevIgnoreSize(true),
_textureFile(""),
_progressBarTextureFile(""),
_slidBallNormalTextureFile(""),
_slidBallPressedTextureFile(""),
_slidBallDisabledTextureFile(""),
_capInsetsBarRenderer(CCRectZero),
_capInsetsProgressBarRenderer(CCRectZero),
_sliderEventListener(NULL),
_sliderEventSelector(NULL)
{
}

Slider::~Slider()
{
    _sliderEventListener = NULL;
    _sliderEventSelector = NULL;
}

Slider* Slider::create()
{
    Slider* widget = new Slider();
    if (widget && widget->init())
    {
        widget->autorelease();
        return widget;
    }
    CC_SAFE_DELETE(widget);
    return NULL;
}
    
bool Slider::init()
{
    if (Widget::init())
    {
        setTouchEnabled(true);
        return true;
    }
    return false;
}

void Slider::initRenderer()
{
    _barRenderer = CCSprite::create();
    _progressBarRenderer = CCSprite::create();
    _progressBarRenderer->setAnchorPoint(Vec2(0.0f, 0.5f));
    _slidBallNormalRenderer = CCSprite::create();
    _slidBallPressedRenderer = CCSprite::create();
    _slidBallPressedRenderer->setVisible(false);
    _slidBallDisabledRenderer = CCSprite::create();
    _slidBallDisabledRenderer->setVisible(false);
    _slidBallRenderer = CCNode::create();
    _slidBallRenderer->addChild(_slidBallNormalRenderer);
    _slidBallRenderer->addChild(_slidBallPressedRenderer);
    _slidBallRenderer->addChild(_slidBallDisabledRenderer);
}

void Slider::loadBarTexture(const char* fileName)
{
    if (!fileName || strcmp(fileName, "") == 0)
    {
        return;
    }
    _textureFile = fileName;
	if (_scale9Enabled)
		static_cast<CCScale9Sprite*>(_barRenderer.Get())->initWithFile(fileName);
	else
		static_cast<CCSprite*>(_barRenderer.Get())->initWithFile(fileName);
    updateRGBAToRenderer(_barRenderer);
    barRendererScaleChangedWithSize();
    progressBarRendererScaleChangedWithSize();
}

void Slider::loadProgressBarTexture(const char *fileName)
{
    if (!fileName || strcmp(fileName, "") == 0)
    {
        return;
    }
	_progressBarTextureFile = fileName;
	if (_scale9Enabled)
		static_cast<CCScale9Sprite*>(_progressBarRenderer.Get())->initWithFile(fileName);
	else
		static_cast<CCSprite*>(_progressBarRenderer.Get())->initWithFile(fileName);
    updateRGBAToRenderer(_progressBarRenderer);
    _progressBarRenderer->setAnchorPoint(Vec2(0.0f, 0.5f));
    _progressBarTextureSize = _progressBarRenderer->getSize();
    progressBarRendererScaleChangedWithSize();
}

void Slider::setScale9Enabled(bool able)
{
    if (_scale9Enabled == able)
    {
        return;
    }
    
    _scale9Enabled = able;
    CCNode::removeChild(_barRenderer, true);
    CCNode::removeChild(_progressBarRenderer, true);
    _barRenderer = NULL;
    _progressBarRenderer = NULL;
    if (_scale9Enabled)
    {
        _barRenderer = CCScale9Sprite::create();
        _progressBarRenderer = CCScale9Sprite::create();
    }
    else
    {
        _barRenderer = CCSprite::create();
        _progressBarRenderer = CCSprite::create();
    }
    loadBarTexture(_textureFile.c_str());
    loadProgressBarTexture(_progressBarTextureFile.c_str());
    if (_scale9Enabled)
    {
        bool ignoreBefore = _ignoreSize;
        ignoreContentAdaptWithSize(false);
        _prevIgnoreSize = ignoreBefore;
    }
    else
    {
        ignoreContentAdaptWithSize(_prevIgnoreSize);
    }
    setCapInsetsBarRenderer(_capInsetsBarRenderer);
    setCapInsetProgressBarRebderer(_capInsetsProgressBarRenderer);
}

void Slider::draw()
{
	_barRenderer->visit();
	_progressBarRenderer->visit();
	_slidBallRenderer->visit();
}
    
bool Slider::isScale9Enabled()
{
    return _scale9Enabled;
}

void Slider::ignoreContentAdaptWithSize(bool ignore)
{
    if (!_scale9Enabled || (_scale9Enabled && !ignore))
    {
        Widget::ignoreContentAdaptWithSize(ignore);
        _prevIgnoreSize = ignore;
    }
}

void Slider::setCapInsets(const CCRect &capInsets)
{
    setCapInsetsBarRenderer(capInsets);
    setCapInsetProgressBarRebderer(capInsets);
}

void Slider::setCapInsetsBarRenderer(const CCRect &capInsets)
{
    _capInsetsBarRenderer = capInsets;
    if (!_scale9Enabled)
    {
        return;
    }
    static_cast<CCScale9Sprite*>(_barRenderer.Get())->setCapInsets(capInsets);
}
    
const CCRect& Slider::getCapInsetBarRenderer()
{
    return _capInsetsBarRenderer;
}

void Slider::setCapInsetProgressBarRebderer(const CCRect &capInsets)
{
    _capInsetsProgressBarRenderer = capInsets;
    if (!_scale9Enabled)
    {
        return;
    }
    static_cast<CCScale9Sprite*>(_progressBarRenderer.Get())->setCapInsets(capInsets);
}
    
const CCRect& Slider::getCapInsetProgressBarRebderer()
{
    return _capInsetsProgressBarRenderer;
}

void Slider::loadSlidBallTextures(const char* normal,const char* pressed,const char* disabled)
{
    loadSlidBallTextureNormal(normal);
    loadSlidBallTexturePressed(pressed);
    loadSlidBallTextureDisabled(disabled);
}

void Slider::loadSlidBallTextureNormal(const char* normal)
{
    if (!normal || strcmp(normal, "") == 0)
    {
        return;
    }
    _slidBallNormalTextureFile = normal;
	_slidBallNormalRenderer->initWithFile(normal);
	
    updateRGBAToRenderer(_slidBallNormalRenderer);
}

void Slider::loadSlidBallTexturePressed(const char* pressed)
{
    if (!pressed || strcmp(pressed, "") == 0)
    {
        return;
    }
    _slidBallPressedTextureFile = pressed;
	_slidBallPressedRenderer->initWithFile(pressed);
	
    updateRGBAToRenderer(_slidBallPressedRenderer);
}

void Slider::loadSlidBallTextureDisabled(const char* disabled)
{
    if (!disabled || strcmp(disabled, "") == 0)
    {
        return;
    }
    _slidBallDisabledTextureFile = disabled;
	_slidBallDisabledRenderer->initWithFile(disabled);
	
    updateRGBAToRenderer(_slidBallDisabledRenderer);
}

void Slider::setPercent(int percent)
{
    if (percent > 100)
    {
        percent = 100;
    }
    if (percent < 0)
    {
        percent = 0;
    }
    _percent = percent;
    float res = percent/100.0f;
    float dis = _barLength * res;
    _slidBallRenderer->setPosition(Vec2(-_barLength/2.0f + dis, 0.0f));
    if (_scale9Enabled)
    {
        static_cast<CCScale9Sprite*>(_progressBarRenderer.Get())->setPreferredSize(CCSize(dis,_progressBarTextureSize.height));
    }
    else
    {
        CCSprite* spriteRenderer = static_cast<CCSprite*>(_progressBarRenderer.Get());
        CCRect rect = spriteRenderer->getTextureRect();
        rect.size.width = _progressBarTextureSize.width * res;
        spriteRenderer->setTextureRect(rect, spriteRenderer->isTextureRectRotated(), rect.size);
    }
}

bool Slider::onTouchBegan(int id, const Vec2& pos)
{
    bool pass = Widget::onTouchBegan(id, pos);
    if (_hitted)
    {
        Vec2 nsp = convertToNodeSpace(_touchStartPos);
        setPercent(getPercentWithBallPos(nsp.x));
        percentChangedEvent();
    }
    return pass;
}

void Slider::onTouchMoved(int id, const Vec2& pos)
{
	_touchMovePos = pos;
    Vec2 nsp = convertToNodeSpace(_touchMovePos);
    _slidBallRenderer->setPosition(Vec2(nsp.x,0));
    setPercent(getPercentWithBallPos(nsp.x));
    percentChangedEvent();
}

void Slider::onTouchEnded(int id, const Vec2& pos)
{
    Widget::onTouchEnded(id, pos);
}

float Slider::getPercentWithBallPos(float px)
{
    return (((px-(-_barLength/2.0f))/_barLength)*100.0f);
}

void Slider::addEventListenerSlider(CCObject *target, SEL_SlidPercentChangedEvent selector)
{
    _sliderEventListener = target;
    _sliderEventSelector = selector;
}

void Slider::percentChangedEvent()
{
    if (_sliderEventListener && _sliderEventSelector)
    {
        (_sliderEventListener->*_sliderEventSelector)(this,SLIDER_PERCENTCHANGED);
    }
}

int Slider::getPercent()
{
    return _percent;
}

void Slider::onSizeChanged()
{
    Widget::onSizeChanged();
    barRendererScaleChangedWithSize();
    progressBarRendererScaleChangedWithSize();
}

const CCSize& Slider::getSize() const
{
    return _barRenderer->getSize();
}

CCNode* Slider::getVirtualRenderer()
{
    return _barRenderer;
}

void Slider::barRendererScaleChangedWithSize()
{
    if (_ignoreSize)
    {
        
        _barRenderer->setScale(1.0f);
        _size = _barRenderer->getSize();
        _barLength = _size.width;
    }
    else
    {
        _barLength = _size.width;
        if (_scale9Enabled)
        {
            static_cast<CCScale9Sprite*>(_barRenderer.Get())->setPreferredSize(_size);
        }
        else
        {
            CCSize btextureSize = _barRenderer->getSize();
            if (btextureSize.width <= 0.0f || btextureSize.height <= 0.0f)
            {
                _barRenderer->setScale(1.0f);
                return;
            }
            float bscaleX = _size.width / btextureSize.width;
            float bscaleY = _size.height / btextureSize.height;
            _barRenderer->setScaleX(bscaleX);
            _barRenderer->setScaleY(bscaleY);
        }
    }
    setPercent(_percent);
}

void Slider::progressBarRendererScaleChangedWithSize()
{
    if (_ignoreSize)
    {
        if (!_scale9Enabled)
        {
            CCSize ptextureSize = _progressBarTextureSize;
            float pscaleX = _size.width / ptextureSize.width;
            float pscaleY = _size.height / ptextureSize.height;
            _progressBarRenderer->setScaleX(pscaleX);
            _progressBarRenderer->setScaleY(pscaleY);
        }
    }
    else
    {
        if (_scale9Enabled)
        {
            static_cast<CCScale9Sprite*>(_progressBarRenderer.Get())->setPreferredSize(_size);
            _progressBarTextureSize = _progressBarRenderer->getSize();
        }
        else
        {
            CCSize ptextureSize = _progressBarTextureSize;
            if (ptextureSize.width <= 0.0f || ptextureSize.height <= 0.0f)
            {
                _progressBarRenderer->setScale(1.0f);
                return;
            }
            float pscaleX = _size.width / ptextureSize.width;
            float pscaleY = _size.height / ptextureSize.height;
            _progressBarRenderer->setScaleX(pscaleX);
            _progressBarRenderer->setScaleY(pscaleY);
        }
    }
    _progressBarRenderer->setPosition(Vec2(-_barLength * 0.5f, 0.0f));
    setPercent(_percent);
}

void Slider::onPressStateChangedToNormal()
{
    _slidBallNormalRenderer->setVisible(true);
    _slidBallPressedRenderer->setVisible(false);
    _slidBallDisabledRenderer->setVisible(false);
}

void Slider::onPressStateChangedToPressed()
{
    _slidBallNormalRenderer->setVisible(false);
    _slidBallPressedRenderer->setVisible(true);
    _slidBallDisabledRenderer->setVisible(false);
}

void Slider::onPressStateChangedToDisabled()
{
    _slidBallNormalRenderer->setVisible(false);
    _slidBallPressedRenderer->setVisible(false);
    _slidBallDisabledRenderer->setVisible(true);
}
    
void Slider::updateTextureColor()
{
    updateColorToRenderer(_barRenderer);
    updateColorToRenderer(_progressBarRenderer);
    updateColorToRenderer(_slidBallNormalRenderer);
    updateColorToRenderer(_slidBallPressedRenderer);
    updateColorToRenderer(_slidBallDisabledRenderer);
}

void Slider::updateTextureOpacity()
{
    updateOpacityToRenderer(_barRenderer);
    updateOpacityToRenderer(_progressBarRenderer);
    updateOpacityToRenderer(_slidBallNormalRenderer);
    updateOpacityToRenderer(_slidBallPressedRenderer);
    updateOpacityToRenderer(_slidBallDisabledRenderer);
}

void Slider::updateTextureRGBA()
{
    updateRGBAToRenderer(_barRenderer);
    updateRGBAToRenderer(_progressBarRenderer);
    updateRGBAToRenderer(_slidBallNormalRenderer);
    updateRGBAToRenderer(_slidBallPressedRenderer);
    updateRGBAToRenderer(_slidBallDisabledRenderer);
}

std::string Slider::getDescription() const
{
    return "Slider";
}

Widget* Slider::createCloneInstance()
{
    return Slider::create();
}

void Slider::copySpecialProperties(Widget *widget)
{
    Slider* slider = dynamic_cast<Slider*>(widget);
    if (slider)
    {
        _prevIgnoreSize = slider->_prevIgnoreSize;
        setScale9Enabled(slider->_scale9Enabled);
        loadBarTexture(slider->_textureFile.c_str());
        loadProgressBarTexture(slider->_progressBarTextureFile.c_str());
        loadSlidBallTextureNormal(slider->_slidBallNormalTextureFile.c_str());
        loadSlidBallTexturePressed(slider->_slidBallPressedTextureFile.c_str());
        loadSlidBallTextureDisabled(slider->_slidBallDisabledTextureFile.c_str());
        setPercent(slider->getPercent());
    }
}

}

NS_CC_END
