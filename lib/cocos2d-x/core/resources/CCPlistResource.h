
#pragma once

#include "CCPlatformMacros.h"
#include "engine/CCResourceCache.h"
#include "resources/CCSpriteFrame.h"

NS_CC_BEGIN

class CCDictionary;
class CCTexture2D;

class PlistResource : public Resource
{
	CCOBJECT(PlistResource, Resource);

public:

	virtual void beginLoad(MemBuffer* buf, void* userdata);
	
	CCTexture2D* getTexture() { return _texture; }

    CCSpriteFrame* getFrame(const std::string& name);
    
private:

	std::map<std::string, SharedPtr<CCSpriteFrame> > _frames;
	SharedPtr<CCTexture2D> _texture;
};

NS_CC_END
