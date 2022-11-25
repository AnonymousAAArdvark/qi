# 字符串 (string)
Strings are a mutable list of characters. All characters encoded in unicode are supported.

To declare a literal, surround text in two double quotes ("").
```c
"你好，世界"
```

## Static Methods

#### string。**长度**（）
Returns the length of the string.
```c
变量 str = "你好"；
打印行（str。长度（））；  // 2
```
#### string。**大写**（）
Returns a string where all characters are in upper case.
```c
变量 str = "test"；
打印行（str。长度（））；  // TEST
```
#### string。**小写**（）
Returns a string where all characters are in lower case.
```c
变量 str = "TEST"；
打印行（str。长度（））；  // test
```
#### string。**子串**（number）
Returns a part of a string between starting index and the end of the string.
```c
变量 str = "零一二三"；
打印行（str。子串（1））；  // 一二三
```
#### string。**子串**（number, number）
Returns a part of a string between starting index and ending index.
```c
变量 str = "零一二三"；
打印行（str。子串（1，3））；  // 一二
```