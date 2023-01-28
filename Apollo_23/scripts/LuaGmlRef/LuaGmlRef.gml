#macro __apollo_debug_gml_ref 0

function lua_gml_ref_to_uid(_state/*:LuaState*/, _ref) {
	var _uid = _state.__ref_to_uid[?_ref];
	if (_uid == undefined) {
		_uid = ++_state.__uid_next;
		_state.__ref_to_uid[?_ref] = _uid;
		_state.__uid_to_ref[?_uid] = _ref;
		//trace("Created GmlRef " + string(_uid) + " for " + string(_ref));
	}
	return _uid;
}

function lua_gml_ref_free(_state/*:LuaState*/, _uid) {
	if (_state.__ptr__ == pointer_null) exit;
	var _ref = _state.__uid_to_ref[?_uid];
	if (_ref == undefined) exit;
	//trace("Freeing GmlRef " + string(_uid) + " for " + string(_ref));
	ds_map_delete(_state.__uid_to_ref, _uid);
	ds_map_delete(_state.__ref_to_uid, _ref);
}

// GmlArray-specific:
function lua_gml_ref_get_length(_state, _uid) {
	return array_length(_state.__uid_to_ref[?_uid]);
}

function lua_gml_ref_get_index(_state, _uid, _ind) {
	try {
		return _state.__uid_to_ref[?_uid][_ind - 1];
	} __lua_catch;
}

function lua_gml_ref_set_index(_state, _uid, _ind, _value) {
	try {
		_state.__uid_to_ref[?_uid][_ind - 1] = _value;
	} __lua_catch;
}

// GmlStruct-specific:
function lua_gml_ref_get_key(_state, _uid, _key) {
	try {
		var _self = _state.__uid_to_ref[?_uid];
		var _value = _self[$ _key];
		if (is_method(_value) && method_get_self(_value) == undefined) {
			// if we do gmlObj.func(), that first gets the value and then invokes it,
			// but we need gmlObj to be preserved for the call...
			_value = method(_self, _value);
		}
		return _value;
	} __lua_catch;
}

function lua_gml_ref_set_key(_state, _uid, _key, _value) {
	try {
		_state.__uid_to_ref[?_uid][$ _key] = _value;
	} __lua_catch;
}

function lua_gml_ref_invoke(_state, _uid, _argArray, _numArgs) {
	try {
		var _func = _state.__uid_to_ref[?_uid];
		if (is_method(_func)) {
			var _self = method_get_self(_func);
			if (_self != undefined) {
				with (_self) {
					return script_execute_ext(method_get_index(_func), _argArray, 0, _numArgs);
				}
				lua_show_error("Bound struct could not be found.");
				return undefined;
			}
			return script_execute_ext(method_get_index(_func), _argArray, 0, _numArgs);
		}
		return script_execute_ext(_func, _argArray, 0, _numArgs);
	} __lua_catch;
}

// CrossRef-specific:
function lua_gml_cross_ref_get_key(_state, _uid, _key) {
	try {
		var _self = _state.__uid_to_ref[?_uid];
		var _value = _self.get(_key);
		if (is_method(_value) && method_get_self(_value) == undefined) {
			// ... do I actually have to worry about it in this case?
			// it's a Lua ref, surely it won't have self-less methods inside, right?
			_value = method(_self, _value);
		}
		return _value;
	} __lua_catch;
}

function lua_gml_cross_ref_set_key(_state, _uid, _key, _value) {
	try {
		_state.__uid_to_ref[?_uid].set(_key, _value);
	} __lua_catch;
}

function lua_gml_cross_ref_invoke(_state, _uid, _argArray, _numArgs) {
	try {
		var _func = _state.__uid_to_ref[?_uid];
		return _func.callExt(_argArray, 0, _numArgs);
	} __lua_catch;
}