
#pragma once

#include "CCPlatformDefine.h"
#include "CCPlatformMacros.h"

#include <string>
#include "nodes/CCLabelAtlas.h"

NS_CC_BEGIN

class CC_DLL DebugHud : public SubSystem
{
	CCOBJECT(DebugHud, SubSystem)
public:
	DebugHud();
private:

private:

	void createLabels();
	void render(EventData& map);

	void updateLabel(EventData& map);
	float _inteval;
	float _frames;

	SharedPtr<CCLabelAtlas> _pFPSLabel;
	SharedPtr<CCLabelAtlas> _pDrawsLabel;
	SharedPtr<CCLabelAtlas> _pSPFLabel;
};


NS_CC_END
