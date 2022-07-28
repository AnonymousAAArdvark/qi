import time

start = time.process_time()

result = 0
for i in range(10000000):
    result = result + 11
    result = result * 10
    result = result - (result / 100) * 99

print(result)
print(time.process_time() - start)
