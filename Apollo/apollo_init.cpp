#include "stdafx.h"
#include "apollo_shared.h"
#include <string>
#define __RELFILE__ "apollo_init.cpp"
#ifdef _WINDOWS
//#define _APOLLO_MEMCHECK
#endif

bool trouble = true;

static bool canAccessMemory(const void* base, size_t size) {
	#ifdef _APOLLO_MEMCHECK
	const auto pmask = PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY
		| PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY;
	::MEMORY_BASIC_INFORMATION mbi{};
	size_t steps = size > 0 ? 2 : 1;
	for (auto step = 0u; step < steps; step++) {
		const void* addr = ((uint8_t*)base) + step * (size - 1);
		if (!VirtualQuery(addr, &mbi, sizeof mbi)) return false;
		if (mbi.State != MEM_COMMIT) return false;
		if ((mbi.Protect & PAGE_GUARD) != 0) return false;
		if ((mbi.Protect & pmask) == 0) return false;
	}
	#endif
	return true;
}

static int findCScriptRefOffset(void* _fptr_1, void* _fptr_2, void* _mptr_1, void* _mptr_2) {
	auto f1 = (void**)_fptr_1;
	auto f2 = (void**)_fptr_2;
	auto f3 = (void**)_mptr_1;
	auto f4 = (void**)_mptr_2;
	void** fx[] = { f1, f2, f3, f4 };
	for (auto i = 10u; i < 24; i++) {
		auto step = 0u;
		for (; step < 2; step++) {
			auto fi = fx[step];

			// should be NULL, <addr>, NULL:
			if (fi[i - 1] != nullptr) break;
			if (fi[i] == nullptr) break;
			if (fi[i + 1] != nullptr) break;
			// and the method pointers shouldn't have a function in them:
			auto mi = fx[step + 2];
			if (mi[i] != nullptr) break;
}
		if (step < 2u) continue;

		// destination address must match:
		auto dest = f1[i];
		if (dest != f2[i]) continue;

		return (int)(sizeof(void*) * i);
	}
	return -1;
}

dllx double apollo_init_1(void* _fptr_1, void* _fptr_2, void* _mptr_1, void* _mptr_2) {
	auto ofs = findCScriptRefOffset(_fptr_1, _fptr_2, _mptr_1, _mptr_2);
	if (ofs < 0) return -1;
	gmlOffsets.CScriptRef.cppFunc = ofs;

	// both CWeakRef and CScriptRef inherit from YYObjectBase;
	// in CScriptRef, the three pointers are the first non-inherited members.
	// in CWeakRef, the destination pointer is the first non-inherited member.
	gmlOffsets.CWeakRef.weakRef = ofs - sizeof(void*);

	// we'll check if it's a 2023.8+ array layout in the function below:
	gmlOffsets.RefDynamicArrayOfRValue.items = gmlOffsets.CWeakRef.weakRef.offset + sizeof(int) * 2;
	gmlOffsets.RefDynamicArrayOfRValue.length = gmlOffsets.RefDynamicArrayOfRValue.items.offset + sizeof(void*) + sizeof(int64_t) + sizeof(int);
	return 1;
}

dllgm void apollo_init_array(RValue* a2, RValue* a3, RValue* a4) {
	// at first arrays were a little struct
	// with introduction of GC, arrays were made into a collectable object, inherting from YYObjectBase
	// now arrays are a little struct again, and they've got a pointer to YYObjectBase (probably used *only* for GC)
	// the new layout is like this:
	/*
	struct RefDynamicArrayOfRValue {
		YYObjectBase* gcThing;
		RValue* items;
		int64 copyOnWriteStuff;
		int refCount;
		int mystery1;
		int mystery2;
		int length;
	}
	*/
	FieldOffset<int> magicLength = sizeof(void*) * 2 + sizeof(int64) + sizeof(int) * 3;
	if (magicLength.read(a2->ptr) == 2 && magicLength.read(a3->ptr) == 3 && magicLength.read(a4->ptr) == 4) {
		// so if what we've passed seems to be using that layout, 
		gmlOffsets.RefDynamicArrayOfRValue.items.offset = sizeof(void*);
		gmlOffsets.RefDynamicArrayOfRValue.length = magicLength;
	}
}

constexpr char gml_Script_[] = "gml_Script_";
dllx double apollo_init_2(uint8_t* _c1, uint8_t* _c2) {
	auto c1 = (void**)_c1;
	auto c2 = (void**)_c2;
	void** cx[] = { c1, c2 };
	#ifdef _APOLLO_MEMCHECK
	::MEMORY_BASIC_INFORMATION mbi{};
	const auto pmask = PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY;
	#endif
	for (auto i = 1; i < 10; i++) {
		auto step = 0u;
		for (; step < 2; step++) {
			auto ci = cx[step];
			if (!canAccessMemory(ci + i, sizeof(void*))) return -1;
			if (c1[i] == nullptr) break;
			if (!canAccessMemory(ci[i], sizeof(gml_Script_))) return -1;
		}
		if (step < 2) continue;
		
		auto dest = c1[i];
		if (c2[i] != dest) continue;

		#ifdef _APOLLO_MEMCHECK
		if (!VirtualQuery(dest, &mbi, sizeof mbi)) continue;
		if ((mbi.Protect & pmask) == 0) continue;
		#endif

		if (memcmp(dest, gml_Script_, std::size(gml_Script_) - 1) != 0) continue;
		gmlOffsets.YYObjectBase.className = i * sizeof(void*);
		gmlClassOf.LuaState = (const char*)dest;
		trouble = false;
		return 1;
	}
	return -1;
}

dllgm void apollo_init_3(RValue* script_execute, RValue* defaultSelf, RValue* asset_get_index,
	RValue* luaRef, RValue* luaTable, RValue* luaFunction, RValue* luaUserdata
) {
	gml_Func.script_execute = script_execute->getCppFunc();
	gmlClassOf.LuaRef = luaRef->getObjectClass();
	gmlClassOf.LuaTable = luaTable->getObjectClass();
	gmlClassOf.LuaFunction = luaFunction->getObjectClass();
	gmlClassOf.LuaUserdata = luaUserdata->getObjectClass();
	GML::defaultSelf = YYStructGetMember(defaultSelf, "__self__");
	gml_Func.asset_get_index = asset_get_index->getCppFunc();
	gml_Script = {};
}

#if 0 // strictly for staring in the debugger.
class CInstanceBase {
public:
	RValue* yyvars;
	virtual ~CInstanceBase() {};
	virtual RValue& getYYVarRef(int index) = 0;
	virtual RValue& getYYVarRefL(int index) = 0;
};
class YYObjectBase : public CInstanceBase {
public:
	YYObjectBase* m_pNextObject;
	YYObjectBase* m_pPrevObject;
	YYObjectBase* m_prototype;
	const char* m_class;
};
#endif

static YYRunnerInterface g_YYRunnerInterface{};
YYRunnerInterface* g_pYYRunnerInterface;
__declspec(dllexport) void YYExtensionInitialise(const struct YYRunnerInterface* _struct, size_t _size) {
	if (_size < sizeof(YYRunnerInterface)) {
		memcpy(&g_YYRunnerInterface, _struct, _size);
	} else {
		memcpy(&g_YYRunnerInterface, _struct, sizeof(YYRunnerInterface));
	}
	g_pYYRunnerInterface = &g_YYRunnerInterface;
}

dllx double apollo_sleep(double ms) {
	if (g_pYYRunnerInterface) {
		Timing_Sleep((int64_t)ms);
	} else Sleep((int)ms);
	return 0;
}