/****************************************************************************
Copyright (c) 2010 cocos2d-x.org

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

#ifndef __CCGEMETRY_H__
#define __CCGEMETRY_H__

#include "graphics/CCVec2.h"
#include "engine/CCObject.h"
#include <math.h>

NS_CC_BEGIN

/**
 * @js NA
 */
class CC_DLL CCSize
{
public:
    float width;
    float height;

public:
    CCSize();
    CCSize(float width, float height);
    /**
     * @lua NA
     */
    CCSize(const CCSize& other);
    /**
     * @lua NA
     */
    CCSize(const Vec2& point);
    /**
     * @lua NA
     */
    CCSize& operator= (const CCSize& other);
    /**
     * @lua NA
     */
    CCSize& operator= (const Vec2& point);
    /**
     * @lua NA
     */
    CCSize operator+(const CCSize& right) const;
    /**
     * @lua NA
     */
    CCSize operator-(const CCSize& right) const;
    /**
     * @lua NA
     */
    CCSize operator*(float a) const;
    /**
     * @lua NA
     */
    CCSize operator/(float a) const;
    /**
     * @lua NA
     */
    void setSize(float width, float height);
    /**
     * @lua NA
     */
    bool equals(const CCSize& target) const;
};

/**
 * @js NA
 */
class CC_DLL CCRect
{
public:
    Vec2 origin;
    CCSize  size;

public:
    CCRect();
    CCRect(float x, float y, float width, float height);
    /**
     * @lua NA
     */
    CCRect(const CCRect& other);
    /**
     * @lua NA
     */
    CCRect& operator= (const CCRect& other);
    void setRect(float x, float y, float width, float height);
    float getMinX() const; /// return the leftmost x-value of current rect
    float getMidX() const; /// return the midpoint x-value of current rect
    float getMaxX() const; /// return the rightmost x-value of current rect
    float getMinY() const; /// return the bottommost y-value of current rect
    float getMidY() const; /// return the midpoint y-value of current rect
    float getMaxY() const; /// return the topmost y-value of current rect
    bool equals(const CCRect& rect) const;   
    bool containsPoint(const Vec2& point) const;
    bool intersectsRect(const CCRect& rect) const;
    void merge(const CCRect& rect);
};


#define CCPointMake(x, y) Vec2((float)(x), (float)(y))
#define CCSizeMake(width, height) CCSize((float)(width), (float)(height))
#define CCRectMake(x, y, width, height) CCRect((float)(x), (float)(y), (float)(width), (float)(height))


const Vec2 CCPointZero = CCPointMake(0,0);

/* The "zero" size -- equivalent to CCSizeMake(0, 0). */ 
const CCSize CCSizeZero = CCSizeMake(0,0);

/* The "zero" rectangle -- equivalent to CCRectMake(0, 0, 0, 0). */ 
const CCRect CCRectZero = CCRectMake(0,0,0,0);

// end of data_structure group
/// @}

NS_CC_END

#endif // __CCGEMETRY_H__
