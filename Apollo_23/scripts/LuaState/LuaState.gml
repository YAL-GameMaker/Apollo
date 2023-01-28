function LuaState() constructor {
	if (global.g_apollo_trouble) exit;
	__self__ = self;
	static __cleanup__ = function() /*=>*/ {
		lua_state_destroy_raw(__ptr__);
		__ptr__ = pointer_null;
		ds_map_destroy(__ref_to_uid);
		ds_map_destroy(__uid_to_ref);
		// If a state has refs and they all get collected at once,
		// it's not guaranteed that refs' __cleanup__ will run before state's __cleanup__
		// so we need a little delay to ensure that refs don't try to clean up freed state memory
		//ds_stack_push(global.g_apollo_destroy_soon, __ptr__);
	}
	__ptr__ = lua_state_create_raw(self, __cleanup__);
	__ref_to_uid = ds_map_create(); /// @is {ds_map<any, int>}
	__uid_to_ref = ds_map_create(); /// @is {ds_map<int, any>}
	__uid_next = 100; __uid_next |= 0;
	__call_args = [1];
	
	static toString = function() /*=>*/ {return "LuaState(ptr:" + string(__ptr__) + ")"};
	
	static addCode = function(_code) /*=>*/ {return lua_add_code(__ptr__, _code)};
	static addCodeMultRet = function(_code) /*=>*/ {return lua_add_code_multret(__ptr__, _code)};
	static get = function(_name) /*=>*/ {return lua_global_get(__ptr__, _name)};
	static set = function(_name, _value) /*=>*/ {;lua_global_set(__ptr__, _name, _value);}
	/// @param {string} name
	/// @param ...args
	static call = function(_name) /*=>*/ {
		var _argc = argument_count;
		var _ptr = __ptr__;
		lua_stack_push_global(_ptr, _name);
		for (var i = 1; i < _argc; i++) lua_stack_push(_ptr, argument[i]);
		return lua_rawcall(_ptr, _argc - 1);
	}
	static callExt = function(_name, _argArray, _offset = 0, _numArgs = -1) /*=>*/ {
		return lua_global_call_ext(__ptr__, _name, _argArray, _offset, _numArgs);
	}
}