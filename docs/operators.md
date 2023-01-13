# Operators
An operator is a special symbol or phrase that you use to check, change, or combine values. For example, the addition operator (+) adds two numbers, as in ```变量 i = 1 + 2```, and the logical AND operator (和) combines two Boolean values, as in ```如果 (flag1 和 flag2)```.

## Arithmetic Operators
* Addition (+)
* Subtraction (-)
* Multiplication (*)
* Division (/)
* Remainder (%)

```c
变量 n1 = 1 + 2        // equals 3
变量 n2 = 5 - 3        // equals 2
变量 n3 = 2 * 3        // equals 6
变量 n4 = 10.0 / 2.5   // equals 4.0
变量 n5 = 9 % 4        // equals 1
```

## Negate Operator
The negate operator ```-``` flips the sign of the value.
```c
变量 n1 = 5        // equals 5
变量 n2 = -n1        // equals -5
变量 n3 = -n2        // equals 5
```

## Increment / Decrement Operators
The increment / decrement operators add or subtract 1 from the given number value.

Based on the placement of the operator, the operation can be either evaluated before (prefix) or after (postfix) the expression. 

```c
变量 n = 0
系统。打印行（++n）          // equals 1
系统。打印行（--n）          // equals 0
系统。打印行（n++）          // equals 0
系统。打印行（n）            // equals 1 (n is incremented afterwards)
```

## Bitwise Operators
Bitwise operators operate on binary numerals at the level of its individual bits.
* Not (位不)
* And (位和)
* Or (位或)
* Xor (位异或)
* Left Shift (位左移)
* Right Shift (位右移)

```c
系统。打印行（位不 0B0111）         // equals 8
系统。打印行（0B0101 位和 0B0011）  // equals 1
系统。打印行（0B0101 位或 0B0011）  // equals 7
系统。打印行（0B0101 位异或 0B0011）// equals 6
系统。打印行（0B0101 位左移 1）     // equals 10
系统。打印行（0B0101 位右移 1）     // equals 2
```

## Concatenation Operator
The concatenation operator ```+``` combines two strings together.
```c
变量 s1 = "一"        // equals 一
变量 s2 = "二"        // equals 二
变量 s3 = s1 + s2     // equals 一二
变量 s4 = s3 + "三"   // equals 一二三
```

## Assignment Operator
The assignment operator ```=``` initialize or update a value:
```c
变量 a = 50       // a = 50
变量 b = a        // b = 50
变量 c = a * b    // c = 50 * 50
```
Please note that contrary to many other programming languages, the assignment operator has no side effect, it means that it does not return any value.

## Comparison Operators
The comparison operators return a boolean value to indicate whether or not the statement is true:

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

## Logical Operators
The comparison operators return a boolean value to indicate whether the statement is true:

* Logical NOT (不)
* Logical AND (和)
* Logical OR (或)
```c
不 1          // 真 because 1 is true
1 和 0      // 假 because one of the two values is false
1 或 0      // 真 because one of the two values is true
```

## Subscript Operator
The subscript operator ```【】``` is used to access and modify items in lists or strings.
```c
变量 l =【1，2，3，4，5】
系统。打印行（l【2】）        // equals 3
系统。打印行（l【-1】）       // equals 5
l【0】= 0
系统。打印行（l）              // equals 【0，2，3，4，5】
```