#include "stdafx.h"
#include "apollo_shared.h"

dllgm void lua_global_get(YYResult& result, lua_State* L, const char* name) {
	lua_getglobal(L, name);
	Apollo::popLuaStackValue(&result, L);
}

dllgm void lua_global_set(lua_State* L, const char* name, RValue* value) {
	Apollo::pushGmlValueToLuaStack(value, L);
	lua_setglobal(L, name);
}

dllgm void lua_global_call(YYResult& result, lua_State* L, const char* fname, YYRest args) {
	lua_getglobal(L, fname);
	Apollo::pushGmlValuesToLuaStack(args.items, args.length, L);
	dllm_handle_lua_error(lua_pcall(L, args.length, 1, 0));
	Apollo::popLuaStackValue(&result, L);
}

dllgm void lua_global_call_multret(YYResult& result, lua_State* L, const char* fname, YYRest args) {
	lua_getglobal(L, fname);
	Apollo::pushGmlValuesToLuaStack(args.items, args.length, L);
	dllm_handle_lua_error(lua_pcall(L, args.length, LUA_MULTRET, 0));
	Apollo::popLuaStackValuesAsArray(&result, L);
}

dllgm void lua_global_call_ext(YYResult& result, lua_State* L, const char* fname,
	YYArrayItems argArray, int offset = 0, int numArgs = -1
) {
	lua_getglobal(L, fname);
	argArray.procCountOffset(&numArgs, &offset);
	Apollo::pushGmlValuesToLuaStack(argArray.items + offset, numArgs, L);
	dllm_handle_lua_error(lua_pcall(L, numArgs, 1, 0));
	Apollo::popLuaStackValue(&result, L);
}

dllgm void lua_global_call_ext_multret(YYResult& result, lua_State* L, const char* fname,
	YYArrayItems argArray, int offset = 0, int numArgs = -1
) {
	lua_getglobal(L, fname);
	argArray.procCountOffset(&numArgs, &offset);
	Apollo::pushGmlValuesToLuaStack(argArray.items + offset, numArgs, L);
	dllm_handle_lua_error(lua_pcall(L, numArgs, LUA_MULTRET, 0));
	Apollo::popLuaStackValuesAsArray(&result, L);
}
