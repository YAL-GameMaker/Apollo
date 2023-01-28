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
		var _ptr = __state__.__ptr__;
		lua_stack_push_global(_ptr, "__apollo_ref_invoke");
		lua_stack_push(_ptr, __uid__);
		for (var i = 0; i < _argc; i++) lua_stack_push(_ptr, argument[i]);
		return lua_rawcall(_ptr, _argc + 1);
	}
	/// @param ...args
	static callMultRet = function() /*=>*/ {
		var _argc = argument_count;
		var _ptr = __state__.__ptr__;
		lua_stack_push_global(_ptr, "__apollo_ref_invoke");
		lua_stack_push(_ptr, __uid__);
		for (var i = 0; i < _argc; i++) lua_stack_push(_ptr, argument[i]);
		return lua_rawcall_multret(_ptr, _argc + 1);
	}
	
	static callExt = function(_argArray, _offset = 0, _numArgs = -1) /*=>*/ {
		var _ptr = __state__.__ptr__;
		lua_stack_push_global(_ptr, "__apollo_ref_invoke");
		lua_stack_push(_ptr, __uid__);
		var _argc = 1 + lua_stack_push_ext(_ptr, _argArray, _offset, _numArgs);
		return lua_rawcall(_ptr,  _argc);
	}
	static callExtMultRet = function(_argArray, _numArgs = -1) /*=>*/ {
		var _ptr = __state__.__ptr__;
		lua_stack_push_global(_ptr, "__apollo_ref_invoke");
		lua_stack_push(_ptr, __uid__);
		var _argc = 1 + lua_stack_push_ext(_ptr, _argArray, _offset, _numArgs);
		return lua_rawcall_multret(_ptr, _argc);
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