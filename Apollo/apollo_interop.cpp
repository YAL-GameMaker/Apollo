#include "stdafx.h"
#include "apollo_shared.h"
#define __RELFILE__ "apollo_interop.cpp"

namespace Apollo {
	void initLuaRef(lua_State* L);
	void initArrayRef(lua_State* L);
	void initStructRef(lua_State* L);
}
void apollo_interop_init(ApolloState* wrapState, lua_State* L) {
	luaL_newmetatable(L, "ApolloState");
	luaL_newmetatable(L, "RValuePtr");
	lua_pop(L, 2);

	// reg.apolloState = ApolloState(wrapState)
	lua_pushstring(L, "apolloState");
	auto ptr = lua_newuserdata_t<ApolloState*>(L);
	*ptr = wrapState;
	luaL_getmetatable(L, "ApolloState");
	lua_setmetatable(L, -2);
	lua_settable(L, LUA_REGISTRYINDEX);

	Apollo::initLuaRef(L);
	Apollo::initArrayRef(L);
	Apollo::initStructRef(L);
	if (lua_gettop(L) != 0) trace("unbalanced stack after interop_init: %s", Apollo::printLuaStack(L));
}