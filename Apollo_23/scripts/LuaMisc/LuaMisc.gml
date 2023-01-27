#macro __lua_catch catch (_e) { lua_show_error("[" + _e.script + "]:" + string(_e.line) + ": " + _e.message); exit; }
function LuaEmptyContext() constructor {
	__self__ = self;
}

#macro lua_type_none 0
#macro lua_type_nil 1
#macro lua_type_bool 2
#macro lua_type_number 3
#macro lua_type_string 4
#macro lua_type_table 5
#macro lua_type_function 6
#macro lua_type_thread 7
#macro lua_type_userdata 8
#macro lua_type_lightuserdata 9
#macro lua_type_unknown 10
