import os

Import("env")

# Ensure toolchain includes are added to compile_commands.json
env.Replace(COMPILATIONDB_INCLUDE_TOOLCHAIN=True)

# Set the output path for the compilation database
env.Replace(
    COMPILATIONDB_PATH=os.path.join(env.subst("$BUILD_DIR"), "compile_commands.json")
)

# Optionally, ensure entries use the correct absolute path
env.Replace(COMPILATIONDB_ENTRY_PATH=os.getcwd())
