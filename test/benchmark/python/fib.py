import time


def fib(n):
    if n < 2:
        return n
    return fib(n - 2) + fib(n - 1)


start = time.process_time()
print(fib(35) == 9227465)
print(time.process_time() - start)
