# This benchmark stresses both field and method lookup.

import time


class Foo:
    def __init__(self):
        self.field0 = 1
        self.field1 = 1
        self.field2 = 1
        self.field3 = 1
        self.field4 = 1
        self.field5 = 1
        self.field6 = 1
        self.field7 = 1
        self.field8 = 1
        self.field9 = 1
        self.field10 = 1
        self.field11 = 1
        self.field12 = 1
        self.field13 = 1
        self.field14 = 1
        self.field15 = 1
        self.field16 = 1
        self.field17 = 1
        self.field18 = 1
        self.field19 = 1
        self.field20 = 1
        self.field21 = 1
        self.field22 = 1
        self.field23 = 1
        self.field24 = 1
        self.field25 = 1
        self.field26 = 1
        self.field27 = 1
        self.field28 = 1
        self.field29 = 1

    def method0(self):
        return self.field0

    def method1(self):
        return self.field1

    def method2(self):
        return self.field2

    def method3(self):
        return self.field3

    def method4(self):
        return self.field4

    def method5(self):
        return self.field5

    def method6(self):
        return self.field6

    def method7(self):
        return self.field7

    def method8(self):
        return self.field8

    def method9(self):
        return self.field9

    def method10(self):
        return self.field10

    def method11(self):
        return self.field11

    def method12(self):
        return self.field12

    def method13(self):
        return self.field13

    def method14(self):
        return self.field14

    def method15(self):
        return self.field15

    def method16(self):
        return self.field16

    def method17(self):
        return self.field17

    def method18(self):
        return self.field18

    def method19(self):
        return self.field19

    def method20(self):
        return self.field20

    def method21(self):
        return self.field21

    def method22(self):
        return self.field22

    def method23(self):
        return self.field23

    def method24(self):
        return self.field24

    def method25(self):
        return self.field25

    def method26(self):
        return self.field26

    def method27(self):
        return self.field27

    def method28(self):
        return self.field28

    def method29(self):
        return self.field29


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
