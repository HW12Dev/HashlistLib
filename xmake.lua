add_rules("mode.release", "mode.debug")

target("HashlistLib")
  set_kind("shared")
  set_languages("cxx20")
  set_exceptions("cxx")

  set_symbols("debug")

  add_includedirs("./src")
  add_includedirs("./include")

  add_headerfiles("./src/*.h", "./src/*.hpp", "./include/*.h")

  add_files("./src/*.cpp")

target_end()