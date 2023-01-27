function scr_test_all() {
	var tests = [
		scr_test_add_code,
		scr_test_lua_ref,
		scr_test_array_ref,
		scr_test_struct_ref,
		scr_test_cross_ref,
	];
	for (var i = 0; i < array_length(tests); i++) {
		var test = tests[i];
		trace(sfmt("%...", script_get_name(test)))
		test();
	}
	show_debug_message("Tests OK!")
}

function assert(a, b) {
	a = string(a);
	b = string(b);
	if (a != b) show_error(sfmt("Assertion failed!\nhave: %\nwant: %",a,b), 1);
}
