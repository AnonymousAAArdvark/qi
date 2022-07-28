start = Time.now

result = 0
for i in (0..9999999)
    result = result + 11
    result = result * 10
    result = result - (result / 100) * 99
end

p(result)
p(Time.now - start)
