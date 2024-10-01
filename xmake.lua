add_rules("mode.debug", "mode.release")

add_repositories("liteldev-repo https://github.com/LiteLDev/xmake-repo.git")

-- Dependencies from xmake-repo.
add_requires("fmt")
add_requires("magic_enum")
add_requires("nlohmann_json")

-- Dependencies from liteldev-repo.
add_requires("levilamina")

if not has_config("vs_runtime") then
    set_runtimes("MD")
end

option("tests")
    set_default(false)
    set_showmenu(true)
    set_description("Enable tests")

target("iListenAttentively")
    add_cxflags(
        "/EHa",
        "/utf-8",
        "/W4",
        "/w44265",
        "/w44289",
        "/w44296",
        "/w45263",
        "/w44738",
        "/w45204"
    )
    add_defines("NOMINMAX", "UNICODE","ILA_EXPORT")
    add_headerfiles("src/**.h")
    add_files("src/**.cpp")
    add_files("src/**.rc")
    add_includedirs("src")
    add_packages(
		"levilamina",
 		"fmt",
        "magic_enum",
        "nlohmann_json")
    add_shflags("/DELAYLOAD:bedrock_server.dll")
    set_exceptions("none")
    set_kind("shared")
    set_languages("c++23")
    set_symbols("debug")

    if is_mode("debug") then
        add_defines("ILA_DEBUG")
    end

    if has_config("tests") then
        add_defines("ILA_TESTS")
        add_includedirs("src/test/")
        add_headerfiles("src/test/**.h")
        add_files("src/test/**.cpp")
    end

    after_build(function (target)
        local mod_packer = import("scripts.after_build")

        local tag = os.iorun("git describe --tags --abbrev=0 --always")
        local major, minor, patch, suffix = tag:match("v(%d+)%.(%d+)%.(%d+)(.*)")
        if not major then
            print("Failed to parse version tag, using 0.0.0")
            major, minor, patch = 0, 0, 0
        end
        local mod_define = {
            modName = target:name(),
            modFile = path.filename(target:targetfile()),
            modVersion = major .. "." .. minor .. "." .. patch,
        }
        
        mod_packer.pack_mod(target,mod_define)
    end)
