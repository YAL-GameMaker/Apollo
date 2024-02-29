#define lua_init
/// ()->bool
//#global lua_init_error
//#global lua_error_handler
lua_error_handler = function(_state, _error_text) {
	show_debug_message("Lua error in " + string(_state) + ": " + _error_text);
}
global.g_apollo_trouble = true;
global.g_apollo_destroy_soon = ds_stack_create();
global.g_apollo_destroy_soon2 = ds_stack_create();
var _ctx = { _things: [] };
var _fptr = method(_ctx, function(_func) /*=>*/ {
   var _method = method(undefined, _func);
   array_push(_things, _method);
   return ptr(_method);
});
var _qptr = method(_ctx, function(_struct) /*=>*/ {
	array_push(_things, _struct);
	return ptr(_struct);
});
var _f1 = function() { return 1; };
var _result = apollo_init_1(_fptr(is_bool), _fptr(is_bool), ptr(_f1), ptr(_f1));
if (_result == 0) {
	lua_init_error = "Apollo native extension is not loaded";
	return false;
}
if (_result < 0) {
	lua_init_error = "Failed to resolve method offset (incompatible runtime?)";
	return false;
}

apollo_init_array([1, 2], [3, 4, 5], [6, 7, 8, 9]);

_result = apollo_init_2(_qptr(new LuaState()), _qptr(new LuaState()));
if (_result <= 0) {
	lua_init_error = "Failed to resolve struct class offset (incompatible runtime?)";
	return false;
}
var _ctr = /*#cast*/ function() constructor {
	__self__ = self;
};
var _self = new LuaEmptyContext();
global.g_lua_default_self = _self;
apollo_init_3(
	_fptr(script_execute),
	_self,
	_fptr(asset_get_index),
	new LuaRef(undefined, 0),
	new LuaTable(undefined, 0),
	new LuaFunction(undefined, 0),
	new LuaUserdata(undefined, 0),
);

lua_init_error = "";
global.g_apollo_trouble = false;
return true;

#define lua_update
exit;
/*var _soon = global.g_apollo_destroy_soon;
repeat (ds_stack_size(_soon)) {
	var _ptr = ds_stack_pop(_soon);
	lua_state_destroy_raw(_ptr);
}*/

#define lua_proc_error_raw
lua_error_handler(argument0, argument1);

#define apollo_prepare_buffer
/// (size:int)->buffer~
var _size = argument0;
gml_pragma("global", "global.__apollo_buffer = undefined");
var _buf = global.__apollo_buffer;
if (_buf == undefined) {
	_buf = buffer_create(_size, buffer_grow, 1);
	global.__apollo_buffer = _buf;
} else if (buffer_get_size(_buf) < _size) {
	buffer_resize(_buf, _size);
}
buffer_seek(_buf, buffer_seek_start, 0);
return _buf;