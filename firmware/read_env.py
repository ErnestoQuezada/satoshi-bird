Import("env")
import os

env_file = ".env"
try:
    with open(env_file, "r") as f:
        for line in f:
            line = line.strip()
            # Ignore comments and empty lines
            if not line or line.startswith("#"):
                continue
            if "=" in line:
                key, val = line.split("=", 1)
                # Strip spaces and quotes
                val = val.strip(' "\'')
                # Append to CPPDEFINES so they become accessible as macros in C++
                env.Append(CPPDEFINES=[(key, env.StringifyMacro(val))])
except IOError:
    print("Warning: No .env file found. Using default fallbacks.")
