# Types

Qi is a dynamically typed programming language, which simply means that a single variable could hold any data type at different points in time. Most data types are objects, such as classes and functions. However, numbers, booleans, and nils are not objects.

The built-in types are [布尔 (boolean)](), [数字 (number)](), [空 (nil)](), [实例 (instance)](), [功能 (function)](), [字符串 (string)](), [功能 (function)](), [列表 (list)](), and [类 (class)]().

## Native Functions

#### **型**（value）
Prints the type of the inputted value.
```c
打印行（型（1234））； // 数字
```
#### **串到数**（string）
Converts the given string to a number.
```c
打印行（串到数（"1"）+ 1）； // 2
```
#### **数到串**（number）
Converts the given number to a string.
```c
打印行（数到串（123）+ "4"）； // 1234
```