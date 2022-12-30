# 单元测试
单元测试非常重要（特别是对于编程语言），我已经包含了一个完整的 Qi 测试套件，以确保解释器的每个方面都做它应该做的事情。测试程序可以在```/test``` 目录[这里](https://github.com/AnonymousAAArdvark/qi/tree/master/test) 中找到。为了自动化测试过程，[这里](https://github.com/AnonymousAAArdvark/qi/blob/master/utils/test.go) 提供的 Go 程序 ```/utils/test.go``` 运行每个使用输入的解释器测试程序，捕获输出并验证结果。
## 运行测试程序
要运行测试，您需要将 Go 安装到您的系统上。从 [此处](https://go.dev/doc/install) 下载并安装它。

执行 Go 程序相当容易。只需运行 ```go run test.go``` 并将所需解释器的路径传递到 ```-interpreter=``` 参数中。
```bash
go run test.go -interpreter=解释器的路径在这里
```
如果您忘记实现否定运算符，以下是测试程序的输出：
```bash
$ go run test.go -interpreter=PATH
FAIL ../test/array/indexing.qi
     Expected output '假' on line 7 and got 'b'.
FAIL ../test/array/methods.qi
     Expected output '【零，1，2，3】' on line 9 and got '【零，2，3，4】'.
FAIL ../test/for/scope.qi
     Expected output '-1' on line 10 and got '1'.
FAIL ../test/number/literals.qi
     Expected output '-0' on line 4 and got '0'.
     Expected output '-0.001' on line 7 and got '0.001'.
FAIL ../test/operator/negate.qi
     Expected output '-3' on line 1 and got '3'.
FAIL ../test/string/indexing.qi
     Expected output 'g' on line 7 and got 't'.
268 tests passed. 6 tests failed.
exit status 1
```
如果所有测试都通过，这里是输出：
```bash
$ go run test.go -interpreter=PATH
All 274 tests passed (641 expectations).
```