function scr_test_add_code() {
	var q = new LuaState();
	assert(q.addCode("return 4"), 4);
	assert(q.addCodeMultRet("return 4, 5"), [4, 5]);
	
	q.addCode(@'
		function forward(i)
			return i
		end
	')
	assert(q.call("forward", sprite_index), int64(sprite_index));
}