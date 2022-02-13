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

#### **字符串。串到数**（字符串）
Converts the given string to a number.
```c
系统。打印行（字符串。串到数（"1"）+ 1） // 2
```

## Methods

#### **长度**（）
Returns the length of the string.
```c
变量 str = "你好"
系统。打印行（str。长度（））  // 2
```
#### **大写**（）
Returns a string where all characters are in upper case.
```c
变量 str = "test"
系统。打印行（str。长度（））  // TEST
```
#### **小写**（）
Returns a string where all characters are in lower case.
```c
变量 str = "TEST"
系统。打印行（str。长度（））  // test
```
#### **子串**（数字）
Returns a part of a string between starting index and the end of the string.
```c
变量 str = "零一二三"
系统。打印行（str。子串（1））  // 一二三
```
#### **子串**（数字, 数字）
Returns a part of a string between starting index and ending index.
```c
变量 str = "零一二三"
系统。打印行（str。子串（1，3））  // 一二
```
#### **指数**（字符串）
Returns the index of the first character matching the input string.
```c
变量 str = "零一二三"
系统。打印行（str。指数（"二三"））  // 2
```
#### **计数**（字符串）
Returns the amount of times the input string was found.
```c
变量 str = "零一二三二一零一二三一二一二"
系统。打印行（str。计数（"一二"））  // 4
```
#### **拆分**（字符串）
Splits the string by the given separator and stores the substrings in a list.
```c
变量 str = "零一二三二一零一二三一二一二"
系统。打印行（str。拆分（"一"））  // 【零，二三二，零，二三，二，二】
```
#### **替换**（字符串，字符串）
Returns a string with all occurrences of the 1st argument replaced with the 2nd argument.
```c
变量 str = "零一二三二一零一二三一二一二"
系统。打印行（str。替换（"一"，"四五"））  // 零四五二三二四五零四五二三四五二四五二
```
#### **修剪**（）
Returns a string with whitespace removed from the start and end of the input string
```c
变量 str = "·t·n  四是四  ·n·t"
系统。打印行（str。修剪（））  // 四是四
```
#### **修剪**（字符串）
Returns a new string with characters of the argument removed from the start and end of the input string.
```c
变量 str = "abbaba四是四babaab"
系统。打印行（str。修剪（"ab"））  // 四是四
```
#### **修剪始**（）
Returns a string with whitespace removed from the start of the input string
```c
变量 str = "·t·n  四是四  ·n·t"
系统。打印行（str。修剪始（））  // 四是四  ·n·t
```
#### **修剪始**（字符串）
Returns a new string with characters of the argument removed from the start of the input string.
```c
变量 str = "abbaba四是四babaab"
系统。打印行（str。修剪始（"ab"））  // abbaba四是四
```
#### **修剪端**（）
Returns a string with whitespace removed from the end of the input string
```c
变量 str = "·t·n  四是四  ·n·t"
系统。打印行（str。修剪端（））  // ·t·n  四是四
```
#### **修剪端**（字符串）
Returns a new string with characters of the argument removed from the end of the input string.
```c
变量 str = "abbaba四是四babaab"
系统。打印行（str。修剪端（"ab"））  // 四是四babaab
```