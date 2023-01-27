#include "stdafx.h"
#include "apollo_shared.h"

namespace Apollo {
	ApolloState* getState(lua_State* L) {
		lua_pushstring(L, "apolloState");
		lua_gettable(L, LUA_REGISTRYINDEX);
		auto ptr = (ApolloState**)luaL_checkudata(L, -1, "ApolloState");
		lua_pop(L, 1);
		return ptr ? *ptr : nullptr;
	}
}

void apollo_interop_init(ApolloState* wrapState, lua_State* L);
dllgm void* lua_state_create_raw(RValue* gmlState, RValue* destructor) {
	auto L = luaL_newstate();
	auto wrapState = new ApolloState(L, nullptr);
	wrapState->selfValue = YYStructGetMember(gmlState, "__self__");
	wrapState->callArgs = YYStructGetMember(gmlState, "__call_args");
	YYStructAddRValue(gmlState, "@@Dispose@@", destructor);
	luaL_openlibs(L);
	apollo_interop_init(wrapState, L);
	return wrapState;
}

dllm void lua_state_destroy_raw(YYFuncArgs) {
	auto state = arg[0].getApolloState();
	if (state) delete state;
}

dllgm void lua_add_code(YYResult& result, lua_State* L, const char* code) {
	dllm_handle_lua_error(luaL_loadstring(L, code) || lua_pcall(L, 0, 1, 0));
	Apollo::popLuaStackValue(&result, L);
}

dllgm void lua_add_code_multret(YYResult& result, lua_State* L, const char* code) {
	dllm_handle_lua_error(luaL_loadstring(L, code) || lua_pcall(L, 0, LUA_MULTRET, 0));
	Apollo::popLuaStackValuesAsArray(&result, L);
}

dllgm bool is_lua_ref(RValue* val) {
	if ((val->kind & MASK_KIND_RVALUE) == VALUE_OBJECT) {
		return gmlClassOf.isLuaRef(val->getObjectClass());
	} else return false;
}