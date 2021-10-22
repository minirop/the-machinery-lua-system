-- premake5.lua
-- version: premake-5.0.0-alpha14

-- %TM_SDK_DIR% should be set to the directory of The Machinery SDK

-- premake5.lua
-- version: premake-5.0.0-alpha14

-- %TM_SDK_DIR% should be set to the directory of The Machinery SDK

-- helper functions:

function check_env(env)
    local env_var = os.getenv(env)

    if env_var == nil then
        return false
    end
    return true
end

function tm_lib_dir(path)
    local lib_dir = os.getenv("TM_LIB_DIR")

    if not check_env("TM_LIB_DIR") then
        error("TM_LIB_DIR not set")
        return nil
    end

    return lib_dir .. "/" .. path
end

oldlibdirs = libdirs
function libdirs(path)
    if not check_env("TM_SDK_DIR") then
        error("TM_SDK_DIR not set")
        return
    end
    oldlibdirs { 
        "$(TM_SDK_DIR)/lib/" .. _ACTION .. "/%{cfg.buildcfg}",
        "$(TM_SDK_DIR)/bin/%{cfg.buildcfg}",
        dirs
    }
    oldlibdirs { 
        "$(TM_SDK_DIR)/lib/" .. _ACTION .. "/%{cfg.buildcfg}",
        "$(TM_SDK_DIR)/bin/%{cfg.buildcfg}",
        dirs
    }
end

-- Make incluedirs() also call sysincludedirs()
oldincludedirs = includedirs
function includedirs(dirs)
    if not check_env("TM_SDK_DIR") then
        error("TM_SDK_DIR not set")
        return
    end
    oldincludedirs { 
        "$(TM_SDK_DIR)/headers",
        "$(TM_SDK_DIR)",
            dirs
    }
    sysincludedirs { 
        "$(TM_SDK_DIR)/headers",
        "$(TM_SDK_DIR)",
        dirs
    }
end
-- Makes sure the debugger points to the machinery
function set_debugger_to_engine()
    local sdk_dir = os.getenv("TM_SDK_DIR")
    if not check_env("TM_SDK_DIR") then
        error("TM_SDK_DIR not set")
        return
    end
    local debug_path_source = ""
    local debug_path_binary = ""
    if os.target() == "windows" then
         debug_path_source = "/bin/Debug/the-machinery.exe"
         debug_path_binary = "/bin/the-machinery.exe"
    else
         debug_path_source = "/bin/Debug/the-machinery"
         debug_path_binary = "/bin/the-machinery"
    end
    if os.isfile(sdk_dir..""..debug_path_source) then
        debugcommand("$(TM_SDK_DIR)"..debug_path_source)
    elseif os.isfile(sdk_dir..""..debug_path_binary) then
        debugcommand("$(TM_SDK_DIR)"..debug_path_binary)
    else
        error("Could not find '"..sdk_dir..""..debug_path_binary.."' nor '"..sdk_dir..""..debug_path_source.."'\nSuggestion: Please make sure the TM_SDK_DIR enviroment variable is pointing to the correct folder.")
    end
end


newoption {
    trigger     = "clang",
    description = "Force use of CLANG for Windows builds"
}

workspace "lua_system"
    configurations {"Debug", "Release"}
    language "C++"
    cppdialect "C++17"
    flags { "FatalWarnings", "MultiProcessorCompile" }
    warnings "Extra"
    inlining "Auto"
    sysincludedirs { "" }
    targetdir "bin/%{cfg.buildcfg}"

filter "system:windows"
    platforms { "Win64" }
    systemversion("latest")

filter {"system:linux"}
    platforms { "Linux" }

filter { "system:windows", "options:clang" }
    toolset("msc-clangcl")
    buildoptions {
        "-Wno-missing-field-initializers",   -- = {0} is OK.
        "-Wno-unused-parameter",             -- Useful for documentation purposes.
        "-Wno-unused-local-typedef",         -- We don't always use all typedefs.
        "-Wno-missing-braces",               -- = {0} is OK.
        "-Wno-microsoft-anon-tag",           -- Allow anonymous structs.
    }
    buildoptions {
        "-fms-extensions",                   -- Allow anonymous struct as C inheritance.
        "-mavx",                             -- AVX.
        "-mfma",                             -- FMA.
    }
    removeflags {"FatalLinkWarnings"}        -- clang linker doesn't understand /WX

filter "platforms:Win64"
    defines { "TM_OS_WINDOWS", "_CRT_SECURE_NO_WARNINGS" }
    includedirs { }
    staticruntime "On"
    architecture "x64"
    prebuildcommands {
        "if not defined TM_SDK_DIR (echo ERROR: Environment variable TM_SDK_DIR must be set)"
    }
    libdirs { }
    disablewarnings {
        "4057", -- Slightly different base types. Converting from type with volatile to without.
        "4100", -- Unused formal parameter. I think unusued parameters are good for documentation.
        "4152", -- Conversion from function pointer to void *. Should be ok.
        "4200", -- Zero-sized array. Valid C99.
        "4201", -- Nameless struct/union. Valid C11.
        "4204", -- Non-constant aggregate initializer. Valid C99.
        "4206", -- Translation unit is empty. Might be #ifdefed out.
        "4214", -- Bool bit-fields. Valid C99.
        "4221", -- Pointers to locals in initializers. Valid C99.
        "4702", -- Unreachable code. We sometimes want return after exit() because otherwise we get an error about no return value.
    }
    linkoptions {"/ignore:4099"}
    buildoptions {"/utf-8"}     

filter {"platforms:Linux"}
    defines { "TM_OS_LINUX", "TM_OS_POSIX" }
    includedirs { }
    architecture "x64"
    toolset "clang"
    buildoptions {
        "-fms-extensions",                   -- Allow anonymous struct as C inheritance.
        "-g",                                -- Debugging.
        "-mavx",                             -- AVX.
        "-mfma",                             -- FMA.
        "-fcommon",                          -- Allow tentative definitions
    }
    libdirs { }
    disablewarnings {
        "missing-field-initializers",   -- = {0} is OK.
        "unused-parameter",             -- Useful for documentation purposes.
        "unused-local-typedef",         -- We don't always use all typedefs.
        "missing-braces",               -- = {0} is OK.
        "microsoft-anon-tag",           -- Allow anonymous structs.
    }
    removeflags {"FatalWarnings"}

filter "configurations:Debug"
    defines { "TM_CONFIGURATION_DEBUG", "DEBUG" }
    symbols "On"
    filter "system:windows"
        set_debugger_to_engine() -- sets the debugger in VS Studio to point to the_machinery.exe

filter "configurations:Release"
    defines { "TM_CONFIGURATION_RELEASE" }
    optimize "On"

project "lua_system"
    location "build/lua_system"
    targetname "tm_lua_system"
    kind "SharedLib"
    language "C++"
    files {"*.inl", "*.h", "*.c", "*.cpp"}
    links { "lua" }
