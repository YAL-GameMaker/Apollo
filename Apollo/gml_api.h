#pragma once
#include "stdafx.h"

struct gml_Func_t {
	YYFunc script_execute = nullptr;
	YYFunc asset_get_index = nullptr;
};
extern gml_Func_t gml_Func;

void YYCreateEmptyArray(RValue* result, int size);

namespace GML {
	extern RValue* defaultSelf;
	inline void script_execute_def(RValue& result, RValue* arg, int argc) {
		gml_Func.script_execute(result, (CInstance*)defaultSelf->ptr, (CInstance*)defaultSelf->ptr, argc, arg);
	}
	template<size_t size> void script_execute_def(RValue& result, RValue(&args)[size]) {
		script_execute_def(result, args, (int)size);
	}

	/// runs script, checks for lua_show_error
	template<size_t size> void script_execute_def_for(RValue& result, RValue(&args)[size], lua_State* L) {
		Apollo::clearError();
		script_execute_def(result, args, (int)size);
		Apollo::checkError(L);
	}

	inline int asset_get_index(const char* name) {
		RValue arg{}, result{};
		YYCreateString(&arg, name);
		gml_Func.asset_get_index(result, nullptr, nullptr, 1, &arg);
		arg.free();
		return result.getInt32(-1);
	}
}

struct gml_Script_t {
	int init(const char* name);
	#define X(name) int name = init(#name)
	X(lua_ref_create_raw);
	X(lua_ref_create_raw_table);
	X(lua_ref_create_raw_function);
	X(lua_ref_create_raw_userdata);
	X(lua_proc_error_raw);
	X(lua_gml_ref_to_uid);
	X(lua_gml_ref_free);
	// GmlArray:
	X(lua_gml_ref_get_length);
	X(lua_gml_ref_get_index);
	X(lua_gml_ref_set_index);
	// GmlStruct:
	X(lua_gml_ref_get_key);
	X(lua_gml_ref_set_key);
	X(lua_gml_ref_invoke);
	// GmlCrossRef:
	X(lua_gml_cross_ref_get_key);
	X(lua_gml_cross_ref_set_key);
	X(lua_gml_cross_ref_invoke);
	#undef X
};
extern gml_Script_t gml_Script;
