function scr_test_closure() {
	var q = new LuaState();
	lua_stack_push_closure(q, method_get_index(function(_state) /*=>*/ {
		lua_stack_push(_state, "hi!");
		return 1;
	}), 0);
	lua_stack_pop_to_global(q, "greet");
	assert(q.call("greet"), "hi!");
}
