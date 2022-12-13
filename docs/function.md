# 功能 (Function)
Functions are pieces of code that you can execute in various areas of a program. They can be stored in variables, passed into functions, returned by functions, and even put in lists.

## Defining a Function
To declare a function, use the ```功能``` keyword followed by the function name:
```c
功能 说你好（）「
    打印行（"你好"）
」

说你好（）  // 你好
```

## Parameters
To pass values to a function, you can provide a parameter list in the function declaration.

Parameters can be added by listing out argument names separated by commas (，).
```c
功能 说你好（名字）「
    打印行（"你好" + 名字）
」

说你好（"世界"）  // 你好世界
```
It’s an error to call a function with fewer arguments than its parameter list expects. If you pass too *many* arguments, the extras are ignored.

## Returning values
Functions without the ```返回``` (return) keyword returns ```空``` by default. You can explicitly return a value using a ```返回``` statement, followed by the value you want returned.
```c
功能 加一（数字）「
    返回 数字 + 1
」

打印行（加一（1））  // 2
```

## Recursion
Function recursion is fully supported in Qi:
```c
功能 斐波（数字）「
    如果（数字 小 2）返回 数字
    返回 斐波（数字 - 2） + 斐波（数字 - 1）
」
 
打印行（斐波（20））  // 6765
```
