/****************************************************************************
Copyright (c) 2012 cocos2d-x.org

http://www.cocos2d-x.org

Created by Jung Sang-Taik on 12. 3. 16..
Copyright (c) 2012 Neofect. All rights reserved.

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

#include "CCScale9Sprite.h"

#include "nodes/CCSpriteBatchNode.h"
#include "nodes/CCSprite.h"
#include "resources/CCSpriteFrame.h"

NS_CC_BEGIN

enum positions
{
    pCentre = 0,
    pTop,
    pLeft,
    pRight,
    pBottom,
    pTopRight,
    pTopLeft,
    pBottomRight,
    pBottomLeft
};

CCScale9Sprite::CCScale9Sprite()
: m_insetLeft(0)
, m_insetTop(0)
, m_insetRight(0)
, m_insetBottom(0)
, m_bSpritesGenerated(false)
, m_bSpriteFrameRotated(false)
, m_positionsAreDirty(false)
, _scale9Image(NULL)
, _topLeft(NULL)
, _top(NULL)
, _topRight(NULL)
, _left(NULL)
, _centre(NULL)
, _right(NULL)
, _bottomLeft(NULL)
, _bottom(NULL)
, _bottomRight(NULL)
{

}

CCScale9Sprite::~CCScale9Sprite()
{
}

bool CCScale9Sprite::init()
{
    return this->initWithBatchNode(NULL, CCRectZero, CCRectZero);
}

bool CCScale9Sprite::initWithBatchNode(CCSpriteBatchNode* batchnode, CCRect rect, CCRect capInsets)
{
    return this->initWithBatchNode(batchnode, rect, false, capInsets);
}

bool CCScale9Sprite::initWithBatchNode(CCSpriteBatchNode* batchnode, CCRect rect, bool rotated, CCRect capInsets)
{
    setCascadeColorEnabled(true);
    setCascadeOpacityEnabled(true);

    if(batchnode)
    {
        this->updateWithBatchNode(batchnode, rect, rotated, capInsets);
        this->setAnchorPoint(ccp(0.5f, 0.5f));
    }
    this->updatePositions();
    this->m_positionsAreDirty = true;

    return true;
}

#define    TRANSLATE_X(x, y, xtranslate) \
    x+=xtranslate;                       \

#define    TRANSLATE_Y(x, y, ytranslate) \
    y+=ytranslate;                       \

bool CCScale9Sprite::updateWithBatchNode(CCSpriteBatchNode* batchnode, CCRect rect, bool rotated, CCRect capInsets)
{
    m_bSpriteFrameRotated = rotated;
    GLubyte opacity = getOpacity();
    ccColor3B color = getColor();

    // Release old sprites
	this->removeAllChildrenWithCleanup(true);

    if(_scale9Image != batchnode)
    {
        CC_SAFE_RELEASE(this->_scale9Image);
        _scale9Image = batchnode;
        CC_SAFE_RETAIN(_scale9Image);
    }

    _scale9Image->removeAllChildrenWithCleanup(true);

    m_capInsets = capInsets;
    
    // If there is no given rect
    if ( rect.equals(CCRectZero) )
    {
        // Get the texture size as original
        CCSize textureSize = _scale9Image->getTextureAtlas()->getTexture()->getContentSize();
    
        rect = CCRectMake(0, 0, textureSize.width, textureSize.height);
    }
    
    // Set the given rect's size as original size
    m_spriteRect = rect;
    m_originalSize = rect.size;
    m_preferredSize = m_originalSize;
    m_capInsetsInternal = capInsets;
    
    float w = rect.size.width;
    float h = rect.size.height;

    // If there is no specified center region
    if ( m_capInsetsInternal.equals(CCRectZero) )
    {
        // CCLog("... cap insets not specified : using default cap insets ...");
        m_capInsetsInternal = CCRectMake(w/3, h/3, w/3, h/3);
    }

    float left_w = m_capInsetsInternal.origin.x;
    float center_w = m_capInsetsInternal.size.width;
    float right_w = rect.size.width - (left_w + center_w);

    float top_h = m_capInsetsInternal.origin.y;
    float center_h = m_capInsetsInternal.size.height;
    float bottom_h = rect.size.height - (top_h + center_h);

    // calculate rects

    // ... top row
    float x = 0.0;
    float y = 0.0;

    // top left
    CCRect lefttopbounds = CCRectMake(x, y,
                                      left_w, top_h);

    // top center
    TRANSLATE_X(x, y, left_w);
    CCRect centertopbounds = CCRectMake(x, y,
                                        center_w, top_h);
        
    // top right
    TRANSLATE_X(x, y, center_w);
    CCRect righttopbounds = CCRectMake(x, y,
                                       right_w, top_h);

    // ... center row
    x = 0.0;
    y = 0.0;
    TRANSLATE_Y(x, y, top_h);

    // center left
    CCRect leftcenterbounds = CCRectMake(x, y,
                                         left_w, center_h);

    // center center
    TRANSLATE_X(x, y, left_w);
    CCRect centerbounds = CCRectMake(x, y,
                                     center_w, center_h);

    // center right
    TRANSLATE_X(x, y, center_w);
    CCRect rightcenterbounds = CCRectMake(x, y,
                                          right_w, center_h);

    // ... bottom row
    x = 0.0;
    y = 0.0;
    TRANSLATE_Y(x, y, top_h);
    TRANSLATE_Y(x, y, center_h);

    // bottom left
    CCRect leftbottombounds = CCRectMake(x, y,
                                         left_w, bottom_h);

    // bottom center
    TRANSLATE_X(x, y, left_w);
    CCRect centerbottombounds = CCRectMake(x, y,
                                           center_w, bottom_h);

    // bottom right
    TRANSLATE_X(x, y, center_w);
    CCRect rightbottombounds = CCRectMake(x, y,
                                          right_w, bottom_h);

    if (!rotated) {
        // CCLog("!rotated");

        CCAffineTransform t = CCAffineTransformMakeIdentity();
        t = CCAffineTransformTranslate(t, rect.origin.x, rect.origin.y);

        centerbounds = CCRectApplyAffineTransform(centerbounds, t);
        rightbottombounds = CCRectApplyAffineTransform(rightbottombounds, t);
        leftbottombounds = CCRectApplyAffineTransform(leftbottombounds, t);
        righttopbounds = CCRectApplyAffineTransform(righttopbounds, t);
        lefttopbounds = CCRectApplyAffineTransform(lefttopbounds, t);
        rightcenterbounds = CCRectApplyAffineTransform(rightcenterbounds, t);
        leftcenterbounds = CCRectApplyAffineTransform(leftcenterbounds, t);
        centerbottombounds = CCRectApplyAffineTransform(centerbottombounds, t);
        centertopbounds = CCRectApplyAffineTransform(centertopbounds, t);

        // Centre
        _centre = new CCSprite();
        _centre->initWithTexture(_scale9Image->getTexture(), centerbounds);
        _scale9Image->addChild(_centre, 0, pCentre);
        
        // Top
        _top = new CCSprite();
        _top->initWithTexture(_scale9Image->getTexture(), centertopbounds);
        _scale9Image->addChild(_top, 1, pTop);
        
        // Bottom
        _bottom = new CCSprite();
        _bottom->initWithTexture(_scale9Image->getTexture(), centerbottombounds);
        _scale9Image->addChild(_bottom, 1, pBottom);
        
        // Left
        _left = new CCSprite();
        _left->initWithTexture(_scale9Image->getTexture(), leftcenterbounds);
        _scale9Image->addChild(_left, 1, pLeft);
        
        // Right
        _right = new CCSprite();
        _right->initWithTexture(_scale9Image->getTexture(), rightcenterbounds);
        _scale9Image->addChild(_right, 1, pRight);
        
        // Top left
        _topLeft = new CCSprite();
        _topLeft->initWithTexture(_scale9Image->getTexture(), lefttopbounds);
        _scale9Image->addChild(_topLeft, 2, pTopLeft);
        
        // Top right
        _topRight = new CCSprite();
        _topRight->initWithTexture(_scale9Image->getTexture(), righttopbounds);
        _scale9Image->addChild(_topRight, 2, pTopRight);
        
        // Bottom left
        _bottomLeft = new CCSprite();
        _bottomLeft->initWithTexture(_scale9Image->getTexture(), leftbottombounds);
        _scale9Image->addChild(_bottomLeft, 2, pBottomLeft);
        
        // Bottom right
        _bottomRight = new CCSprite();
        _bottomRight->initWithTexture(_scale9Image->getTexture(), rightbottombounds);
        _scale9Image->addChild(_bottomRight, 2, pBottomRight);
    } else {
        // set up transformation of coordinates
        // to handle the case where the sprite is stored rotated
        // in the spritesheet
        // CCLog("rotated");

        CCAffineTransform t = CCAffineTransformMakeIdentity();

        CCRect rotatedcenterbounds = centerbounds;
        CCRect rotatedrightbottombounds = rightbottombounds;
        CCRect rotatedleftbottombounds = leftbottombounds;
        CCRect rotatedrighttopbounds = righttopbounds;
        CCRect rotatedlefttopbounds = lefttopbounds;
        CCRect rotatedrightcenterbounds = rightcenterbounds;
        CCRect rotatedleftcenterbounds = leftcenterbounds;
        CCRect rotatedcenterbottombounds = centerbottombounds;
        CCRect rotatedcentertopbounds = centertopbounds;
        
        t = CCAffineTransformTranslate(t, rect.size.height+rect.origin.x, rect.origin.y);
        t = CCAffineTransformRotate(t, 1.57079633f);
        
        centerbounds = CCRectApplyAffineTransform(centerbounds, t);
        rightbottombounds = CCRectApplyAffineTransform(rightbottombounds, t);
        leftbottombounds = CCRectApplyAffineTransform(leftbottombounds, t);
        righttopbounds = CCRectApplyAffineTransform(righttopbounds, t);
        lefttopbounds = CCRectApplyAffineTransform(lefttopbounds, t);
        rightcenterbounds = CCRectApplyAffineTransform(rightcenterbounds, t);
        leftcenterbounds = CCRectApplyAffineTransform(leftcenterbounds, t);
        centerbottombounds = CCRectApplyAffineTransform(centerbottombounds, t);
        centertopbounds = CCRectApplyAffineTransform(centertopbounds, t);

        rotatedcenterbounds.origin = centerbounds.origin;
        rotatedrightbottombounds.origin = rightbottombounds.origin;
        rotatedleftbottombounds.origin = leftbottombounds.origin;
        rotatedrighttopbounds.origin = righttopbounds.origin;
        rotatedlefttopbounds.origin = lefttopbounds.origin;
        rotatedrightcenterbounds.origin = rightcenterbounds.origin;
        rotatedleftcenterbounds.origin = leftcenterbounds.origin;
        rotatedcenterbottombounds.origin = centerbottombounds.origin;
        rotatedcentertopbounds.origin = centertopbounds.origin;

        // Centre
        _centre = new CCSprite();
        _centre->initWithTexture(_scale9Image->getTexture(), rotatedcenterbounds, true);
        _scale9Image->addChild(_centre, 0, pCentre);
        
        // Top
        _top = new CCSprite();
        _top->initWithTexture(_scale9Image->getTexture(), rotatedcentertopbounds, true);
        _scale9Image->addChild(_top, 1, pTop);
        
        // Bottom
        _bottom = new CCSprite();
        _bottom->initWithTexture(_scale9Image->getTexture(), rotatedcenterbottombounds, true);
        _scale9Image->addChild(_bottom, 1, pBottom);
        
        // Left
        _left = new CCSprite();
        _left->initWithTexture(_scale9Image->getTexture(), rotatedleftcenterbounds, true);
        _scale9Image->addChild(_left, 1, pLeft);
        
        // Right
        _right = new CCSprite();
        _right->initWithTexture(_scale9Image->getTexture(), rotatedrightcenterbounds, true);
        _scale9Image->addChild(_right, 1, pRight);
        
        // Top left
        _topLeft = new CCSprite();
        _topLeft->initWithTexture(_scale9Image->getTexture(), rotatedlefttopbounds, true);
        _scale9Image->addChild(_topLeft, 2, pTopLeft);
        
        // Top right
        _topRight = new CCSprite();
        _topRight->initWithTexture(_scale9Image->getTexture(), rotatedrighttopbounds, true);
        _scale9Image->addChild(_topRight, 2, pTopRight);
        
        // Bottom left
        _bottomLeft = new CCSprite();
        _bottomLeft->initWithTexture(_scale9Image->getTexture(), rotatedleftbottombounds, true);
        _scale9Image->addChild(_bottomLeft, 2, pBottomLeft);
        
        // Bottom right
        _bottomRight = new CCSprite();
        _bottomRight->initWithTexture(_scale9Image->getTexture(), rotatedrightbottombounds, true);
        _scale9Image->addChild(_bottomRight, 2, pBottomRight);
    }

    this->setSize(rect.size);
    this->addChild(_scale9Image);
    
    setCascadeColorEnabled(true);
    setCascadeOpacityEnabled(true);
    _scale9Image->setCascadeColorEnabled(true);
    _scale9Image->setCascadeOpacityEnabled(true);

    if (m_bSpritesGenerated)
        {
            // Restore color and opacity
            this->setOpacity(opacity);
            this->setColor(color);
        }
    m_bSpritesGenerated = true;

    return true;
}

void CCScale9Sprite::setSize(const CCSize &size)
{
    CCNode::setSize(size);
    this->updatePositions();
    this->m_positionsAreDirty = true;
}

void CCScale9Sprite::updatePositions()
{
    // Check that instances are non-NULL
    if(!((_topLeft) &&
         (_topRight) &&
         (_bottomRight) &&
         (_bottomLeft) &&
         (_centre))) {
        // if any of the above sprites are NULL, return
        return;
    }

    CCSize size = this->_size;

    float sizableWidth = size.width - _topLeft->getSize().width - _topRight->getSize().width;
    float sizableHeight = size.height - _topLeft->getSize().height - _bottomRight->getSize().height;
    
    float horizontalScale = sizableWidth/_centre->getSize().width;
    float verticalScale = sizableHeight/_centre->getSize().height;

    _centre->setScaleX(horizontalScale);
    _centre->setScaleY(verticalScale);

    float rescaledWidth = _centre->getSize().width * horizontalScale;
    float rescaledHeight = _centre->getSize().height * verticalScale;

    float leftWidth = _bottomLeft->getSize().width;
    float bottomHeight = _bottomLeft->getSize().height;

    _bottomLeft->setAnchorPoint(ccp(0,0));
    _bottomRight->setAnchorPoint(ccp(0,0));
    _topLeft->setAnchorPoint(ccp(0,0));
    _topRight->setAnchorPoint(ccp(0,0));
    _left->setAnchorPoint(ccp(0,0));
    _right->setAnchorPoint(ccp(0,0));
    _top->setAnchorPoint(ccp(0,0));
    _bottom->setAnchorPoint(ccp(0,0));
    _centre->setAnchorPoint(ccp(0,0));

    // Position corners
    _bottomLeft->setPosition(ccp(0,0));
    _bottomRight->setPosition(ccp(leftWidth+rescaledWidth,0));
    _topLeft->setPosition(ccp(0, bottomHeight+rescaledHeight));
    _topRight->setPosition(ccp(leftWidth+rescaledWidth, bottomHeight+rescaledHeight));

    // Scale and position borders
    _left->setPosition(ccp(0, bottomHeight));
    _left->setScaleY(verticalScale);
    _right->setPosition(ccp(leftWidth+rescaledWidth,bottomHeight));
    _right->setScaleY(verticalScale);
    _bottom->setPosition(ccp(leftWidth,0));
    _bottom->setScaleX(horizontalScale);
    _top->setPosition(ccp(leftWidth,bottomHeight+rescaledHeight));
    _top->setScaleX(horizontalScale);

    // Position centre
    _centre->setPosition(ccp(leftWidth, bottomHeight));
}

bool CCScale9Sprite::initWithFile(const char* file, CCRect rect,  CCRect capInsets)
{
    CCAssert(file != NULL, "Invalid file for sprite");
    
    CCSpriteBatchNode *batchnode = CCSpriteBatchNode::create(file, 9);
    bool pReturn = this->initWithBatchNode(batchnode, rect, capInsets);
    return pReturn;
}

CCScale9Sprite* CCScale9Sprite::create(const char* file, CCRect rect,  CCRect capInsets)
{
    CCScale9Sprite* pReturn = new CCScale9Sprite();
    if ( pReturn && pReturn->initWithFile(file, rect, capInsets) )
    {
        pReturn->autorelease();
        return pReturn;
    }
    CC_SAFE_DELETE(pReturn);
    return NULL;
}

bool CCScale9Sprite::initWithFile(const char* file, CCRect rect)
{
    CCAssert(file != NULL, "Invalid file for sprite");
    bool pReturn = this->initWithFile(file, rect, CCRectZero);
    return pReturn;
}

CCScale9Sprite* CCScale9Sprite::create(const char* file, CCRect rect)
{
    CCScale9Sprite* pReturn = new CCScale9Sprite();
    if ( pReturn && pReturn->initWithFile(file, rect) )
    {
        pReturn->autorelease();
        return pReturn;
    }
    CC_SAFE_DELETE(pReturn);
    return NULL;
}


bool CCScale9Sprite::initWithFile(CCRect capInsets, const char* file)
{
    bool pReturn = this->initWithFile(file, CCRectZero, capInsets);
    return pReturn;
}

CCScale9Sprite* CCScale9Sprite::create(CCRect capInsets, const char* file)
{
    CCScale9Sprite* pReturn = new CCScale9Sprite();
    if ( pReturn && pReturn->initWithFile(capInsets, file) )
    {
        pReturn->autorelease();
        return pReturn;
    }
    CC_SAFE_DELETE(pReturn);
    return NULL;
}

bool CCScale9Sprite::initWithFile(const char* file)
{
	CCSpriteFrame* frame = SubSystem::get<ResourceCache>()->getResource<CCSpriteFrame>(file);
    bool pReturn = this->initWithSpriteFrame(frame);
    return pReturn;
}

CCScale9Sprite* CCScale9Sprite::create(const char* file)
{
    CCScale9Sprite* pReturn = new CCScale9Sprite();
    if ( pReturn && pReturn->initWithFile(file) )
    {
        pReturn->autorelease();
        return pReturn;
    }
    CC_SAFE_DELETE(pReturn);
    return NULL;
}

bool CCScale9Sprite::initWithSpriteFrame(CCSpriteFrame* spriteFrame, CCRect capInsets)
{
    CCTexture2D* texture = spriteFrame->getTexture();
    CCAssert(texture != NULL, "CCTexture must be not nil");

    CCSpriteBatchNode *batchnode = CCSpriteBatchNode::createWithTexture(texture, 9);
    CCAssert(batchnode != NULL, "CCSpriteBatchNode must be not nil");

    bool pReturn = this->initWithBatchNode(batchnode, spriteFrame->getRect(), spriteFrame->isRotated(), capInsets);
    return pReturn;
}

CCScale9Sprite* CCScale9Sprite::createWithSpriteFrame(CCSpriteFrame* spriteFrame, CCRect capInsets)
{
    CCScale9Sprite* pReturn = new CCScale9Sprite();
    if ( pReturn && pReturn->initWithSpriteFrame(spriteFrame, capInsets) )
    {
        pReturn->autorelease();
        return pReturn;
    }
    CC_SAFE_DELETE(pReturn);
    return NULL;
}
bool CCScale9Sprite::initWithSpriteFrame(CCSpriteFrame* spriteFrame)
{
    CCAssert(spriteFrame != NULL, "Invalid spriteFrame for sprite");
    bool pReturn = this->initWithSpriteFrame(spriteFrame, CCRectZero);
    return pReturn;
}

CCScale9Sprite* CCScale9Sprite::createWithSpriteFrame(CCSpriteFrame* spriteFrame)
{
    CCScale9Sprite* pReturn = new CCScale9Sprite();
    if ( pReturn && pReturn->initWithSpriteFrame(spriteFrame) )
    {
        pReturn->autorelease();
        return pReturn;
    }
    CC_SAFE_DELETE(pReturn);
    return NULL;
}

bool CCScale9Sprite::initWithSpriteFrameName(const char* spriteFrameName, CCRect capInsets)
{
	CCSpriteFrame *frame = SubSystem::get<ResourceCache>()->getResource<CCSpriteFrame>(spriteFrameName);
    CCAssert(frame != NULL, "CCSpriteFrame must be non-NULL");

    if (NULL == frame) return false;

    bool pReturn = this->initWithSpriteFrame(frame, capInsets);
    return pReturn;
}

CCScale9Sprite* CCScale9Sprite::createWithSpriteFrameName(const char* spriteFrameName, CCRect capInsets)
{
    CCScale9Sprite* pReturn = new CCScale9Sprite();
    if ( pReturn && pReturn->initWithSpriteFrameName(spriteFrameName, capInsets) )
    {
        pReturn->autorelease();
        return pReturn;
    }
    CC_SAFE_DELETE(pReturn);
    return NULL;
}

bool CCScale9Sprite::initWithSpriteFrameName(const char* spriteFrameName)
{
    bool pReturn = this->initWithSpriteFrameName(spriteFrameName, CCRectZero);
    return pReturn;
}

CCScale9Sprite* CCScale9Sprite::createWithSpriteFrameName(const char* spriteFrameName)
{
    CCAssert(spriteFrameName != NULL, "spriteFrameName must be non-NULL");

    CCScale9Sprite* pReturn = new CCScale9Sprite();
    if ( pReturn && pReturn->initWithSpriteFrameName(spriteFrameName) )
    {
        pReturn->autorelease();
        return pReturn;
    }
    CC_SAFE_DELETE(pReturn);

    CCLOG("Could not allocate CCScale9Sprite()");
    return NULL;
    
}

CCScale9Sprite* CCScale9Sprite::resizableSpriteWithCapInsets(CCRect capInsets)
{
    CCScale9Sprite* pReturn = new CCScale9Sprite();
    if ( pReturn && pReturn->initWithBatchNode(_scale9Image, m_spriteRect, capInsets) )
    {
        pReturn->autorelease();
        return pReturn;
    }
    CC_SAFE_DELETE(pReturn);
    return NULL;
}

CCScale9Sprite* CCScale9Sprite::create()
{ 
    CCScale9Sprite *pReturn = new CCScale9Sprite();
    if (pReturn && pReturn->init())
    { 
        pReturn->autorelease();   
        return pReturn;
    } 
    CC_SAFE_DELETE(pReturn);
    return NULL;
}

/** sets the opacity.
 @warning If the the texture has premultiplied alpha then, the R, G and B channels will be modifed.
 Values goes from 0 to 255, where 255 means fully opaque.
 */

void CCScale9Sprite::setPreferredSize(CCSize preferedSize)
{
    this->setSize(preferedSize);
    this->m_preferredSize = preferedSize;
}

CCSize CCScale9Sprite::getPreferredSize()
{
    return this->m_preferredSize;
}

void CCScale9Sprite::setCapInsets(CCRect capInsets)
{
    if (!_scale9Image)
    {
        return;
    }
    CCSize contentSize = this->_size;
    this->updateWithBatchNode(this->_scale9Image, this->m_spriteRect, m_bSpriteFrameRotated, capInsets);
    this->setSize(contentSize);
}

CCRect CCScale9Sprite::getCapInsets()
{
    return m_capInsets;
}

void CCScale9Sprite::updateCapInset()
{
    CCRect insets;
    if (this->m_insetLeft == 0 && this->m_insetTop == 0 && this->m_insetRight == 0 && this->m_insetBottom == 0)
    {
        insets = CCRectZero;
    }
    else
    {
        if (m_bSpriteFrameRotated)
        {
            insets = CCRectMake(m_insetBottom,
                m_insetLeft,
                m_spriteRect.size.width-m_insetRight-m_insetLeft,
                m_spriteRect.size.height-m_insetTop-m_insetBottom);
        }
        else
        {
            insets = CCRectMake(m_insetLeft,
                m_insetTop,
                m_spriteRect.size.width-m_insetLeft-m_insetRight,
                m_spriteRect.size.height-m_insetTop-m_insetBottom);
        }
    }
    this->setCapInsets(insets);
}

void CCScale9Sprite::setSpriteFrame(CCSpriteFrame * spriteFrame)
{
    CCSpriteBatchNode * batchnode = CCSpriteBatchNode::createWithTexture(spriteFrame->getTexture(), 9);
    this->updateWithBatchNode(batchnode, spriteFrame->getRect(), spriteFrame->isRotated(), CCRectZero);

    // Reset insets
    this->m_insetLeft = 0;
    this->m_insetTop = 0;
    this->m_insetRight = 0;
    this->m_insetBottom = 0;
}

float CCScale9Sprite::getInsetLeft()
{
    return this->m_insetLeft;
}

float CCScale9Sprite::getInsetTop()
{
    return this->m_insetTop;
}

float CCScale9Sprite::getInsetRight()
{
    return this->m_insetRight;
}

float CCScale9Sprite::getInsetBottom()
{
    return this->m_insetBottom;
}

void CCScale9Sprite::setInsetLeft(float insetLeft)
{
    this->m_insetLeft = insetLeft;
    this->updateCapInset();
}

void CCScale9Sprite::setInsetTop(float insetTop)
{
    this->m_insetTop = insetTop;
    this->updateCapInset();
}

void CCScale9Sprite::setInsetRight(float insetRight)
{
    this->m_insetRight = insetRight;
    this->updateCapInset();
}

void CCScale9Sprite::setInsetBottom(float insetBottom)
{
    this->m_insetBottom = insetBottom;
    this->updateCapInset();
}

void CCScale9Sprite::visit()
{
    if(this->m_positionsAreDirty)
    {
        this->updatePositions();
        this->m_positionsAreDirty = false;
    }
    CCNode::visit();
}

void CCScale9Sprite::setBlendFunc(ccBlendFunc blendFunc)
{
    this->_scale9Image->setBlendFunc(blendFunc);
}

ccBlendFunc CCScale9Sprite::getBlendFunc(void)
{
    return this->_scale9Image->getBlendFunc();
}

NS_CC_END
