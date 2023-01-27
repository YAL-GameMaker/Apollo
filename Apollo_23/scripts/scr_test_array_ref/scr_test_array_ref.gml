function scr_test_array_ref() {
	var q = new LuaState();
	q.addCode(@'
	function test(arr)
		print(arr)
	end
	function get_array_len(arr)
		return #arr
	end
	function get_array_item(arr, ind)
		return arr[ind]
	end
	function set_array_item(arr, ind, val)
		arr[ind] = val
	end
	')
	var arr = ["A", "B", "C"];
	//q.call("test", arr);
	assert(q.call("get_array_len", arr), 3);
	assert(q.call("get_array_item", arr, 2), "B");
	q.call("set_array_item", arr, 2, "O");
	assert(arr[1], "O");
	//q.call("get_array_item", arr, 4);
	//q.addCode(@'return 0 + ""')
}