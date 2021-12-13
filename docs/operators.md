# Operators
An operator is a special symbol or phrase that you use to check, change, or combine values. For example, the addition operator (+) adds two numbers, as in ```变量 i = 1 + 2；```, and the logical AND operator (和) combines two Boolean values, as in ```如果 (flag1 和 flag2)；```.

## Arithmetic Operators
* Addition (+)
* Subtraction (-)
* Multiplication (*)
* Division (/)
* Remainder (%)
```c
变量 n1 = 1 + 2；        // equals 3
变量 n2 = 5 - 3；        // equals 2
变量 n3 = 2 * 3；        // equals 6
变量 n4 = 10.0 / 2.5；   // equals 4.0
变量 n5 = 9 % 4；        // equals 1
```

## Assignment Operator
The assignment operator = initialize or update a value:
```c
变量 a = 50；       // a = 50
变量 b = a；        // b = 50
变量 c = a * b；    // c = 50 * 50
```
Please note that contrary to many other programming languages, the assignment operator has no side effect, it means that it does not return any value.

# Comparison Operators
The comparison operators return a Bool value to indicate whether or not the statement is true:

* Equal (等)
* Not equal (不等)
* Less than (小)
* Less than or equal (小等)
* Greater than (大)
* Greater than or equal (大等)
```c
1 等 1        // 真 because 1 is equal to 1
1 不等 2      // 真 because 1 is not equal to 2
1 小 2        // 真 because 1 is less than 2
1 小等 1      // 真 because 1 is less than or equal to 1
1 大 2        // 假 because 1 is not greater than 2
1 大等 1      // 真 because 1 is greater than or equal to 1
```
