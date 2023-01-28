constexpr char __lua_ref_init[] = R"(
(function()
	local _create_lua_ref_struct = __apollo_tmp
	__apollo_tmp = nil
	local _uid_to_ref = {}
	local _ref_to_rvalue = {}
	local _next_uid = 100000
	__apollo_ref_to_rvalue = function(ref)
		local rv = _ref_to_rvalue[ref]
		if (rv == nil) then
			local uid = _next_uid
			_next_uid = _next_uid + 1
			rv = _create_lua_ref_struct(uid, ref)
			_ref_to_rvalue[ref] = rv;
			_uid_to_ref[uid] = ref
			--print("created ref", ref, uid, rv)
		end
		return rv
	end
	__apollo_uid_free = function(uid)
		local ref = _uid_to_ref[uid]
		if (ref ~= nil) then
			_uid_to_ref[uid] = nil
			_ref_to_rvalue[ref] = nil
		end
	end
	__apollo_uid_to_ref = function(uid)
		return _uid_to_ref[uid]
	end
	__apollo_ref_get_index = function(uid, index)
		return _uid_to_ref[uid][index]
	end
	__apollo_ref_get_length = function(uid)
		return #_uid_to_ref[uid]
	end
	__apollo_ref_set_index = function(uid, index, value)
		_uid_to_ref[uid][index] = value
	end
	__apollo_ref_invoke = function(uid, ...)
		return _uid_to_ref[uid](...)
	end
	__apollo_ref_typeof = function(uid)
		return type(_uid_to_ref[uid])
	end
end)()
)";