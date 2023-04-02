#include "stdafx.h"
#include "apollo_shared.h"

dllgm int lua_stack_size(lua_State* L) {
	return lua_gettop(L);
}
dllgm void lua_stack_discard(lua_State* L, int count) {
	lua_pop(L, count);
}
dllgm void lua_stack_clear(lua_State* L) {
	lua_settop(L, 0);
}
dllgm void lua_stack_resize(lua_State* L, int index) {
	lua_settop(L, index);
}
dllgm bool lua_stack_ensure(lua_State* L, int numElements) {
	return lua_checkstack(L, numElements);
}

dllgm void lua_stack_get(YYResult& result, lua_State* L, int index) {
	Apollo::luaToGML(&result, L, index);
}

// pop

dllgm void lua_stack_pop(YYResult& result, lua_State* L) {
	Apollo::popLuaStackValue(&result, L);
}
dllgm void lua_stack_pop_multret(YYResult& result, lua_State* L, int count = -1) {
	Apollo::popLuaStackValuesAsArray(&result, L, count);
}
dllgm void lua_stack_pop_to_global(lua_State* L, const char* name) {
	lua_setglobal(L, name);
}

// push

dllgm void lua_stack_push(lua_State* L, YYRest values) {
	Apollo::pushGmlValuesToLuaStack(values.items, values.length, L);
}
dllgm int lua_stack_push_ext(lua_State* L, YYArrayItems valArray, int offset = 0, int count = -1) {
	valArray.procCountOffset(&count, &offset);
	Apollo::pushGmlValuesToLuaStack(valArray.items + offset, count, L);
	return count;
}
dllgm void lua_stack_push_global(lua_State* L, const char* name) {
	lua_getglobal(L, name);
}
dllgm void lua_stack_push_new_table(lua_State* L) {
	lua_newtable(L);
}

// calls

dllgm void lua_raw_call(YYResult& result, lua_State* L, int numArgs) {
	dllm_handle_lua_error(lua_pcall(L, numArgs, 1, 0));
	Apollo::popLuaStackValue(&result, L);
}
dllgm void lua_raw_call_multret(YYResult& result, lua_State* L, int numArgs) {
	dllm_handle_lua_error(lua_pcall(L, numArgs, LUA_MULTRET, 0));
	Apollo::popLuaStackValuesAsArray(&result, L);
}

dllgm bool lua_raw_call_ext(lua_State* L, int numArgs, int numResults) {
	if (lua_pcall(L, numArgs, numResults, 0)) {
		Apollo::handleLuaError(L);
		return false;
	}
	return true;
}
dllgm void lua_raw_handle_error(lua_State* L) {
	Apollo::handleLuaError(L);
}

// closures
static int gml_closure(lua_State* L) {
	RValue result{}, args[2]{};
	args[0].setScriptID(lua_tointeger(L, lua_upvalueindex(1)));
	args[1].setTo(Apollo::getState(L)->selfValue);
	GML::script_execute_def_for(result, args, L);
	auto ret = result.getInt32();
	result.free();
	args[1].free();
	return ret;
}
dllgm void lua_stack_push_closure(lua_State* L, RValue* script_index, int upvalueCount) {
	int _script_index;

	if (script_index->getKind() == VALUE_OBJECT) {
		RValue result{}, args[1]{};
		args[0].setTo(script_index);
		gml_Func.is_method(result, 0, 0, 1, args);
		if (!result.getBool()) __YYArgError("script_index", "script index or method", 1)
	}
	RValue result{}, args[1]{};
	args[0].setTo(script_index);
	gml_Func.is_method(result, 0, 0, 1, args);
	if (result.getBool()) {

	}
	args[0].free();
	lua_pushinteger(L, script_index);
	if (upvalueCount > 0) lua_absindex(L, -(upvalueCount + 1));
	lua_pushcclosure(L, gml_closure, upvalueCount + 1);
}
dllgm void lua_local_upvalue_get(YYResult& result, lua_State* L, int upvalueIndex) {
	Apollo::luaToGML(&result, L, lua_upvalueindex(1 + upvalueIndex));
}
dllgm void lua_local_upvalue_set(lua_State* L, int upvalueIndex, RValue* value) {
	Apollo::pushGmlValueToLuaStack(value, L);
	lua_copy(L, -1, lua_upvalueindex(1 + upvalueIndex));
	lua_pop(L, 1);
}
dllgm void lua_local_upvalue_push_to_stack(lua_State* L, int upvalueIndex) {
	lua_pushvalue(L, lua_upvalueindex(1 + upvalueIndex));
}
dllgm void lua_local_upvalue_set_from_stack(lua_State* L, int upvalueIndex) {
	lua_copy(L, -1, lua_upvalueindex(1 + upvalueIndex));
	lua_pop(L, 1);
}