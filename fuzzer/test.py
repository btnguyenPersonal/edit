#!/usr/bin/env python3

import subprocess

def run_test(input_string):
    try:
        result = subprocess.run(['build/e', 'longtest.md', '-c', input_string], check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        print("Success")
    except subprocess.CalledProcessError as e:
        print("Failed with error: {e}")

if __name__ == "__main__":
    run_test('4JnzJ@Y(	vpNh0USZVD}!(3>L5')
    # run_test('gg')
    