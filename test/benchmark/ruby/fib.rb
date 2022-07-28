


def fib(n)
    if n < 2
        return n
    end
    return fib(n - 2) + fib(n - 1)
end


start = Time.now
p(fib(35) == 9227465)
p(Time.now - start)
