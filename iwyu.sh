#!/bin/bash

# Create backup of source files
backup_dir="include_backup_$(date +%Y%m%d_%H%M%S)"
mkdir -p "$backup_dir"
cp -r src "$backup_dir/"

# Function to process a single file
process_file() {
    local file="$1"
    local temp_file="${file}.tmp"

    # Run include-what-you-use and capture output
    include-what-you-use \
        -std=c++17 \
        -I./src \
        -I/usr/include \
        -I/usr/local/include \
        "${file}" 2>&1 | awk '
        # Process IWYU output and modify includes
        BEGIN { add=0; remove=0; }
        /should add these lines:/ { add=1; remove=0; next }
        /should remove these lines:/ { add=0; remove=1; next }
        /^-/ && remove==1 {
            # Store lines to remove
            sub(/^- /, "");
            sub(/ +#.*$/, "");
            removes[$0] = 1;
            next
        }
        add==1 && /^#include/ {
            # Store lines to add
            sub(/ +#.*$/, "");
            adds[NR] = $0;
        }
        END {
            # Process the original file
            while ((getline line < FILENAME) > 0) {
                if (line ~ /^#include/) {
                    sub(/ +#.*$/, "", line);
                    if (!(line in removes)) {
                        print line > "'${temp_file}'"
                    }
                } else {
                    print line > "'${temp_file}'"
                }
            }
            # Add new includes after the last include or at the start
            for (i in adds) {
                print adds[i] > "'${temp_file}'"
            }
        }' "${file}"

    # If temporary file was created, replace original
    if [ -f "${temp_file}" ]; then
        mv "${temp_file}" "${file}"
    fi
}

# Find and process all source files
echo "Processing source files..."
find src -type f \( -name "*.cpp" -o -name "*.h" \) | while read -r file; do
    echo "Processing $file..."
    process_file "$file"
done

echo "Backup of original files saved in: $backup_dir"
echo "If you need to revert changes, run: cp -r $backup_dir/* ."