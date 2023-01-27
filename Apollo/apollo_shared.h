#pragma once
#include "stdafx.h"
#include <string>
#include "gml_api.h"

struct lua_next_error_t {
	bool hasValue = false;
	std::string text;
	void set(const char* _text) {
		text = _text;
	}
	const char* pop() {
		if (hasValue) {
			hasValue = false;
			return text.c_str();
		} else return nullptr;
	}
};
extern lua_next_error_t lua_next_error;

namespace Apollo {
	inline void clearError() {
		lua_next_error.hasValue = false;
	}
	inline void checkError(lua_State* L) {
		auto error_text = lua_next_error.pop();
		if (error_text) {
			lua_pushstring(L, error_text);
			lua_error(L);
		}
	}
	/// Submits the error to the GML callback
	void handleLuaError(lua_State* L, ApolloState* state = nullptr);
	/// For Watch. Reuses the return buffer.
	const char* printLuaStack(lua_State* L, const char* label = "Stack");
	ApolloState* getState(lua_State* L);

	void luaToGML(RValue* result, lua_State* L, int idx);
	void popLuaStackValue(RValue* result, lua_State* L);
	void popLuaStackValuesAsArray(RValue* result, lua_State* L, int count = LUA_MULTRET);
	void pushGMLtoLuaStack(RValue* value, lua_State* L);

	//
	void createLuaRef(RValue* result, lua_State* L, int ind);
}

template<typename T> T* lua_newuserdata_t(lua_State* L) {
	return (T*)lua_newuserdatauv(L, sizeof(T), 1);
};

#define luaL_dostring_trace(L, name, code) \
	if (luaL_loadbuffer(L,code,strlen(code),name) || lua_pcall(L, 0, LUA_MULTRET, 0)) {\
		printf("Error executing %s: %s\n", name, lua_tostring(L, -1));\
		fflush(stdout);\
	}

#define dllm_handle_lua_error(call) if (call) { Apollo::handleLuaError(L); return; }