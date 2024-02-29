#include "stdafx.h"
#include "apollo_shared.h"

namespace Apollo {
	void luaToGML(RValue* result, lua_State* L, int idx) {
		auto t = lua_type(L, idx);
		const auto refMask = (1 << LUA_TTABLE)
			| (1 << LUA_TFUNCTION)
			| (1 << LUA_TUSERDATA)
			| (1 << LUA_TLIGHTUSERDATA)
			| (1 << LUA_TTHREAD);
		if ((refMask & (1 << t)) != 0) {
			Apollo::createLuaRef(result, L, idx);
			return;
		}
		switch (lua_type(L, idx)) {
			case LUA_TNUMBER:
				if (lua_isinteger(L, idx)) {
					result->kind = VALUE_INT64;
					result->v64 = lua_tointeger(L, idx);
				} else {
					result->kind = VALUE_REAL;
					result->val = lua_tonumber(L, idx);
				}
				break;
			case LUA_TBOOLEAN:
				result->kind = VALUE_BOOL;
				result->val = lua_toboolean(L, idx);
				break;
			case LUA_TSTRING:
				YYCreateString(result, lua_tostring(L, idx));
				break;
			default:
				result->kind = VALUE_UNDEFINED;
				result->ptr = nullptr;
				break;
		}
	}
	void popLuaStackValue(RValue* result, lua_State* L) {
		luaToGML(result, L, -1);
		lua_pop(L, 1);
	}
	void popLuaStackValuesAsArray(RValue* result, lua_State* L, int count) {
		//trace("pop start: %s", apollo_print_stack(L));
		if (count == LUA_MULTRET) count = lua_gettop(L);

		YYCreateEmptyArray(result, count);

		auto items = result->getArrayItems();
		for (int i = 0; i < count; i++) {
			luaToGML(&items[i], L, i + 1);
		}
		lua_pop(L, count);
	}
	void pushGMLtoLuaStack(RValue* value, lua_State* L) {
		switch (value->kind) {
			case VALUE_REAL:
				lua_pushnumber(L, value->val);
				break;
			case VALUE_INT32: case VALUE_REF:
				lua_pushinteger(L, value->v32);
				break;
			case VALUE_INT64:
				lua_pushinteger(L, value->v64);
				break;
			case VALUE_BOOL:
				lua_pushboolean(L, (int)value->val);
				break;
			case VALUE_STRING:
				lua_pushstring(L, value->getString());
				break;
			case VALUE_OBJECT: {
				auto isLuaRef = gmlClassOf.isLuaRef(value->getObjectClass());
				auto isCrossRef = isLuaRef && value->getStructMember("__state__")->ptr != Apollo::getState(L)->selfValue->ptr;
				if (isLuaRef && !isCrossRef) {
					// passing a Lua reference back to Lua?
					auto uid = YYStructGetMember(value, "__uid__")->getInt64();
					lua_getglobal(L, "__apollo_uid_to_ref");
					lua_pushinteger(L, uid);
					lua_pcall(L, 1, 1, 0);
					break;
				}
				static RValue args[3], result;
				args[0].setScriptID(gml_Script.lua_gml_ref_to_uid);
				args[1].setTo(Apollo::getState(L)->selfValue);
				args[2].setTo(value);
				GML::script_execute_def(result, args);
				args[1].free();
				args[2].free();
				lua_getglobal(L, "__apollo_get_gml_struct_udata");
				lua_pushinteger(L, result.getInt64());
				lua_pushboolean(L, isCrossRef);
				if (lua_pcall(L, 2, 1, 0)) {
					Apollo::handleLuaError(L);
					lua_pushnil(L);
				}
				break;
			}
			case VALUE_ARRAY: {
				static RValue args[3], result;
				args[0].setScriptID(gml_Script.lua_gml_ref_to_uid);
				args[1].setTo(Apollo::getState(L)->selfValue);
				args[2].setTo(value);
				GML::script_execute_def(result, args);
				args[1].free();
				args[2].free();
				lua_getglobal(L, "__apollo_get_gml_array_udata");
				lua_pushinteger(L, result.getInt64());
				if (lua_pcall(L, 1, 1, 0)) {
					Apollo::handleLuaError(L);
					lua_pushnil(L);
				}
				break;
			}
			default:
				lua_pushnil(L);
				break;
		}
	}
}