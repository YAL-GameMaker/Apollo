# Apollo (v3)

**Quick links:** [itch.io](https://yellowafterlife.itch.io/gamemaker-lua)
· [GM Marketplace](https://marketplace.yoyogames.com/assets/5192/_)

This extension lets you integrate Lua scripting in your GameMaker 2022+ projects!

That includes loading and executing code, exchanging values with it, passing functions back and forth, and more.

It is most commonly used to implement modding support in games,
but, just like with Lua itself, you may find a variety of uses.

v3 is a complete rewrite of the extension that overhauls the behind-the-scenes logic and expands user-facing functionality (see below).

v3 is work-in-progress but you can compile it from source and mess around with what there is.

## What's new

- As a preamble, this version of the extension targets the current versions of GameMaker (and _possibly_ GMS 2.3.6 - untested) instead of "any version released after mid-2015", which gives me a lot more freedom in what I can do and how.
- Everything is structs now, meaning that you do `state = new LuaState()` instead of `state = lua_state_create()` and `state.call("fun", 1)` instead of `lua_call(state, "fun", 1)`.  
  A "compatibility script" for old-style function names will come later.
- Extension's data structures are now integrated with GM's garbage-collector, meaning that you don't have to call `destroy` anymore.
- Reference types (GML arrays/structs/methods, Lua tables/functions/userdatas) are now passed by-reference in both directions.
- Consequence of above, you can now work with nested data in Lua states easily, e.g. `show_debug_message(state.get("math").get("pi"))` will give you 3.14.

## What's interesting here

This extension is "a bit of everything" case:

- It uses the new extension function format (which takes raw GML values).
- And I'm auto-generating these functions because I don't want my code to look
  [like this](https://github.com/YoYoGames/GMEXT-Steamworks/blob/ad3d8ca083865cc3bd86ded8cb9e2a8f9dbfc250/source/Steamworks_vs/Steamworks/GMLSteam/steam_inventory.cpp#L454-L463).
- It uses the (as of writing this still poorly documented) `YYRunnerInterface`.
- However, that's still not enough, so I auto-resolve a number of field offsets in [apollo_init.cpp](Apollo/apollo_init.cpp).
- Both GML and Lua are garbage-collected languages, which makes sharing references between them quite a headache.  
  You can see an example in [LuaRef.svg](export/LuaRef.svg) that I started drawing before realizing that a hellish block chart doesn't make the thing any less confusing.
- Error handling across language boundaries gets a little funky because GML errors are custom C++ exceptions.
- Other than that, actual Lua interop is pretty mild - convert values and give them to Lua. Convert the results back to GML.
  Don't get any numbers wrong or everything will catch fire
  (or worse, _won't_ catch fire and will mysteriously misbehave instead).

## Building

See [BUILD.md](BUILD.md)

## History

The primary stumbling point for extensions like Apollo has always been in invocation of GML scripts from native code.

Apollo v1 ran Lua code in a separate thread, using locks to make GML code to wait for Lua code to finish and vice versa.  
This approach left much to be desired, especially in terms of performance.

Apollo v2 used coroutines to implement waiting in a saner way.  
So, whenever Lua code did something that required calling GML, the state would yield (`lua_yieldk`), store context information, and return execution to GML, where the extension would read what it's being asked to do, do that, and call `lua_resume` to resume execution.  
This approach _mostly_ worked, but there are limitations to where you can yield in Lua.

Apollo v3 uses a combination of new extension interfaces and novel tricks to move most of the logic to native code.  
This is, by far, the closest we have been to using Lua the way it's supposed to be used.

## Why is Lua statically linked?

As much as I enjoy the idea of being able to swap out Lua for a compatible variant by replacing the dynamic library file, dynamically linking nearby libraries in GameMaker extensions is a rather hellish experience. Especially on Mac. Especially when the library path can differ depending on build type and you have no way to address this other than telling the user to fiddle with `install_name_tool`.

## Why is Apollo paid?

As if that's not apparent, it's a lot of work.

Less work than designing a scripting language from scratch
(which I also did more than a few times), but still -
given the current state of GameMaker extension API, this is not something you're expected to be doing _at all_.

And thus it's not something that people are doing -
at the time of writing, vast majority of native extensions for scripting language integrations remain to be "isolated".
You fill in the inputs, the code runs, you collect the outputs.
No calls to game code amid of it.
Hard to make a modding API on top of that.

But it's not just about coming up with ways around engine limitations -
you have to document and support it too. For years, no less.

This source code release (along with its new custom license)
makes the extension more accessible.

## Meta

**Author:** [YellowAfterlife](https://github.com/YellowAfterlife)  
**License:** Custom license (see `LICENSE`)
