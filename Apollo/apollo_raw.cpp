#include "stdafx.h"
#include "apollo_shared.h"

dllgm void lua_stack_discard(lua_State* L, int count) {
	lua_pop(L, count);
}
dllgm void lua_stack_clear(lua_State* L) {
	lua_settop(L, 0);
}

dllgm int lua_stack_size(lua_State* L) {
	return lua_gettop(L);
}
dllgm void lua_stack_resize(lua_State* L, int index) {
	lua_settop(L, index);
}

dllgm void lua_stack_get(YYResult& result, lua_State* L, int index) {
	Apollo::luaToGML(&result, L, index);
}

dllgm void lua_stack_pop(YYResult& result, lua_State* L) {
	Apollo::popLuaStackValue(&result, L);
}
dllgm void lua_stack_pop_multret(YYResult& result, lua_State* L, int count = -1) {
	Apollo::popLuaStackValuesAsArray(&result, L, count);
}

dllgm void lua_stack_push(lua_State* L, YYRest values) {
	for (int i = 0; i < values.length; i++) Apollo::pushGMLtoLuaStack(&values[i], L);
}
dllgm int lua_stack_push_ext(lua_State* L, YYArrayItems valArray, int offset = 0, int count = -1) {
	valArray.procCountOffset(&count, &offset);
	auto till = offset + count;
	for (int i = offset; i < till; i++) Apollo::pushGMLtoLuaStack(&valArray[i], L);
	return count;
}
dllgm void lua_stack_push_global(lua_State* L, const char* name) {
	lua_getglobal(L, name);
}

dllgm void lua_rawcall(YYResult& result, lua_State* L, int numArgs) {
	dllm_handle_lua_error(lua_pcall(L, numArgs, 1, 0));
	Apollo::popLuaStackValue(&result, L);
}
dllgm void lua_rawcall_multret(YYResult& result, lua_State* L, int numArgs) {
	dllm_handle_lua_error(lua_pcall(L, numArgs, LUA_MULTRET, 0));
	Apollo::popLuaStackValuesAsArray(&result, L);
}
dllgm int lua_rawcall_ext(lua_State* L, int numArgs, int numResults) {
	return lua_pcall(L, numArgs, numResults, 0) == LUA_OK;
}
dllgm void lua_handle_rawerror(lua_State* L) {
	Apollo::handleLuaError(L);
}