# Standard Lib
The Standard Library is a collection of useful functions that are built into the core language.

## Functions
#### **打印**（value）
Prints the specified message to the console.
```c
打印（"你好"）  
打印（"，世界"）
// output：
// 你好，世界
```
#### **打印行**（value）
Prints the specified message to the console followed by a newline.
```c
打印行（"你好"）  
打印行（"，世界"）
// output：
// 你好
// ，世界
```
#### **扫描**（）
Takes user input from console and returns it as a ```字符串```.
```c
打印（"请输入您的姓名："）
名字 = 扫描（）
打印行（"你好，" + 名字）
// output:
// 请输入您的姓名：奕辰
// 你好，奕辰
```
#### **时钟**（）
Returns the number of seconds since the program was started in decimal form. This is usually used for benchmarking.
```c
变量 开始 = 时钟（）
变量 结果 = 0
对于（变量 i = 0；i 小 10000000；i++）结果 += 2
打印行（结果）
打印行（时钟（） - 开始）
// output:
// 2e+07
// 0.490107
```
