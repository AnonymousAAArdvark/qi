# This benchmark stresses instance creation and initializer calling.
import time


class Foo:
    def __init__(self):
        pass


start = time.process_time()
i = 0
while i < 500000:
    Foo()
    Foo()
    Foo()
    Foo()
    Foo()
    Foo()
    Foo()
    Foo()
    Foo()
    Foo()
    Foo()
    Foo()
    Foo()
    Foo()
    Foo()
    Foo()
    Foo()
    Foo()
    Foo()
    Foo()
    Foo()
    Foo()
    Foo()
    Foo()
    Foo()
    Foo()
    Foo()
    Foo()
    Foo()
    Foo()
    i = i + 1

print(time.process_time() - start)
