# 字符串 (string)
Strings are a mutable list of characters. All characters encoded in unicode are supported.

To declare a literal, surround text in two double quotes ("").
```c
"你好，世界"
```

## Escaping
Most other programming languages use a backslash (\) followed by a letter or combination of digits to declare escape sequences. However, since the backslash is not readily available on the standard pinyin keyboard, Qi uses the middle dot (·) in place of the backslash.
A handful of escape characters are supported:
```c
"·"" // A double quote character.
"··" // A middle dot.
"·a" // Alarm beep. 
"·b" // Backspace.
"·f" // Formfeed.
"·n" // Newline.
"·r" // Carriage return.
"·t" // Tab.
"·v" // Vertical tab.
```
> For more information on escape sequences, reference [this article](https://en.wikipedia.org/wiki/Escape_sequences_in_C).

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