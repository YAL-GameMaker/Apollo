#pragma once
#include <vector>
extern "C" {
	#include "./../Lua/lua.h"
	#include "./../Lua/lualib.h"
	#include "./../Lua/lauxlib.h"
}

struct RValue;
class ApolloState {
public:
	lua_State* luaState;
	ApolloState* parent;
	RValue* selfValue = nullptr;
	RValue* callArgs = nullptr;
	std::vector<ApolloState*> children{};
	ApolloState(lua_State* _state, ApolloState* _parent) : luaState(_state), parent(_parent) {
		//
	}
	~ApolloState() {
		for (auto& child : children) {
			delete child;
		}
		lua_close(luaState);
	}
};