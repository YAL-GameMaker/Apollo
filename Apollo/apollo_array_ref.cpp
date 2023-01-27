#include "stdafx.h"
#include "apollo_shared.h"

namespace Apollo {
	static int64_t getUID(lua_State* L, int ind) {
		auto ptr = (int64_t*)luaL_testudata(L, ind, "GmlArray");
		return ptr ? *ptr : 0;
	}
	template<size_t argc> static void setupCall(RValue(&args)[argc], lua_State* L, int script_id) {
		auto state = Apollo::getState(L);
		auto uid = getUID(L, 1);
		static_assert(argc >= 3, "Not enough arguments!");
		args[0].setScriptID(script_id);
		args[1].setTo(state->selfValue);
		args[2].setInt64(uid);
	}
	static int createGmlArrayUD(lua_State* L) {
		auto uid = lua_tointeger(L, 1);
		auto ptr = lua_newuserdata_t<int64_t>(L);
		*ptr = uid;
		luaL_getmetatable(L, "GmlArray");
		lua_setmetatable(L, -2);
		return 1;
	}
	static int __gc(lua_State* L) {
		static RValue args[3], result;
		setupCall(args, L, gml_Script.lua_gml_ref_free);
		GML::script_execute_def(result, args);
		args[1].free();
		return 0;
	}
	static int __len(lua_State* L) {
		static RValue args[3], result;
		setupCall(args, L, gml_Script.lua_gml_ref_get_length);
		GML::script_execute_def_for(result, args, L);
		args[1].free();
		Apollo::pushGMLtoLuaStack(&result, L);
		return 1;
	}
	static int __index(lua_State* L) {
		static RValue args[4], result;
		setupCall(args, L, gml_Script.lua_gml_ref_get_index);
		Apollo::luaToGML(&args[3], L, 2);
		GML::script_execute_def_for(result, args, L);
		args[1].free();
		args[3].free();
		Apollo::pushGMLtoLuaStack(&result, L);
		return 1;
	}
	static int __newindex(lua_State* L) {
		static RValue args[5], result;
		setupCall(args, L, gml_Script.lua_gml_ref_set_index);
		Apollo::luaToGML(&args[3], L, 2);
		Apollo::luaToGML(&args[4], L, 3);
		GML::script_execute_def_for(result, args, L);
		args[1].free();
		args[3].free();
		args[4].free();
		return 0;
	}
	static luaL_Reg metaPairs[] = {
		{ "__gc", __gc },
		{ "__len", __len },
		{ "__index", __index },
		{ "__newindex", __newindex },
		{ 0, 0 }
	};
	void initArrayRef(lua_State* L) {
		luaL_newmetatable(L, "GmlArray");
		luaL_setfuncs(L, metaPairs, 0);
		lua_pop(L, 1);

		lua_pushcfunction(L, createGmlArrayUD);
		lua_setglobal(L, "__apollo_tmp");

		luaL_dostring_trace(L, "lua_array_ref", R"lua(
(function()
	local _create_udata = __apollo_tmp
	__apollo_tmp = nil
	
	local _uid_to_udata = setmetatable({}, { __mode = "v" })
	__apollo_get_gml_array_udata = function(uid)
		local ud = _uid_to_udata[uid]
		if (ud == nil) then
			ud = _create_udata(uid)
			_uid_to_udata[uid] = ud
		end
		return ud
	end
end)()
)lua")
	}
}