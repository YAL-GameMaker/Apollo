#include "stdafx.h"
#include "apollo_shared.h"
#include "apollo_lua_ref.lua.h"
#define __RELFILE__ "apollo_lua_ref.cpp"
// A wrapper for a Lua reference-type (tables, functions, userdata) for use in GML.
// The GML half of this is LuaRef.

dllm void lua_ref_create_post(YYFuncArgs) {
	YYStructAddRValue(&arg[0], "@@Dispose@@", &arg[1]);
}
namespace Apollo {
	static int refConstructorsPerType[LUA_NUMTYPES] = { -1 };
	static void refConstructorsPerType_init() {
		for (int i = 0; i < std::size(refConstructorsPerType); i++) {
			refConstructorsPerType[i] = gml_Script.lua_ref_create_raw;
		}
		refConstructorsPerType[LUA_TTABLE] = gml_Script.lua_ref_create_raw_table;
		refConstructorsPerType[LUA_TFUNCTION] = gml_Script.lua_ref_create_raw_function;
		refConstructorsPerType[LUA_TUSERDATA] = gml_Script.lua_ref_create_raw_userdata;
	}
	static RValue* refResult;
	static int createLuaRefStruct(lua_State* L) {
		auto state = Apollo::getState(L);
		auto uid = lua_tointeger(L, 1);
		auto type = lua_type(L, 2);

		if (refConstructorsPerType[0] < 0) refConstructorsPerType_init();
		auto scriptID = refConstructorsPerType[type];

		RValue args[3], result;
		args[0].setScriptID(scriptID);
		args[1].setTo(state->selfValue);
		args[2].setInt64(uid);
		GML::script_execute_def_for(result, args, L);
		
		auto ptr = lua_newuserdata_t<RValue*>(L);
		*ptr = YYStructGetMember(&result, "__self__");
		luaL_getmetatable(L, "RValuePtr");
		lua_setmetatable(L, -2);

		result.free();
		args[1].free();
		return 1;
	}
	void initLuaRef(lua_State* L) {
		lua_pushcfunction(L, createLuaRefStruct);
		lua_setglobal(L, "__apollo_tmp");

		luaL_dostring_trace(L, "lua_ref", __lua_ref_init);
	}

	void createLuaRef(RValue* result, lua_State* L, int ind) {
		lua_getglobal(L, "__apollo_ref_to_rvalue");
		lua_pushvalue(L, ind > 0 ? ind : ind - 1);
		if (lua_pcall(L, 1, 1, 0)) {
			trace("Error creating ref! %s", lua_tostring(L, -1));
			result->kind = VALUE_UNDEFINED;
			result->ptr = nullptr;
			return;
		}
		auto ptr = (RValue**)luaL_testudata(L, -1, "RValuePtr");
		if (ptr) {
			COPY_RValue(result, *ptr);
		} else {
			result->kind = VALUE_UNDEFINED;
			result->ptr = nullptr;
		}
		lua_pop(L, 1);
	}
}
