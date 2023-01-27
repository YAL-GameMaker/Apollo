#include "stdafx.h"
#include "gml_api.h"
#define __RELFILE__ "gml_api.cpp"

GmlOffsets gmlOffsets{};
GmlClassOf gmlClassOf{};
gml_Func_t gml_Func;
gml_Script_t gml_Script;
int gml_Script_t::init(const char* name) {
	if (gml_Func.asset_get_index) {
		auto id = GML::asset_get_index(name);
		if (id < 0) trace("Required script %s is missing.", name);
		return id;
	} else return -1;
}

namespace GML {
	RValue* defaultSelf;
}

void YYCreateEmptyArray(RValue* result, int size) {
	static std::vector<double> dummies{};
	if (dummies.size() < size) dummies.resize(size, 0);
	YYCreateArray(result, size, dummies.data());
}

// Could be inline but figuring out how to cross-reference two types is icky
bool RValue::tryGetLuaState(lua_State** out) {
	switch (kind & MASK_KIND_RVALUE) {
		case VALUE_OBJECT: {
			if (getObjectClass() != gmlClassOf.LuaState) return false;
			// it would be faster to find a variable slot but that's a lot of work!
			auto rv = YYStructGetMember(this, "__ptr__");
			if (rv->kind != VALUE_PTR) return false;
			*out = ((ApolloState*)rv->ptr)->luaState;
			return true;
		};
		case VALUE_PTR:
			*out = ((ApolloState*)ptr)->luaState;
			return true;
		default:
			return false;
	}
}