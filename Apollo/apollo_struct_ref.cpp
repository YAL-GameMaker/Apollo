#include "stdafx.h"
#include "apollo_shared.h"

namespace Apollo {
	struct GmlStructData {
		int64_t uid = 0;
		bool isCrossRef = false;
	};
	static inline GmlStructData getData(lua_State* L, int ind = 1) {
		auto ptr = (GmlStructData*)luaL_testudata(L, ind, "GmlStruct");
		if (ptr == nullptr) return {};
		return *ptr;
	}
	template<size_t argc> static void setupCall(RValue(&args)[argc], int64_t uid, lua_State* L, int script_id, ApolloState* state = nullptr) {
		if (state == nullptr) state = Apollo::getState(L);
		static_assert(argc >= 3, "Not enough arguments!");
		args[0].setScriptID(script_id);
		args[1].setTo(state->selfValue);
		args[2].setInt64(uid);
	}
	static int createGmlStructUD(lua_State* L) {
		auto uid = lua_tointeger(L, 1);
		auto isCrossRef = lua_toboolean(L, 2);
		auto ptr = lua_newuserdata_t<GmlStructData>(L);
		ptr->uid = uid;
		ptr->isCrossRef = isCrossRef;
		luaL_getmetatable(L, "GmlStruct");
		lua_setmetatable(L, -2);
		return 1;
	}
	static int __gc(lua_State* L) {
		auto inf = getData(L);
		static RValue args[3], result;
		setupCall(args, inf.uid, L, gml_Script.lua_gml_ref_free);
		GML::script_execute_def(result, args);
		args[1].free();
		return 0;
	}
	static int __index(lua_State* L) {
		auto inf = getData(L);
		static RValue args[4], result;
		auto script = inf.isCrossRef ? gml_Script.lua_gml_cross_ref_get_key : gml_Script.lua_gml_ref_get_key;
		setupCall(args, inf.uid, L, script);
		Apollo::luaToGML(&args[3], L, 2);
		GML::script_execute_def_for(result, args, L);
		args[1].free();
		args[3].free();
		Apollo::pushGmlValueToLuaStack(&result, L);
		return 1;
	}
	static int __newindex(lua_State* L) {
		auto inf = getData(L);
		static RValue args[5], result;
		auto script = inf.isCrossRef ? gml_Script.lua_gml_cross_ref_set_key : gml_Script.lua_gml_ref_set_key;
		setupCall(args, inf.uid, L, script);
		Apollo::luaToGML(&args[3], L, 2);
		Apollo::luaToGML(&args[4], L, 3);
		GML::script_execute_def_for(result, args, L);
		args[1].free();
		args[3].free();
		args[4].free();
		return 0;
	}
	static int __call(lua_State* L) {
		auto inf = getData(L);
		static RValue args[5], result;
		auto state = Apollo::getState(L);
		auto script = inf.isCrossRef ? gml_Script.lua_gml_cross_ref_invoke : gml_Script.lua_gml_ref_invoke;
		setupCall(args, inf.uid, L, script, state);

		// make sure that argument array is big enough:
		auto argc = lua_gettop(L) - 1;
		auto argArr = state->callArgs;
		if (argArr->getArrayLength() < argc) {
			SET_RValue(argArr, &args[0], (YYObjectBase*)state->selfValue->ptr, argc - 1);
		}

		auto argItems = argArr->getArrayItems();
		for (int i = 0; i < argc; i++) {
			Apollo::luaToGML(&argItems[i], L, 2 + i);
		}
		args[3].setTo(argArr);
		args[4].setReal(argc);

		GML::script_execute_def_for(result, args, L);

		argItems = argArr->getArrayItems();
		for (int i = 0; i < argc; i++) {
			argItems[i].free();
		}
		args[1].free();
		args[3].free();

		Apollo::pushGmlValueToLuaStack(&result, L);
		return 1;
	}
	static luaL_Reg metaPairs[] = {
		{ "__gc", __gc },
		{ "__index", __index },
		{ "__newindex", __newindex },
		{ "__call", __call },
		{ 0, 0 }
	};
	void initStructRef(lua_State* L) {
		luaL_newmetatable(L, "GmlStruct");
		luaL_setfuncs(L, metaPairs, 0);
		lua_pop(L, 1);

		lua_pushcfunction(L, createGmlStructUD);
		lua_setglobal(L, "__apollo_tmp");

		luaL_dostring_trace(L, "lua_struct_ref", R"lua(
(function()
	local _create_udata = __apollo_tmp
	__apollo_tmp = nil
	
	local _uid_to_udata = setmetatable({}, { __mode = "v" })
	__apollo_get_gml_struct_udata = function(uid, isCrossRef)
		local ud = _uid_to_udata[uid]
		if (ud == nil) then
			ud = _create_udata(uid, isCrossRef)
			_uid_to_udata[uid] = ud
		end
		return ud
	end
end)()
)lua")
	}
}