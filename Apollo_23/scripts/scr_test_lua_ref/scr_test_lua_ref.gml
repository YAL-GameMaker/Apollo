function scr_test_lua_ref() {
	var q = new LuaState();
	assert(q.get("math").get("pi"), pi);
	assert(q.get("math").get("ceil").call(3.5), 4);
	assert(q.get("math").type(), "table");
	assert(q.get("math") == q.get("math"), true);
	
	q.get("math").set("tau", pi * 2);
	assert(q.addCode("return math.tau"), pi * 2);
	
	var _math = q.get("math");
	q.addCode(@'
		function get_pi(_math)
			return _math.pi;
		end
	');
	assert(q.call("get_pi", _math), pi);
	
	var _table = q.addCode("return {}");
	assert(string_pos("LuaTable", string(_table)) != 0, true);
	assert(is_lua_ref(_table), true);
	
	var _func = q.addCode("return math.ceil")
	assert(string_pos("LuaFunction", string(_func)) != 0, true);
	assert(is_lua_ref(_func), true);
}