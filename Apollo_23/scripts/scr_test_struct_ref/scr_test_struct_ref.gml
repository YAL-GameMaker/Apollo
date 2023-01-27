function scr_test_struct_ref() {
	var q = new LuaState();
	q.addCode(@'
	function get_item(q, key)
		return q[key]
	end
	function set_item(q, key, val)
		q[key] = val
	end
	function call_gml(q, ...)
		return q(...)
	end
	')
	var struct = { num: 3 };
	assert(q.call("get_item", struct, "num"), 3);
	q.call("set_item", struct, "num", 5);
	assert(struct.num, 5);
	
	var _fn = function(_n) /*=>*/ {return "wow" + string(_n)};
	assert(q.call("call_gml", _fn, 5), "wow5");
}