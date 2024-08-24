#include "script.h"

#include <lua.h>
#include <lualib.h>
#include <Luau/CodeGen.h>
#include <Luau/Compiler.h>
#include <Luau/Parser.h>
#include <Luau/TimeTrace.h>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

static lua_State* LState = nullptr;

struct Coverage{
    lua_State* L = nullptr;
    std::vector<int> functions;
} gCoverage;

struct GlobalOptions
{
    int optimizationLevel = 1;
    int debugLevel = 1;
} globalOptions;

static Luau::CompileOptions copts()
{
    Luau::CompileOptions result = {};
    result.optimizationLevel = globalOptions.optimizationLevel;
    result.debugLevel = globalOptions.debugLevel;
    result.typeInfoLevel = 1;
    result.coverageLevel = (gCoverage.L != nullptr) ? 2 : 0;

    return result;
}


std::string resolve_module_path(const std::string& module_name) {
    return module_name + ".lua";
}

static int lua_require(lua_State* L) {
    
    const char* module_name = luaL_checkstring(L, 1);

    lua_getfield(L, LUA_REGISTRYINDEX, "_LOADED");
    lua_getfield(L, -1, module_name);
    if (!lua_isnil(L, -1))
    {
        return 1; // Module already loaded, return it
    }
    lua_pop(L, 1); // Remove nil from the stack

    std::string module_path = resolve_module_path(module_name); 

    std::ifstream module_file(module_path);
    if (!module_file.is_open())
    {
        lua_pushfstring(L, "module '%s' not found", module_name);
        lua_error(L);
    }

    std::stringstream buffer;
    buffer << module_file.rdbuf();
    std::string source_code = buffer.str();
    std::string bytecode = Luau::compile(source_code, copts());

    if (luau_load(L, module_name, bytecode.data(), bytecode.size(), 0) != 0)  {
        lua_error(L); // Compilation error
    }

    if (lua_pcall(L, 0, 1, 0) != 0){
        lua_error(L); // Runtime error
    }


    lua_getfield(L, LUA_REGISTRYINDEX, "_LOADED");
    lua_pushvalue(L, -2);
    lua_setfield(L, -2, module_name);
    lua_pop(L, 1); // Remove _LOADED from stack

    return 1; // Return the module's result
}


script_t::script_t() {
    LState = luaL_newstate();
    luaL_openlibs(LState);
	lua_pushcfunction(LState, lua_require, "lua_req");
    lua_setglobal(LState, "require");

}

script_t::~script_t() {
    lua_close(LState);
}