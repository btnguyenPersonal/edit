import itertools
import subprocess
import threading
import random

charset = ''.join(chr(i) for i in range(1, 4)) + \
          ''.join(chr(i) for i in range(5, 21) if i != 15) + \
          ''.join(chr(i) for i in range(21, 127))

def readable_string(s):
    key_map = {
        '\x00': 'C-@',
        '\x01': 'C-A',
        '\x02': 'C-B',
        '\x03': 'C-C',
        '\x04': 'C-D',
        '\x05': 'C-E',
        '\x06': 'C-F',
        '\x07': 'C-G',
        '\x08': 'C-H',
        '\x09': 'C-I',
        '\x0A': 'C-J',
        '\x0B': 'C-K',
        '\x0C': 'C-L',
        '\x0D': 'C-M',
        '\x0E': 'C-N',
        '\x0F': 'C-O',
        '\x10': 'C-P',
        '\x11': 'C-Q',
        '\x12': 'C-R',
        '\x13': 'C-S',
        '\x14': 'C-T',
        '\x15': 'C-U',
        '\x16': 'C-V',
        '\x17': 'C-W',
        '\x18': 'C-X',
        '\x19': 'C-Y',
        '\x1A': 'C-Z',
        '\x1B': 'Esc',
        '\x1C': 'C-\\',
        '\x1D': 'C-]',
        '\x1E': 'C-^',
        '\x1F': 'C-_',
        '\x7F': 'Del'
    }
    return '   '.join(key_map.get(c, c).ljust(3) for c in s)

def run_test(input_string):
    try:
        safe_input = input_string.encode('latin1')
        result = subprocess.run(['build/e', 'longtest.md', '-c', safe_input], check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        print(f"Tested: {readable_string(input_string)}, Success")
    except subprocess.CalledProcessError as e:
        print(f"Tested: {readable_string(input_string)}, Failed with error: {e}")

def fuzzer(charset, string_length):
    for i in range(100000):
        input_string = ''.join(random.choice(charset) for _ in range(string_length))
        threading.Thread(target=run_test, args=(input_string,)).start()

if __name__ == "__main__":
    fuzzer(charset, 30)
