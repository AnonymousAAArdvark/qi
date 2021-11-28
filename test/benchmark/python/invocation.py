# This benchmark stresses just method invocation.

import time


class Foo:
    def method0(self):
        pass

    def method1(self):
        pass

    def method2(self):
        pass

    def method3(self):
        pass

    def method4(self):
        pass

    def method5(self):
        pass

    def method6(self):
        pass

    def method7(self):
        pass

    def method8(self):
        pass

    def method9(self):
        pass

    def method10(self):
        pass

    def method11(self):
        pass

    def method12(self):
        pass

    def method13(self):
        pass

    def method14(self):
        pass

    def method15(self):
        pass

    def method16(self):
        pass

    def method17(self):
        pass

    def method18(self):
        pass

    def method19(self):
        pass

    def method20(self):
        pass

    def method21(self):
        pass

    def method22(self):
        pass

    def method23(self):
        pass

    def method24(self):
        pass

    def method25(self):
        pass

    def method26(self):
        pass

    def method27(self):
        pass

    def method28(self):
        pass

    def method29(self):
        pass


foo = Foo()
start = time.process_time()
i = 0
while i < 500000:
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

print(time.process_time() - start)
