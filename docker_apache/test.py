#!/usr/bin/env python3
import os

for name, value in os.environ.items():
    print("{0}: {1}".format(name, value))

if __name__ == "__main__":
    print("OK")
