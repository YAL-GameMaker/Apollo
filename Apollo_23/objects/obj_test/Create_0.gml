if (keyboard_check_direct(vk_lcontrol) || keyboard_check_direct(vk_rcontrol)) {
	show_debug_message("Waiting for a Ctrl release...");
	while (keyboard_check_direct(vk_lcontrol) || keyboard_check_direct(vk_rcontrol)) {
		apollo_sleep(1);
	}
	show_debug_message("Waiting for a Ctrl press...");
	while (!keyboard_check_direct(vk_lcontrol) && !keyboard_check_direct(vk_rcontrol)) {
		apollo_sleep(1);
	}
}
var _init = lua_init();
if (!_init) trace("Init failed:", lua_init_error);
scr_test_all();
