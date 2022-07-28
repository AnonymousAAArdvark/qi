i = 0
loopStart = Time.now

while i < 10000000
    i = i + 1

    1
    1
    1
    2
    1
    nil
    1
    "str"
    1
    true

    nil
    nil
    nil
    1
    nil
    "str"
    nil
    true

    true
    true
    true
    1
    true
    false
    true
    "str"
    true
    nil

    "str"
    "str"
    "str"
    "stru"
    "str"
    1
    "str"
    nil
    "str"
    true
end

loopTime = Time.now - loopStart
start = Time.now
i = 0

while i < 10000000
    i = i + 1

    1 == 1
    1 == 2
    1 == nil
    1 == "str"
    1 == true

    nil == nil
    nil == 1
    nil == "str"
    nil == true

    true == true
    true == 1
    true == false
    true == "str"
    true == nil

    "str" == "str"
    "str" == "stru"
    "str" == 1
    "str" == nil
    "str" == true
end

elapsed = Time.now - start

puts("loop")
p(loopTime)
puts("elapsed")
p(elapsed)
puts("equals")
p(elapsed - loopTime)
