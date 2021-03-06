#include "LuaFunction.h"
#include "CCPlatformMacros.h"
#include "ccMacros.h"
#include <assert.h>
#include "tolua++.h"
#include "CCLuaEngine.h"

namespace cocos2d
{

LuaFunction::LuaFunction(lua_State* L, int index) :
    _luaState(L),
    _numArguments(-1)
{
    assert(L);
    lua_pushvalue(L, index);
    _functionRef = luaL_ref(L, LUA_REGISTRYINDEX);
}

LuaFunction::LuaFunction(lua_State* L, lua_CFunction func) :
    _luaState(L),
    _numArguments(-1)
{
    assert(L);
    lua_pushcfunction(L, func);
    _functionRef = luaL_ref(L, LUA_REGISTRYINDEX);
}

LuaFunction::LuaFunction(int index) :
    _luaState(CCLuaEngine::defaultEngine()->getLuaStack()->getLuaState())
{
    lua_pushvalue(_luaState, index);
    _functionRef = luaL_ref(_luaState, LUA_REGISTRYINDEX);
}

LuaFunction::~LuaFunction()
{
    luaL_unref(_luaState, LUA_REGISTRYINDEX, _functionRef);
    _functionRef = LUA_NOREF;
}

bool LuaFunction::IsValid() const
{
    return _functionRef != LUA_REFNIL && _functionRef != LUA_NOREF;
}

bool LuaFunction::BeginCall()
{
    if (!IsValid())
        return false;

    lua_rawgeti(_luaState, LUA_REGISTRYINDEX, _functionRef);
    _numArguments = 0;

    return true;
}

bool LuaFunction::EndCall(int numReturns)
{
    assert(_numArguments >= 0);
    int numArguments = _numArguments;
    _numArguments = -1;

	CCLuaEngine::defaultEngine()->getLuaStack()->executeFunction(numArguments);

    return true;
}

void LuaFunction::PushInt(int value)
{
    assert(_numArguments >= 0);
    ++_numArguments;
    lua_pushinteger(_luaState, value);
}

void LuaFunction::PushBool(bool value)
{
    assert(_numArguments >= 0);
    ++_numArguments;
    lua_pushboolean(_luaState, value);
}

void LuaFunction::PushFloat(float value)
{
    assert(_numArguments >= 0);
    ++_numArguments;
    lua_pushnumber(_luaState, value);
}

void LuaFunction::PushDouble(double value)
{
    assert(_numArguments >= 0);
    ++_numArguments;
    lua_pushnumber(_luaState, value);
}

void LuaFunction::PushString(const char* value)
{
    assert(_numArguments >= 0);
    ++_numArguments;
    lua_pushstring(_luaState, value);
}

void LuaFunction::PushUserType(void* userType, const char* typeName)
{
    assert(_numArguments >= 0);
    ++_numArguments;
    tolua_pushusertype(_luaState, userType, typeName);
}

void LuaFunction::PushLuaTable(const LuaTable* t)
{
    assert(_numArguments >= 0);
    ++_numArguments;
	// doing nothing.
}

}
