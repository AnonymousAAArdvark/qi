# Unit Tests
Unit testing is so important (especially for a programming language) that I have included a full Qi test suite to make sure that every aspect of the interpreter does what it's supposed to do. The test programs can be found in the ```/test``` directory [here](https://github.com/AnonymousAAArdvark/qi/tree/master/test). To automate the testing procedure, the Go program ```/utils/test.go``` provided [here](https://github.com/AnonymousAAArdvark/qi/blob/master/utils/test.go) runs each test program with the inputted interpreter, captures the output, and validates the result.
## Running the Testing Program
To run the tests, you'll need to have Go installed onto your system. Download and install it from [here](https://go.dev/doc/install).

Executing the Go program is fairly easy. Simply run ```go run test.go``` and pass the path to the desired interpreter into the ```-interpreter=``` argument.
```bash
go run test.go -interpreter=PATH_TO_INTERPRETER_HERE
```
Here is the output of the testing program if you forget to implement the negate operator:
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
And here is the output if all tests pass:
```bash
$ go run test.go -interpreter=PATH
All 274 tests passed (641 expectations).
```