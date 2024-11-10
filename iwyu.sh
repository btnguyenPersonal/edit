#!/bin/bash

# Install required tools if they're missing
install_tools() {
    if ! command -v iwyu_tool.py &> /dev/null || ! command -v fix_includes.py &> /dev/null; then
        echo "Installing include-what-you-use tools..."
        if [[ "$OSTYPE" == "darwin"* ]]; then
            brew install include-what-you-use
        else
            sudo apt-get update && sudo apt-get install -y include-what-you-use iwyu-tool
        fi
    fi
}

# Generate compilation database
generate_compile_commands() {
    echo '[' > compile_commands.json
    find src -name "*.cpp" | while read -r file; do
        echo '  {
            "directory": "'$(pwd)'",
            "command": "g++-13 -c -Wall -lncurses -pthread -O2 -I./src '${file}' -o '${file}'.o",
            "file": "'${file}'"
        },' >> compile_commands.json
    done
    echo '  {}' >> compile_commands.json  # Add empty object to make valid JSON
    echo ']' >> compile_commands.json
}

# Main function to run IWYU and apply fixes
fix_includes() {
    # Create backup directory
    backup_dir="include_backup_$(date +%Y%m%d_%H%M%S)"
    mkdir -p "$backup_dir"

    # Backup all source files
    find src -type f \( -name "*.cpp" -o -name "*.h" \) -exec cp --parents {} "$backup_dir/" \;

    echo "Running include-what-you-use analysis..."
    iwyu_tool.py -p . -- -Xiwyu --mapping_file=${IWYU_MAPPING_FILE:-/usr/share/include-what-you-use/gcc.libc.imp} \
        -Xiwyu --no_fwd_decls \
        2>&1 | tee iwyu.out

    echo "Applying suggested fixes..."
    fix_includes.py < iwyu.out

    echo "Backup of original files saved in: $backup_dir"
    echo "If you need to revert changes, run: cp -r $backup_dir/* ."
}

# Main script execution
echo "Starting automatic include fixing..."
install_tools
generate_compile_commands
fix_includes
echo "Include fixing complete!"

# Optional: clean up temporary files
rm -f iwyu.out compile_commands.json