import sys

with open(sys.argv[1], 'rb') as f:
    data = f.read()
    for i in range(len(data)):
        print(chr(data[i] - i), end='')