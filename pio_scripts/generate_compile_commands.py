import os

Import("env")

# Ensure toolchain includes are added
env.Replace(COMPILATIONDB_INCLUDE_TOOLCHAIN=True)

# Use the build dir for the output
compile_commands_path = os.path.join(env.subst("$BUILD_DIR"), "compile_commands.json")

env.Replace(COMPILATIONDB_PATH=compile_commands_path)

# Fix entry paths so Sonar gets absolute file paths
env.AddPostAction(
    "$BUILD_DIR/compile_commands.json",
    lambda source, target, env: print(f"Generated {compile_commands_path}"),
)
