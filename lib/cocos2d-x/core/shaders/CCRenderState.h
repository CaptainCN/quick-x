
#pragma once
#include "engine/CCObject.h"
#include "CCGLProgram.h"

NS_CC_BEGIN


class ShaderParam : public RefCounted
{
public:

    enum
    {
        INT1,INT2,INT3,INT4,
        FLOAT1,FLOAT2,FLOAT3,FLOAT4,
		MAT,
        SAMPLER
    } type;

	ShaderParam(Uniform* u) : _uniform(u) {};

	void set1i(int v1);
	void set2i(int v1, int v2);
	void set3i(int v1, int v2, int v3);
	void set4i(int v1, int v2, int v3, int v4);
	void set1f(float v1);
	void set2f(float v1, float v2);
	void set3f(float v1, float v2, float v3);
	void set4f(float v1, float v2, float v3, float v4);

	void setmat(float* mat);
    void setSampler(unsigned i);
private:
	friend class RenderState;
	Uniform* _uniform;
    VariantValue value;
};

class RenderState : public CCObject
{
public:
	RenderState(CCGLProgram* program);

	CCGLProgram* getGLProgram() { return _program; }
	ShaderParam* getParam(const char* name);

	void set1i(const char* name, int v1);
	void set2i(const char* name, int v1, int v2);
	void set3i(const char* name, int v1, int v2, int v3);
	void set4i(const char* name, int v1, int v2, int v3, int v4);

	void set1f(const char* name, float v1);
	void set2f(const char* name, float v1, float v2);
	void set3f(const char* name, float v1, float v2, float v3);
	void set4f(const char* name, float v1, float v2, float v3, float v4);

	void use();
private:
	void bindValue();

	ShaderParam* _matrixP;
	ShaderParam* _matrixMV;
	ShaderParam* _matrixMVP;
	std::map<std::string, SharedPtr<ShaderParam> > _params;
	CCGLProgram* _program;
};

NS_CC_END