# This benchmark stresses just method invocation.

class Foo
    def method0()
    end

    def method1()
    end

    def method2()
    end

    def method3()
    end

    def method4()
    end

    def method5()
    end

    def method6()
    end

    def method7()
    end

    def method8()
    end

    def method9()
    end

    def method10()
    end

    def method11()
    end

    def method12()
    end

    def method13()
    end

    def method14()
    end

    def method15()
    end

    def method16()
    end

    def method17()
    end

    def method18()
    end

    def method19()
    end

    def method20()
    end

    def method21()
    end

    def method22()
    end

    def method23()
    end

    def method24()
    end

    def method25()
    end

    def method26()
    end

    def method27()
    end

    def method28()
    end

    def method29()
    end
end


foo = Foo.new
start = Time.now
i = 0
while i < 500000
    foo.method0()
    foo.method1()
    foo.method2()
    foo.method3()
    foo.method4()
    foo.method5()
    foo.method6()
    foo.method7()
    foo.method8()
    foo.method9()
    foo.method10()
    foo.method11()
    foo.method12()
    foo.method13()
    foo.method14()
    foo.method15()
    foo.method16()
    foo.method17()
    foo.method18()
    foo.method19()
    foo.method20()
    foo.method21()
    foo.method22()
    foo.method23()
    foo.method24()
    foo.method25()
    foo.method26()
    foo.method27()
    foo.method28()
    foo.method29()
    i = i + 1
end

p(Time.now - start)
