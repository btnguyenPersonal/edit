import itertools
import subprocess
import threading

charset = ''.join(chr(i) for i in range(1, 4)) + \
          ''.join(chr(i) for i in range(5, 21) if i != 15) + \
          ''.join(chr(i) for i in range(21, 127))

def run_test(input_string):
    try:
        result = subprocess.run(['build/e', 'longtest.md', '-c', input_string], check=True, text=True, capture_output=True, shell=False)
    except subprocess.CalledProcessError as e:
        print(f"Tested: {repr(input_string)}, Failed with error: {e}")

def fuzzer(max_length):
    for length in range(1, max_length + 1):
        for test_string in itertools.product(charset, repeat=length):
            input_string = ''.join(test_string)
            threading.Thread(target=run_test, args=(input_string,)).start()

if __name__ == "__main__":
    max_length = 8
    fuzzer(max_length)