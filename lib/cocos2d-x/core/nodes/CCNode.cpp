/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2008-2010 Ricardo Quesada
 Copyright (c) 2009      Valentin Milea
 Copyright (c) 2011      Zynga Inc.

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
#include "cocoa/CCString.h"
#include "CCNode.h"
#include "cocoa/TransformUtils.h"
#include "CCCamera.h"
#include "engine/CCDirector.h"
#include "CCScheduler.h"
#include "touch_dispatcher/CCTouch.h"
#include "actions/CCActionManager.h"
#include "cocoa/CCScriptSupport.h"
#include "shaders/CCGLProgram.h"
#include "nodes/CCScene.h"
// externals
#include "kazmath/matrix.h"
#include "engine/CCAttributeAccessor.h"

#if CC_NODE_RENDER_SUBPIXEL
#define RENDER_IN_SUBPIXEL
#else
#define RENDER_IN_SUBPIXEL(__ARGS__) (ceil(__ARGS__))
#endif

NS_CC_BEGIN

// XXX: Yes, nodes might have a sort problem once every 15 days if the game runs at 60 FPS and each frame sprites are reordered.
static int s_globalOrderOfArrival = 1;

unsigned int CCNode::g_drawOrder = 0;

CCNode::CCNode(void)
: m_fRotationX(0.0f)
, m_fRotationY(0.0f)
, _name("")
, m_fScaleX(1.0f)
, m_fScaleY(1.0f)
, m_fVertexZ(0.0f)
, m_obPosition(CCPointZero)
, m_fSkewX(0.0f)
, m_fSkewY(0.0f)
, m_obAnchorPointInPoints(CCPointZero)
, m_obAnchorPoint(CCPointZero)
, _size(CCSizeZero)
, m_sAdditionalTransform(CCAffineTransformMakeIdentity())
, m_pCamera(NULL)
// children (lazy allocs)
// lazy alloc
, m_nZOrder(0)
, _parent(NULL)
// "whole screen" objects. like Scenes and Layers, should set m_bIgnoreAnchorPointForPosition to true
, m_nTag(kCCNodeTagInvalid)
// userData is always inited as nil
, m_pUserData(NULL)
, m_eGLServerState(ccGLServerState(0))
, m_uOrderOfArrival(0)
, m_bRunning(false)
, m_bTransformDirty(true)
, m_bInverseDirty(true)
, m_bAdditionalTransformDirty(false)
, m_bVisible(true)
, m_bIgnoreAnchorPointForPosition(false)
, m_bReorderChildDirty(false)
, m_displayedOpacity(255)
, m_realOpacity(255)
, m_isOpacityModifyRGB(false)
, m_displayedColor(ccWHITE)
, m_realColor(ccWHITE)
, m_cascadeColorEnabled(false)
, m_cascadeOpacityEnabled(false)
, m_drawOrder(0)
// touch
, _touchEnabled(false)
{
    // set default scheduler and actionManager
    CCDirector *director = CCDirector::sharedDirector();

	m_pActionManager = director->getSubSystem<CCActionManager>();
	m_pScheduler = director->getSubSystem<CCScheduler>();
}

CCNode::~CCNode(void)
{
}

bool CCNode::init()
{
    return true;
}

float CCNode::getSkewX()
{
    return m_fSkewX;
}

void CCNode::setSkewX(float newSkewX)
{
    m_fSkewX = newSkewX;
    m_bTransformDirty = m_bInverseDirty = true;
}

float CCNode::getSkewY()
{
    return m_fSkewY;
}

void CCNode::setSkewY(float newSkewY)
{
    m_fSkewY = newSkewY;

    m_bTransformDirty = m_bInverseDirty = true;
}

/// zOrder getter
int CCNode::getZOrder() const
{
    return m_nZOrder;
}

/// zOrder setter : private method
/// used internally to alter the zOrder variable. DON'T call this method manually
void CCNode::_setZOrder(int z)
{
    m_nZOrder = z;
}

void CCNode::setZOrder(int z)
{
    _setZOrder(z);
    if (_parent)
    {
        _parent->reorderChild(this, z);
    }
}

/// vertexZ getter
float CCNode::getVertexZ()
{
    return m_fVertexZ;
}


/// vertexZ setter
void CCNode::setVertexZ(float var)
{
    m_fVertexZ = var;
}


/// rotation getter
float CCNode::getRotation() const 
{
    CCAssert(m_fRotationX == m_fRotationY, "CCNode#rotation. RotationX != RotationY. Don't know which one to return");
    return m_fRotationX;
}

/// rotation setter
void CCNode::setRotation(float newRotation)
{
    m_fRotationX = m_fRotationY = newRotation;
    m_bTransformDirty = m_bInverseDirty = true;
}

float CCNode::getRotationX()
{
    return m_fRotationX;
}

void CCNode::setRotationX(float fRotationX)
{
    m_fRotationX = fRotationX;
    m_bTransformDirty = m_bInverseDirty = true;
}

float CCNode::getRotationY()
{
    return m_fRotationY;
}

void CCNode::setRotationY(float fRotationY)
{
    m_fRotationY = fRotationY;
    m_bTransformDirty = m_bInverseDirty = true;
}

/// scale getter
float CCNode::getScale(void) const
{
    CCAssert( m_fScaleX == m_fScaleY, "CCNode#scale. ScaleX != ScaleY. Don't know which one to return");
    return m_fScaleX;
}

/// scale setter
void CCNode::setScale(float scale)
{
    m_fScaleX = m_fScaleY = scale;
    m_bTransformDirty = m_bInverseDirty = true;
}

/// scale setter
void CCNode::setScale(float fScaleX,float fScaleY)
{
    m_fScaleX = fScaleX;
    m_fScaleY = fScaleY;
    m_bTransformDirty = m_bInverseDirty = true;
}

/// scaleX getter
float CCNode::getScaleX()
{
    return m_fScaleX;
}

/// scaleX setter
void CCNode::setScaleX(float newScaleX)
{
    m_fScaleX = newScaleX;
    m_bTransformDirty = m_bInverseDirty = true;
}

/// scaleY getter
float CCNode::getScaleY()
{
    return m_fScaleY;
}

/// scaleY setter
void CCNode::setScaleY(float newScaleY)
{
    m_fScaleY = newScaleY;
    m_bTransformDirty = m_bInverseDirty = true;
}

/// position getter
const Vec2& CCNode::getPosition() const
{
    return m_obPosition;
}

/// position setter
void CCNode::setPosition(const Vec2& newPosition)
{
    m_obPosition = newPosition;
    m_bTransformDirty = m_bInverseDirty = true;
}

void CCNode::getPosition(float* x, float* y)
{
    *x = m_obPosition.x;
    *y = m_obPosition.y;
}

void CCNode::setPosition(float x, float y)
{
    setPosition(ccp(x, y));
}

float CCNode::getPositionX(void)
{
    return m_obPosition.x;
}

float CCNode::getPositionY(void)
{
    return  m_obPosition.y;
}

void CCNode::setPositionX(float x)
{
    setPosition(ccp(x, m_obPosition.y));
}

void CCNode::setPositionY(float y)
{
    setPosition(ccp(m_obPosition.x, y));
}

/// children getter
CCArray* CCNode::getChildren()
{
    return m_pChildren;
}

unsigned int CCNode::getChildrenCount(void) const
{
    return m_pChildren ? m_pChildren->count() : 0;
}

/// camera getter: lazy alloc
CCCamera* CCNode::getCamera()
{
    if (!m_pCamera)
        m_pCamera = new CCCamera();

    return m_pCamera;
}

/// isVisible getter
bool CCNode::isVisible() const
{
    return m_bVisible;
}

/// isVisible setter
void CCNode::setVisible(bool var)
{
    m_bVisible = var;
}

const Vec2& CCNode::getAnchorPointInPoints()
{
    return m_obAnchorPointInPoints;
}

/// anchorPoint getter
const Vec2& CCNode::getAnchorPoint() const 
{
    return m_obAnchorPoint;
}

void CCNode::setAnchorPoint(const Vec2& point)
{
    if( ! point.equals(m_obAnchorPoint))
    {
        m_obAnchorPoint = point;
        m_obAnchorPointInPoints = ccp(_size.width * m_obAnchorPoint.x, _size.height * m_obAnchorPoint.y );
        m_bTransformDirty = m_bInverseDirty = true;
    }
}

/// contentSize getter
const CCSize& CCNode::getSize() const
{
    return _size;
}

void CCNode::setSize(const CCSize & size)
{
    if ( ! size.equals(_size))
    {
        _size = size;

        m_obAnchorPointInPoints = ccp(_size.width * m_obAnchorPoint.x, _size.height * m_obAnchorPoint.y );
        m_bTransformDirty = m_bInverseDirty = true;
    }
}

// isRunning getter
bool CCNode::isRunning()
{
    return m_bRunning;
}

/// parent getter
CCNode * CCNode::getParent()
{
    return _parent;
}
/// parent setter
void CCNode::setParent(CCNode * var)
{
    _parent = var;
}

/// isRelativeAnchorPoint getter
bool CCNode::isIgnoreAnchorPointForPosition()
{
    return m_bIgnoreAnchorPointForPosition;
}
/// isRelativeAnchorPoint setter
void CCNode::ignoreAnchorPointForPosition(bool newValue)
{
    if (newValue != m_bIgnoreAnchorPointForPosition)
    {
		m_bIgnoreAnchorPointForPosition = newValue;
		m_bTransformDirty = m_bInverseDirty = true;
	}
}

/// tag getter
int CCNode::getTag() const
{
    return m_nTag;
}

/// tag setter
void CCNode::setTag(int var)
{
    m_nTag = var;
}

/// userData getter
void * CCNode::getUserData()
{
    return m_pUserData;
}

/// userData setter
void CCNode::setUserData(void *var)
{
    m_pUserData = var;
}

unsigned int CCNode::getOrderOfArrival()
{
    return m_uOrderOfArrival;
}

void CCNode::setOrderOfArrival(unsigned int uOrderOfArrival)
{
    m_uOrderOfArrival = uOrderOfArrival;
}

CCGLProgram* CCNode::getShaderProgram()
{
    return m_pShaderProgram;
}

CCObject* CCNode::getUserObject()
{
    return m_pUserObject;
}

ccGLServerState CCNode::getGLServerState()
{
    return m_eGLServerState;
}

void CCNode::setGLServerState(ccGLServerState glServerState)
{
    m_eGLServerState = glServerState;
}

void CCNode::setUserObject(CCObject *pUserObject)
{
    m_pUserObject = pUserObject;
}

RenderState* CCNode::getRenderState()
{
	return _renderState;
}

void CCNode::setRenderState(RenderState* m)
{
	_renderState = m;
}

void CCNode::setShaderProgram(CCGLProgram *pShaderProgram)
{
    m_pShaderProgram = pShaderProgram;
}

CCRect CCNode::boundingBox()
{
    CCRect rect = CCRectMake(0, 0, _size.width, _size.height);
    return CCRectApplyAffineTransform(rect, nodeToParentTransform());
}

CCRect CCNode::getCascadeBoundingBox(void)
{
    CCRect cbb;
    if (m_cascadeBoundingBox.size.width > 0 && m_cascadeBoundingBox.size.height > 0)
    {
        // if cascade bounding box set by user, ignore all childrens bounding box
        cbb = m_cascadeBoundingBox;
    }
    else
    {
        // check all childrens bounding box, get maximize box
        CCObject *object = NULL;
        CCNode* child = NULL;
        bool merge = false;
        CCARRAY_FOREACH(m_pChildren, object)
        {
            child = dynamic_cast<CCNode*>(object);
            if (!child->isVisible()) continue;

            const CCRect box = child->getCascadeBoundingBox();
            if (box.size.width <= 0 || box.size.height <= 0) continue;

            if (!merge)
            {
                cbb = box;
                merge = true;
            }
            else
            {
                cbb.merge(box);
            }
        }

        // merge content size
        if (_size.width > 0 && _size.height > 0)
        {
            const CCRect box = CCRectApplyAffineTransform(CCRect(0, 0, _size.width, _size.height), nodeToWorldTransform());
            if (!merge)
            {
                cbb = box;
            }
            else
            {
                cbb.merge(box);
            }
        }
    }

    return cbb;
}

void CCNode::setCascadeBoundingBox(const cocos2d::CCRect &boundingBox)
{
    m_cascadeBoundingBox = boundingBox;
}

void CCNode::resetCascadeBoundingBox(void)
{
    m_cascadeBoundingBox = CCRectZero;
}

CCNode * CCNode::create(void)
{
	CCNode * pRet = new CCNode();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }
	return pRet;
}

void CCNode::cleanup()
{
    // actions
    this->stopAllActions();
    this->unscheduleAllSelectors();

    // timers
    arrayMakeObjectsPerformSelector(m_pChildren, cleanup, CCNode*);

    if (m_scriptEventListeners)
    {
    }
}

const char* CCNode::description()
{
    return CCString::createWithFormat("<CCNode | Tag = %d>", m_nTag)->getCString();
}

// lazy allocs
void CCNode::childrenAlloc(void)
{
    m_pChildren = CCArray::createWithCapacity(4);
}

CCNode* CCNode::getChildByTag(int aTag)
{
    CCAssert( aTag != kCCNodeTagInvalid, "Invalid tag");

    if(m_pChildren && m_pChildren->count() > 0)
    {
        CCObject* child;
        CCARRAY_FOREACH(m_pChildren, child)
        {
            CCNode* pNode = (CCNode*) child;
            if(pNode && pNode->m_nTag == aTag)
                return pNode;
        }
    }
    return NULL;
}

/* "add" logic MUST only be on this method
 * If a class want's to extend the 'addChild' behavior it only needs
 * to override this method
 */
void CCNode::addChild(CCNode *child, int zOrder, int tag)
{
    CCAssert( child != NULL, "Argument must be non-nil");
    CCAssert( child->_parent == NULL, "child already added. It can't be added again");

    if( ! m_pChildren )
    {
        this->childrenAlloc();
    }

    this->insertChild(child, zOrder);

    child->m_nTag = tag;

    child->setParent(this);
    child->setOrderOfArrival(s_globalOrderOfArrival++);

    if( m_bRunning )
    {
        child->onEnter();
    }
}

void CCNode::addChild(CCNode *child, int zOrder)
{
    CCAssert( child != NULL, "Argument must be non-nil");
    this->addChild(child, zOrder, child->m_nTag);
}

void CCNode::addChild(CCNode *child)
{
    CCAssert( child != NULL, "Argument must be non-nil");
    this->addChild(child, child->m_nZOrder, child->m_nTag);
}

void CCNode::removeSelf()
{
    this->removeFromParentAndCleanup(true);
}

void CCNode::removeFromParent()
{
    this->removeFromParentAndCleanup(true);
}

void CCNode::removeFromParentAndCleanup(bool cleanup)
{
    if (_parent != NULL)
    {
        _parent->removeChild(this,cleanup);
    }
}

void CCNode::removeChild(CCNode* child)
{
    this->removeChild(child, true);
}

/* "remove" logic MUST only be on this method
 * If a class want's to extend the 'removeChild' behavior it only needs
 * to override this method
 */
void CCNode::removeChild(CCNode* child, bool cleanup)
{
    // explicit nil handling
    if (m_pChildren == NULL)
    {
        return;
    }

    if ( m_pChildren->containsObject(child) )
    {
        this->detachChild(child,cleanup);
    }
}

void CCNode::removeChildByTag(int tag)
{
    this->removeChildByTag(tag, true);
}

void CCNode::removeChildByTag(int tag, bool cleanup)
{
    CCAssert( tag != kCCNodeTagInvalid, "Invalid tag");

    CCNode *child = this->getChildByTag(tag);

    if (child == NULL)
    {
        CCLOG("cocos2d: removeChildByTag(tag = %d): child not found!", tag);
    }
    else
    {
        this->removeChild(child, cleanup);
    }
}

void CCNode::removeAllChildren()
{
    this->removeAllChildrenWithCleanup(true);
}

void CCNode::removeAllChildrenWithCleanup(bool cleanup)
{
    // not using detachChild improves speed here
    if ( m_pChildren && m_pChildren->count() > 0 )
    {
        CCObject* child;
        CCARRAY_FOREACH(m_pChildren, child)
        {
            CCNode* pNode = (CCNode*) child;
            if (pNode)
            {
                // IMPORTANT:
                //  -1st do onExit
                //  -2nd cleanup
                if(m_bRunning)
                {
                    pNode->onExit();
                }

                if (cleanup)
                {
                    pNode->cleanup();
                }
                // set parent nil at the end
                pNode->setParent(NULL);
            }
        }

        m_pChildren->removeAllObjects();
    }

}

void CCNode::detachChild(CCNode *child, bool doCleanup)
{
    // IMPORTANT:
    //  -1st do onExit
    //  -2nd cleanup
    if (m_bRunning)
    {
        child->onExit();
    }

    // If you don't do cleanup, the child's actions will not get removed and the
    // its scheduledSelectors_ dict will not get released!
    if (doCleanup)
    {
        child->cleanup();
    }

    // set parent nil at the end
    child->setParent(NULL);

    m_pChildren->removeObject(child);
}


// helper used by reorderChild & add
void CCNode::insertChild(CCNode* child, int z)
{
    m_bReorderChildDirty = true;
    ccArrayAppendObjectWithResize(m_pChildren->data, child);
    child->_setZOrder(z);
}

void CCNode::reorderChild(CCNode *child, int zOrder)
{
    CCAssert( child != NULL, "Child must be non-nil");
    m_bReorderChildDirty = true;
    child->setOrderOfArrival(s_globalOrderOfArrival++);
    child->_setZOrder(zOrder);
}

void CCNode::sortAllChildren()
{
    if (m_bReorderChildDirty)
    {
        int i,j,length = m_pChildren->data->num;
        CCNode ** x = (CCNode**)m_pChildren->data->arr;
        CCNode *tempItem;

        // insertion sort
        for(i=1; i<length; i++)
        {
            tempItem = x[i];
            j = i-1;

            //continue moving element downwards while zOrder is smaller or when zOrder is the same but mutatedIndex is smaller
            while(j>=0 && ( tempItem->m_nZOrder < x[j]->m_nZOrder || ( tempItem->m_nZOrder== x[j]->m_nZOrder && tempItem->m_uOrderOfArrival < x[j]->m_uOrderOfArrival ) ) )
            {
                x[j+1] = x[j];
                j = j-1;
            }
            x[j+1] = tempItem;
        }

        //don't need to check children recursively, that's done in visit of each child

        m_bReorderChildDirty = false;
    }
}


void CCNode::draw()
{
    //CCAssert(0);
    // override me
    // Only use- this function to draw your stuff.
    // DON'T draw your stuff outside this method
}

void CCNode::visit()
{
    m_drawOrder = ++g_drawOrder;
    // quick return if not visible. children won't be drawn.
    if (!m_bVisible)
    {
        return;
    }
    kmGLPushMatrix();

    this->transform();

    CCNode* pNode = NULL;
    unsigned int i = 0;

    if(m_pChildren && m_pChildren->count() > 0)
    {
        sortAllChildren();
        // draw children zOrder < 0
        ccArray *arrayData = m_pChildren->data;
        for( ; i < arrayData->num; i++ )
        {
            pNode = (CCNode*) arrayData->arr[i];

            if ( pNode && pNode->m_nZOrder < 0 )
            {
                pNode->visit();
            }
            else
            {
                break;
            }
        }
        // self draw
        this->draw();

        for( ; i < arrayData->num; i++ )
        {
            pNode = (CCNode*) arrayData->arr[i];
            if (pNode)
            {
                pNode->visit();
            }
        }
    }
    else
    {
        this->draw();
    }

    // reset for next frame
    m_uOrderOfArrival = 0;

    kmGLPopMatrix();
}

void CCNode::transformAncestors()
{
    if( _parent != NULL  )
    {
        _parent->transformAncestors();
        _parent->transform();
    }
}

void CCNode::transform()
{
    kmMat4 transfrom4x4;

    // Convert 3x3 into 4x4 matrix
    CCAffineTransform tmpAffine = this->nodeToParentTransform();
    CGAffineToGL(&tmpAffine, transfrom4x4.mat);

    // Update Z vertex manually
    transfrom4x4.mat[14] = m_fVertexZ;

    kmGLMultMatrix( &transfrom4x4 );


    // XXX: Expensive calls. Camera should be integrated into the cached affine matrix
    if ( m_pCamera != NULL)
    {
        bool translate = (m_obAnchorPointInPoints.x != 0.0f || m_obAnchorPointInPoints.y != 0.0f);

        if( translate )
            kmGLTranslatef(RENDER_IN_SUBPIXEL(m_obAnchorPointInPoints.x), RENDER_IN_SUBPIXEL(m_obAnchorPointInPoints.y), 0 );

        m_pCamera->locate();

        if( translate )
            kmGLTranslatef(RENDER_IN_SUBPIXEL(-m_obAnchorPointInPoints.x), RENDER_IN_SUBPIXEL(-m_obAnchorPointInPoints.y), 0 );
    }

}


void CCNode::onEnter()
{
    //fix setTouchEnabled not take effect when called the function in onEnter in JSBinding.
    m_bRunning = true;

    if (m_scriptEventListeners)
    {
    }

    //Judge the running state for prevent called onEnter method more than once,it's possible that this function called by addChild
    if (m_pChildren && m_pChildren->count() > 0)
    {
        CCObject* child;
        CCNode* node;
        CCARRAY_FOREACH(m_pChildren, child)
        {
            node = (CCNode*)child;
            if (!node->isRunning())
            {
                node->onEnter();
            }
        }
    }

    this->resumeSchedulerAndActions();
}

void CCNode::onExit()
{
    this->pauseSchedulerAndActions();

    m_bRunning = false;

    arrayMakeObjectsPerformSelector(m_pChildren, onExit, CCNode*);

    if (m_scriptEventListeners)
    {
    }
}

void CCNode::setActionManager(CCActionManager* actionManager)
{
    if( actionManager != m_pActionManager ) {
        this->stopAllActions();
        m_pActionManager = actionManager;
    }
}

CCActionManager* CCNode::getActionManager()
{
    return m_pActionManager;
}

CCAction * CCNode::runAction(CCAction* action)
{
    CCAssert( action != NULL, "Argument must be non-nil");
    m_pActionManager->addAction(action, this, !m_bRunning);
    return action;
}

void CCNode::stopAllActions()
{
    m_pActionManager->removeAllActionsFromTarget(this);
}

void CCNode::stopAction(CCAction* action)
{
    m_pActionManager->removeAction(action);
}

void CCNode::stopActionByTag(int tag)
{
    CCAssert( tag != kCCActionTagInvalid, "Invalid tag");
    m_pActionManager->removeActionByTag(tag, this);
}

CCAction * CCNode::getActionByTag(int tag)
{
    CCAssert( tag != kCCActionTagInvalid, "Invalid tag");
    return m_pActionManager->getActionByTag(tag, this);
}

unsigned int CCNode::numberOfRunningActions()
{
    return m_pActionManager->numberOfRunningActionsInTarget(this);
}

// CCNode - Callbacks

void CCNode::setScheduler(CCScheduler* scheduler)
{
    if( scheduler != m_pScheduler ) {
        this->unscheduleAllSelectors();
        m_pScheduler = scheduler;
    }
}

CCScheduler* CCNode::getScheduler()
{
    return m_pScheduler;
}

void CCNode::scheduleUpdate()
{
    scheduleUpdateWithPriority(0);
}

void CCNode::scheduleUpdateWithPriority(int priority)
{
    m_pScheduler->scheduleUpdateForTarget(this, priority, !m_bRunning);
}

void CCNode::unscheduleUpdate()
{
    m_pScheduler->unscheduleUpdateForTarget(this);
}

void CCNode::schedule(SEL_SCHEDULE selector)
{
    this->schedule(selector, 0.0f, kCCRepeatForever, 0.0f);
}

void CCNode::schedule(SEL_SCHEDULE selector, float interval)
{
    this->schedule(selector, interval, kCCRepeatForever, 0.0f);
}

void CCNode::schedule(SEL_SCHEDULE selector, float interval, unsigned int repeat, float delay)
{
    CCAssert( selector, "Argument must be non-nil");
    CCAssert( interval >=0, "Argument must be positive");

    m_pScheduler->scheduleSelector(selector, this, interval , repeat, delay, !m_bRunning);
}

void CCNode::scheduleOnce(SEL_SCHEDULE selector, float delay)
{
    this->schedule(selector, 0.0f, 0, delay);
}

void CCNode::unschedule(SEL_SCHEDULE selector)
{
    // explicit nil handling
    if (selector == 0)
        return;

    m_pScheduler->unscheduleSelector(selector, this);
}

void CCNode::unscheduleAllSelectors()
{
    m_pScheduler->unscheduleAllForTarget(this);
}

void CCNode::resumeSchedulerAndActions()
{
    m_pScheduler->resumeTarget(this);
    m_pActionManager->resumeTarget(this);
}

void CCNode::pauseSchedulerAndActions()
{
    m_pScheduler->pauseTarget(this);
    m_pActionManager->pauseTarget(this);
}

// override me
void CCNode::update(float fDelta)
{
    if (m_scriptEventListeners)
    {
    }
}

CCAffineTransform CCNode::nodeToParentTransform(void)
{
    if (m_bTransformDirty)
    {

        // Translate values
        float x = m_obPosition.x;
        float y = m_obPosition.y;

        if (m_bIgnoreAnchorPointForPosition)
        {
            x += m_obAnchorPointInPoints.x;
            y += m_obAnchorPointInPoints.y;
        }

        // Rotation values
		// Change rotation code to handle X and Y
		// If we skew with the exact same value for both x and y then we're simply just rotating
        float cx = 1, sx = 0, cy = 1, sy = 0;
        if (m_fRotationX || m_fRotationY)
        {
            float radiansX = -CC_DEGREES_TO_RADIANS(m_fRotationX);
            float radiansY = -CC_DEGREES_TO_RADIANS(m_fRotationY);
            cx = cosf(radiansX);
            sx = sinf(radiansX);
            cy = cosf(radiansY);
            sy = sinf(radiansY);
        }

        bool needsSkewMatrix = ( m_fSkewX || m_fSkewY );


        // optimization:
        // inline anchor point calculation if skew is not needed
        // Adjusted transform calculation for rotational skew
        if (! needsSkewMatrix && !m_obAnchorPointInPoints.equals(CCPointZero))
        {
            x += cy * -m_obAnchorPointInPoints.x * m_fScaleX + -sx * -m_obAnchorPointInPoints.y * m_fScaleY;
            y += sy * -m_obAnchorPointInPoints.x * m_fScaleX +  cx * -m_obAnchorPointInPoints.y * m_fScaleY;
        }


        // Build Transform Matrix
        // Adjusted transform calculation for rotational skew
        m_sTransform = CCAffineTransformMake( cy * m_fScaleX,  sy * m_fScaleX,
                                             -sx * m_fScaleY, cx * m_fScaleY,
                                             x, y );

        // XXX: Try to inline skew
        // If skew is needed, apply skew and then anchor point
        if (needsSkewMatrix)
        {
            CCAffineTransform skewMatrix = CCAffineTransformMake(1.0f, tanf(CC_DEGREES_TO_RADIANS(m_fSkewY)),
                                                                 tanf(CC_DEGREES_TO_RADIANS(m_fSkewX)), 1.0f,
                                                                 0.0f, 0.0f );
            m_sTransform = CCAffineTransformConcat(skewMatrix, m_sTransform);

            // adjust anchor point
            if (!m_obAnchorPointInPoints.equals(CCPointZero))
            {
                m_sTransform = CCAffineTransformTranslate(m_sTransform, -m_obAnchorPointInPoints.x, -m_obAnchorPointInPoints.y);
            }
        }

        if (m_bAdditionalTransformDirty)
        {
            m_sTransform = CCAffineTransformConcat(m_sTransform, m_sAdditionalTransform);
            m_bAdditionalTransformDirty = false;
        }

        m_bTransformDirty = false;
    }

    return m_sTransform;
}

void CCNode::setAdditionalTransform(const CCAffineTransform& additionalTransform)
{
    m_sAdditionalTransform = additionalTransform;
    m_bTransformDirty = true;
    m_bAdditionalTransformDirty = true;
}

CCAffineTransform CCNode::parentToNodeTransform(void)
{
    if ( m_bInverseDirty ) {
        m_sInverse = CCAffineTransformInvert(this->nodeToParentTransform());
        m_bInverseDirty = false;
    }

    return m_sInverse;
}

CCAffineTransform CCNode::nodeToWorldTransform()
{
    CCAffineTransform t = this->nodeToParentTransform();

    for (CCNode *p = _parent; p != NULL; p = p->getParent())
        t = CCAffineTransformConcat(t, p->nodeToParentTransform());

    return t;
}

CCAffineTransform CCNode::worldToNodeTransform(void)
{
    return CCAffineTransformInvert(this->nodeToWorldTransform());
}

Vec2 CCNode::convertToNodeSpace(const Vec2& worldPoint)
{
    Vec2 ret = CCPointApplyAffineTransform(worldPoint, worldToNodeTransform());
    return ret;
}

Vec2 CCNode::convertToWorldSpace(const Vec2& nodePoint)
{
    Vec2 ret = CCPointApplyAffineTransform(nodePoint, nodeToWorldTransform());
    return ret;
}

Vec2 CCNode::convertToNodeSpaceAR(const Vec2& worldPoint)
{
    Vec2 nodePoint = convertToNodeSpace(worldPoint);
    return ccpSub(nodePoint, m_obAnchorPointInPoints);
}

Vec2 CCNode::convertToWorldSpaceAR(const Vec2& nodePoint)
{
    Vec2 pt = ccpAdd(nodePoint, m_obAnchorPointInPoints);
    return convertToWorldSpace(pt);
}

Vec2 CCNode::convertToWindowSpace(const Vec2& nodePoint)
{
    Vec2 worldPoint = this->convertToWorldSpace(nodePoint);
    return CCDirector::sharedDirector()->convertToUI(worldPoint);
}

// convenience methods which take a CCTouch instead of CCPoint
Vec2 CCNode::convertTouchToNodeSpace(CCTouch *touch)
{
    Vec2 point = touch->getLocation();
    return this->convertToNodeSpace(point);
}

Vec2 CCNode::convertTouchToNodeSpaceAR(CCTouch *touch)
{
    Vec2 point = touch->getLocation();
    return this->convertToNodeSpaceAR(point);
}

void CCNode::updateTransform()
{
    m_drawOrder = ++g_drawOrder;
    // Recursively iterate over children
    arrayMakeObjectsPerformSelector(m_pChildren, updateTransform, CCNode*);
}
// merge CCNodeRGBA to CCNode

GLubyte CCNode::getOpacity(void)
{
	return m_realOpacity;
}

GLubyte CCNode::getDisplayedOpacity(void)
{
	return m_displayedOpacity;
}

void CCNode::setOpacity(GLubyte opacity)
{
    m_displayedOpacity = m_realOpacity = opacity;

	if (m_cascadeOpacityEnabled)
    {
		GLubyte parentOpacity = 255;
        if (_parent && _parent->isCascadeOpacityEnabled())
        {
            parentOpacity = _parent->getDisplayedOpacity();
        }
        this->updateDisplayedOpacity(parentOpacity);
    }
}

void CCNode::setOpacityModifyRGB(bool var)
{
    m_isOpacityModifyRGB = var;
    if (m_pChildren && m_pChildren->count() != 0)
    {
        CCObject* child;
        CCARRAY_FOREACH(m_pChildren, child)
        {
            dynamic_cast<CCNode*>(child)->setOpacityModifyRGB(var);
        }
    }
}

bool CCNode::isOpacityModifyRGB(void)
{
    return m_isOpacityModifyRGB;
}

void CCNode::updateDisplayedOpacity(GLubyte parentOpacity)
{
	m_displayedOpacity = (GLubyte)(m_realOpacity * parentOpacity/255.0);

    if (m_cascadeOpacityEnabled)
    {
        CCObject* pObj;
        CCARRAY_FOREACH(m_pChildren, pObj)
        {
            dynamic_cast<CCNode*>(pObj)->updateDisplayedOpacity(m_displayedOpacity);
        }
    }
}

bool CCNode::isCascadeOpacityEnabled(void)
{
    return m_cascadeOpacityEnabled;
}

void CCNode::setCascadeOpacityEnabled(bool cascadeOpacityEnabled)
{
    m_cascadeOpacityEnabled = cascadeOpacityEnabled;
}

const ccColor3B& CCNode::getColor(void)
{
	return m_realColor;
}

const ccColor3B& CCNode::getDisplayedColor()
{
	return m_displayedColor;
}

void CCNode::setColor(const ccColor3B& color)
{
	m_displayedColor = m_realColor = color;

	if (m_cascadeColorEnabled)
    {
		ccColor3B parentColor = ccWHITE;
		if (_parent && _parent->isCascadeColorEnabled())
        {
            parentColor = _parent->getDisplayedColor();
        }

        updateDisplayedColor(parentColor);
	}
}

void CCNode::updateDisplayedColor(const ccColor3B& parentColor)
{
	m_displayedColor.r = (GLubyte)(m_realColor.r * (float)parentColor.r/255.0f);
	m_displayedColor.g = (GLubyte)(m_realColor.g * (float)parentColor.g/255.0f);
	m_displayedColor.b = (GLubyte)(m_realColor.b * (float)parentColor.b/255.0f);

    if (m_cascadeColorEnabled)
    {
        CCObject *obj = NULL;
        CCARRAY_FOREACH(m_pChildren, obj)
        {
            dynamic_cast<CCNode*>(obj)->updateDisplayedColor(m_displayedColor);
        }
    }
}

bool CCNode::isCascadeColorEnabled(void)
{
    return m_cascadeColorEnabled;
}

void CCNode::setCascadeColorEnabled(bool cascadeColorEnabled)
{
    m_cascadeColorEnabled = cascadeColorEnabled;
}

// ----------------------------------------

CCScene *CCNode::getScene()
{
    if (!m_bRunning) return NULL;
    CCNode *parent = getParent();
    if (!parent) return NULL;

    CCNode *scene = parent;
    while (parent)
    {
        parent = parent->getParent();
        if (parent) scene = parent;
    }
    return dynamic_cast<CCScene*>(scene);
}

bool CCNode::onTouchBegan(int id, const Vec2& pos)
{
    if (m_scriptEventListeners)
    {
        return !!executeScriptTouchHandler(CCTOUCHBEGAN, pos);
    }
    else
    {
        return false;
    }
}

void CCNode::onTouchMoved(int id, const Vec2& pos)
{
    if (m_scriptEventListeners)
    {
        executeScriptTouchHandler(CCTOUCHMOVED, pos);
    }
}

void CCNode::onTouchEnded(int id, const Vec2& pos)
{
    if (m_scriptEventListeners)
    {
        executeScriptTouchHandler(CCTOUCHENDED, pos);
    }
}

bool CCNode::isTouchEnabled()
{
    return _touchEnabled;
}

void CCNode::setTouchEnabled(bool enabled)
{
	_touchEnabled = enabled;
}

int CCNode::executeScriptTouchHandler(int nEventType, const Vec2& pos)
{
    return CCScriptEngineManager::sharedManager()->getScriptEngine()->executeNodeTouchEvent(this, nEventType, pos, 0);
}

void CCNode::registAttribute()
{
	ATTR("Name",   getName,     setName, std::string, "");
	ATTR("Tag",    getTag,      setTag, int, 0);
	ATTR("Rotate", getRotation, setRotation, float, 0.0f);
    ATTR("Scale",  getScale,    setScale,   float, 1.0f);
    ATTR("Pos",    getPosition, setPosition, Vec2,  CCPointZero);
    ATTR("Anchor", getAnchorPoint, setAnchorPoint, Vec2, Vec2(0.5f, 0.5f));
    ATTR("ZOrder", getZOrder,   setZOrder, int, 0);
    ATTR("Visible",isVisible,   setVisible, bool, true);
}

NS_CC_END
