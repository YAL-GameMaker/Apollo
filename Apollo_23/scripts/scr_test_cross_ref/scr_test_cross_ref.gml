function scr_test_cross_ref() {
	var a = new LuaState();
	var b = new LuaState();
	b.addCode(@'
	function get_pi_of(_math)
		return _math.pi
	end
	function call_ceil_of(_math, _val)
		return _math.ceil(_val)
	end
	function set_tau_of(_math)
		_math.tau = math.pi * 2
	end
	')
	var a_math = a.get("math");
	assert(b.call("get_pi_of", a_math), pi);
	assert(b.call("call_ceil_of", a_math, pi), 4);
	b.call("set_tau_of", a_math);
	assert(a_math.get("tau"), pi * 2);
}