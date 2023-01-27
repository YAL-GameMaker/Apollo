#include "stdafx.h"
#include "apollo_shared.h"
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

		luaL_dostring_trace(L, "lua_ref", R"(
(function()
	local _create_lua_ref_struct = __apollo_tmp
	__apollo_tmp = nil
	local _uid_to_ref = {}
	local _ref_to_rvalue = {}
	local _next_uid = 100000
	__apollo_ref_to_rvalue = function(ref)
		local rv = _ref_to_rvalue[ref]
		if (rv == nil) then
			local uid = _next_uid
			_next_uid = _next_uid + 1
			rv = _create_lua_ref_struct(uid, ref)
			_ref_to_rvalue[ref] = rv;
			_uid_to_ref[uid] = ref
			--print("created ref", ref, uid, rv)
		end
		return rv
	end
	__apollo_uid_free = function(uid)
		local ref = _uid_to_ref[uid]
		if (ref ~= nil) then
			_uid_to_ref[uid] = nil
			_ref_to_rvalue[ref] = nil
		end
	end
	__apollo_uid_to_ref = function(uid)
		return _uid_to_ref[uid]
	end
	__apollo_ref_get_index = function(uid, index)
		return _uid_to_ref[uid][index]
	end
	__apollo_ref_get_length = function(uid)
		return #_uid_to_ref[uid]
	end
	__apollo_ref_set_index = function(uid, index, value)
		_uid_to_ref[uid][index] = value
	end
	__apollo_ref_invoke = function(uid, ...)
		return _uid_to_ref[uid](...)
	end
	__apollo_ref_typeof = function(uid)
		return type(_uid_to_ref[uid])
	end
end)()
		)");
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
