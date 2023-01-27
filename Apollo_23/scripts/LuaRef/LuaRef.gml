function LuaRef(_state, _uid) constructor {
	if (_state == undefined) return;
	__self__ = self;
	static __cleanup__ = function() /*=>*/ {
		if (__state__.__ptr__ == pointer_null) exit;
		lua_global_call(__state__, "__apollo_uid_free", __uid__);
	}
	lua_ref_create_post(self, __cleanup__);
	__state__ = _state /*#as LuaState*/;
	__uid__ = _uid /*#as int*/;
	static __type__ = "LuaRef";
	static toString = function() /*=>*/ {
		return __type__ + "(state:" + string(__state__.__ptr__)
			+ ", uid:" + string(__uid__)
		+ ")";
	}
	
	static length = function() /*=>*/ {
		return lua_global_call(__state__, "__apollo_ref_get_length", __uid__);
	}
	static get = function(_key) /*=>*/ {
		return lua_global_call(__state__, "__apollo_ref_get_index", __uid__, _key);
	}
	static set = function(_key, _value) /*=>*/ {
		lua_global_call(__state__, "__apollo_ref_set_index", __uid__, _key, _value);
	}
	/// @param ...args
	static call = function() /*=>*/ {
		var _argc = argument_count;
		var _args = array_create(_argc);
		_args[0] = __uid__;
		for (var i = 0; i < _argc; i++) _args[i + 1] = argument[i];
		return lua_global_call_ext(__state__, "__apollo_ref_invoke", _args);
	}
	/// @param ...args
	static callMultRet = function() /*=>*/ {
		var _argc = argument_count;
		var _args = array_create(_argc);
		_args[0] = __uid__;
		for (var i = 0; i < _argc; i++) _args[i + 1] = argument[i];
		return lua_global_call_ext_multret(__state__, "__apollo_ref_invoke", _args);
	}
	
	static callExt = function(_arg_array, _arg_count = -1) /*=>*/ {
		var _arg_length = array_length(_arg_array);
		if (_arg_count < 0 || _arg_count > _arg_length) _arg_count = _arg_length;
		var _args = array_create(_arg_count + 1);
		_args[0] = __uid__;
		array_copy(_args, 1, _arg_array, 0, _arg_count);
		return lua_global_call_ext(__state__, "__apollo_ref_invoke", _args);
	}
	static callExtMultRet = function(_arg_array, _arg_count = -1) /*=>*/ {
		var _arg_length = array_length(_arg_array);
		if (_arg_count < 0 || _arg_count > _arg_length) _arg_count = _arg_length;
		var _args = array_create(_arg_count + 1);
		_args[0] = __uid__;
		array_copy(_args, 1, _arg_array, 0, _arg_count);
		return lua_global_call_ext_multret(__state__, "__apollo_ref_invoke", _args);
	}
	
	static type = function() /*=>*/ {
		return lua_global_call(__state__, "__apollo_ref_typeof", __uid__);
	}
}
function lua_ref_create_raw(_state, _uid) {
	return new LuaRef(_state, _uid);
}

function LuaTable(_state, _uid) : LuaRef(_state, _uid) constructor {
	static __type__ = "LuaTable";
}
function lua_ref_create_raw_table(_state, _uid) {
	return new LuaTable(_state, _uid);
}

function LuaFunction(_state, _uid) : LuaRef(_state, _uid) constructor {
	static __type__ = "LuaFunction";
}
function lua_ref_create_raw_function(_state, _uid) {
	return new LuaFunction(_state, _uid);
}

function LuaUserdata(_state, _uid) : LuaRef(_state, _uid) constructor {
	static __type__ = "LuaUserdata";
}
function lua_ref_create_raw_userdata(_state, _uid) {
	return new LuaUserdata(_state, _uid);
}