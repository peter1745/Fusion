-- Adds support for different Dwarf versions to clang and GCC (GCC may not spport -gdwarf-{version}?)
premake.api.addAllowed("debugformat", {
  "Dwarf-2",
  "Dwarf-3",
  "Dwarf-4",
  "Dwarf-5"
})

premake.tools.clang.shared.symbols = function(cfg, mappings)
    local values = {
        ["Default"] = "",
        ["Dwarf"] = "dwarf",
        ["Dwarf-2"] = "dwarf-2",
        ["Dwarf-3"] = "dwarf-3",
        ["Dwarf-4"] = "dwarf-4",
        ["Dwarf-5"] = "dwarf-5",
        ["SplitDwarf"] = "split-dwarf"
    }

    local debugformat = values[cfg.debugformat] or ""

    return {
        On       = "-g" .. debugformat,
        FastLink = "-g" .. debugformat,
        Full     = "-g" .. debugformat
    }
end

premake.tools.gcc.shared.symbols = premake.tools.clang.shared.symbols
