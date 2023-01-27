#include "stdafx.h"
#include "apollo_shared.h"

lua_next_error_t lua_next_error{};
///
dllx void lua_show_error(const char* text) {
	lua_next_error.text = text;
	lua_next_error.hasValue = true;
}

namespace Apollo {
	const char* printLuaStack(lua_State* L, const char* label) {
		// what a mess
		static std::string str{};
		auto n = lua_gettop(L);
		str = std::string(label) + " [" + std::to_string(n) + "]:";
		for (int i = 1; i <= n; i++) {
			lua_pushvalue(L, i);
			auto ti = lua_type(L, -1);
			auto tn = lua_typename(L, ti);
			std::string s;
			if (ti == LUA_TBOOLEAN) {
				s = lua_toboolean(L, -1) ? "true" : "false";
			} else if (ti == LUA_TNUMBER) {
				s = std::to_string(lua_tonumber(L, -1));
			} else if (ti == LUA_TFUNCTION) {
				char tmp[9] = "";
				sprintf(tmp, "%p", lua_tocfunction(L, -1));
			} else {
				auto ps = lua_tostring(L, -1);
				s = ps ? ps : "???";
			}
			lua_pop(L, 1);

			str += "\n" + std::to_string(i)
				+ "\t" + (tn ? tn : "?")
				+ "\t" + s;
		}
		return str.c_str();
	}


	void handleLuaError(lua_State* L, ApolloState* state) {
		if (state == nullptr) state = Apollo::getState(L);
		auto error_text = lua_tostring(L, -1);
		luaL_traceback(L, L, error_text, 0);
		error_text = lua_tostring(L, -1);
		//trace("lua error: %s", error_text);

		static RValue args[3], result;
		args[0].setScriptID(gml_Script.lua_proc_error_raw);
		args[1].setTo(state->selfValue);
		YYCreateString(&args[2], error_text);
		lua_pop(L, 2);
		GML::script_execute_def(result, args);
	}
}