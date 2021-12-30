# 数字 (number)
Like most other scripting languages, Qi represents all numeric values with a double-precision floating point. Number literals look very similar to other languages:
```c
123
0
-123
1.5
.4562
-930248.64323
10000000000
```

## Native Functions

#### **最小**（number，number）
Returns the smallest number out of the two arguments.
```c
打印行（最小（1, 2））； // 1
```
#### **最大**（number，number）
Returns the largest number out of the two arguments.
```c
打印行（最大（1, 2））； // 2
```
#### **四舍五入**（number）
Rounds the given number to the nearest integer.
```c
打印行（四舍五入（1.5））； // 2
```
#### **四舍五入**（number，number）
Rounds the first argument to the number of decimal places given by second argument.
```c
打印行（四舍五入（1.57, 1））； // 1.6
```
#### **平方根**（number）
Takes the square root of the input number.
```c
打印行（平方根（4））； // 2
```
#### **次方**（number，number）
Computes the first argument raised to the second argument.
```c
打印行（次方（4, 2））； // 16
```
