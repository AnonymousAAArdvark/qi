import time

i = 0
loopStart = time.process_time()

while i < 10000000:
    i = i + 1

    1
    1
    1
    2
    1
    None
    1
    "str"
    1
    True

    None
    None
    None
    1
    None
    "str"
    None
    True

    True
    True
    True
    1
    True
    False
    True
    "str"
    True
    None

    "str"
    "str"
    "str"
    "stru"
    "str"
    1
    "str"
    None
    "str"
    True

loopTime = time.process_time() - loopStart
start = time.process_time()
i = 0

while i < 10000000:
    i = i + 1

    1 == 1
    1 == 2
    1 == None
    1 == "str"
    1 == True

    None == None
    None == 1
    None == "str"
    None == True

    True == True
    True == 1
    True == False
    True == "str"
    True == None

    "str" == "str"
    "str" == "stru"
    "str" == 1
    "str" == None
    "str" == True

elapsed = time.process_time() - start

print("loop")
print(loopTime)
print("elapsed")
print(elapsed)
print("equals")
print(elapsed - loopTime)
