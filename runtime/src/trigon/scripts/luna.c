#ifdef _LUNA
#include "luna.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "luajit.h"

static lua_State* lua_state  = NULL;

void luna_init() {
	lua_state = luaL_newstate();
	luaL_openlibs(lua_state);
}

void luna_start() {
	lua_state = luaL_newstate();
	luaL_openlibs(lua_state);

	if (luaL_loadfile(lua_state, "scripts/main.lua") || lua_pcall(lua_state, 0, 0, 0)) {
		fprintf(stderr, "Cannot run script: %s\n", lua_tostring(lua_state, -1));
		return;
	}

	lua_getglobal(lua_state, "main");
	if (lua_pcall(lua_state, 0, 0, 0) != LUA_OK) {
		fprintf(stderr, "Failed to call function: %s\n", lua_tostring(lua_state, -1));
		return;
	}
}

void luna_update() {

}

void luna_stop() {
	lua_close(lua_state);
}

#else

void luna_init() {
}

void luna_start() {

}

void luna_update() {
	
}

void luna_stop() {

}

#endif