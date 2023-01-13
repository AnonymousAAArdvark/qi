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
0xFF   // hexadecimal
0O7777 // octal
0B0101 // binary
1.2e5  // scientific notation
```

## Static Constants

#### **数字。圆周率**
The value of π.
#### **数字。欧拉数**
The value of e.
#### **数字。无穷大**
The value of ∞.
#### **数字。不数字**
A value representing NaN.
#### **数字。最大值**
The largest representable number value.
#### **数字。最小值**
The smallest representable number value.
#### **数字。最大安全**
The largest integer that Qi can safely represent. It’s a constant value of 9007199254740991.

This is relevant because Qi uses double precision floating-point format for numbers, which can only safely represent integers between -(2<sup>53</sup> - 1) and 2<sup>53</sup> - 1.
#### **数字。最小安全**
The smallest integer that Qi can safely represent. It’s a constant value of -9007199254740991.

## Static Methods

#### **数字。最小**（数字，数字）
Returns the smallest number out of the two arguments.
```c
系统。打印行（数字。最小（1, 2）） // 1
```
#### **数字。最大**（数字，数字）
Returns the largest number out of the two arguments.
```c
系统。打印行（数字。最大（1, 2）） // 2
```
#### **数字。四舍五入**（数字）
Rounds the given number to the nearest integer.
```c
系统。打印行（数字。四舍五入（1.5）） // 2
```
#### **数字。四舍五入**（数字，数字）
Rounds the first argument to the number of decimal places given by second argument.
```c
系统。打印行（数字。四舍五入（1.57, 1）） // 1.6
```
#### **数字。平方根**（数字）
Takes the square root of the input number.
```c
系统。打印行（数字。平方根（4）） // 2
```
#### **数字。次方**（数字，数字）
Computes the first argument raised to the second argument.
```c
系统。打印行（数字。次方（4, 2）） // 16
```
#### **数字。对数**（数字）
Returns the base 10 logarithm of the given number.
```c
系统。打印行（数字。对数（1e5）） // 5
```
#### **数字。对数**（数字， 数字）
Returns the logarithm of the first number with the base given as the second number.
```c
系统。打印行（数字。对数（8，2）） // 3
```
#### **数字。正弦**（数字）
Takes the sin of the given number (in radians).
```c
系统。打印行（数字。正弦（数字。圆周率）） // 0
```
#### **数字。余弦**（数字）
Takes the cos of the given number (in radians).
```c
系统。打印行（数字。余弦（数字。圆周率）） // -1
```
#### **数字。正切**（数字）
Takes the tan of the given number (in radians).
```c
系统。打印行（数字。正切（数字。圆周率）） // 0
```
#### **数字。反正弦**（数字）
Takes the arcsin of the given number (in radians).
```c
系统。打印行（数字。反正弦（1）） // 1.5708
```
#### **数字。反余弦**（数字）
Takes the arccos of the given number (in radians).
```c
系统。打印行（数字。反余弦（1）） // 0
```
#### **数字。反正切**（数字）
Takes the arctan of the given number (in radians).
```c
系统。打印行（数字。反正切（1）） // 0.785398
```
#### **数字。上限**（数字）
Rounds the number up to the nearest integer.
```c
系统。打印行（数字。上限（1.001）） // 2
```
#### **数字。下限**（数字）
Rounds the number down to the nearest integer.
```c
系统。打印行（数字。下限（1.999）） // 1
```
#### **数字。随机**（）
Returns a random integer that is either 1 or 0.
```c
系统。打印行（数字。随机（）） // 1 or 0
```
#### **数字。随机**（数字）
Returns a random integer that is between 0 and the given value.
```c
系统。打印行（数字。随机（5）） // 0, 1, 2, 3, 4, or 5
```
#### **数字。随机**（数字, 数字）
Returns a random integer that is between the two given values.
```c
系统。打印行（数字。随机（3，5）） // 3, 4, or 5
```
#### **数字。数到串**（数字）
Converts the given number to a string.
```c
系统。打印行（数字。数到串（123）+ "4"） // 1234
```
