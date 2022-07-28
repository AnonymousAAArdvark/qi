# This benchmark stresses both field and method lookup.

class Foo
    #Python fields are always public whereas Ruby's are private. attr_accessor makes them public
    attr_accessor :field22, :field19, :field7, :field18, :field25, :field21, :field8, :field29, :field12, :field5, :field16, :field27, :field4, :field14, :field15, :field3, :field26, :field6, :field2, :field1, :field0, :field11, :field17, :field10, :field24, :field23, :field9, :field28, :field20, :field13
    def initialize()
        @field0 = 1
        @field1 = 1
        @field2 = 1
        @field3 = 1
        @field4 = 1
        @field5 = 1
        @field6 = 1
        @field7 = 1
        @field8 = 1
        @field9 = 1
        @field10 = 1
        @field11 = 1
        @field12 = 1
        @field13 = 1
        @field14 = 1
        @field15 = 1
        @field16 = 1
        @field17 = 1
        @field18 = 1
        @field19 = 1
        @field20 = 1
        @field21 = 1
        @field22 = 1
        @field23 = 1
        @field24 = 1
        @field25 = 1
        @field26 = 1
        @field27 = 1
        @field28 = 1
        @field29 = 1
    end

    def method0()
        return @field0
    end

    def method1()
        return @field1
    end

    def method2()
        return @field2
    end

    def method3()
        return @field3
    end

    def method4()
        return @field4
    end

    def method5()
        return @field5
    end

    def method6()
        return @field6
    end

    def method7()
        return @field7
    end

    def method8()
        return @field8
    end

    def method9()
        return @field9
    end

    def method10()
        return @field10
    end

    def method11()
        return @field11
    end

    def method12()
        return @field12
    end

    def method13()
        return @field13
    end

    def method14()
        return @field14
    end

    def method15()
        return @field15
    end

    def method16()
        return @field16
    end

    def method17()
        return @field17
    end

    def method18()
        return @field18
    end

    def method19()
        return @field19
    end

    def method20()
        return @field20
    end

    def method21()
        return @field21
    end

    def method22()
        return @field22
    end

    def method23()
        return @field23
    end

    def method24()
        return @field24
    end

    def method25()
        return @field25
    end

    def method26()
        return @field26
    end

    def method27()
        return @field27
    end

    def method28()
        return @field28
    end

    def method29()
        return @field29
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
