# 列表 (List)
Lists are simple sequence of values with dynamic size. Each element can be quickly accessed with a number index. You can create a array by placing a sequence of comma-separated expressions inside square brackets (【】):
```c
【真，2，"三"】
```
The list elements don't have to be the same type.

## Methods

#### **长度**（）
Returns the length of the list.
```c
变量 test = 【"一"，"二"，"三"，"四"】
系统。打印行（test。长度（））  // 4
```
#### **推**（值）
Push a value to the end of a list increasing the list's length by 1
```c
变量 test = 【"一"，"二"，"三"，"四"】
test。推（"五"）
系统。打印行（test）  // 【"一"，"二"，"三"，"四"，"五"】
```
#### **弹**（）
Pop a value from the end of a list decreasing the list's length by 1.
```c
变量 test = 【"一"，"二"，"三"，"四"】
test。弹（）
系统。打印行（test）  // 【"一"，"二"，"三"】
```
#### **插**（值，数字）
Insert a value to the specified index of a list increasing the list's length by 1.
```c
变量 test = 【"一"，"二"，"三"，"四"】
test。插（"零"，0）
系统。打印行（test）  // 【"零"，"一"，"二"，"三"，"四"】
```
#### **删**（数字）
Delete an item from a list at the given index.
```c
变量 test = 【"一"，"二"，"三"，"四"】
test。插（0）
系统。打印行（test）  // 【"二"，"三"，"四"】
```
#### **排序**（）
Sorts the list in ascending order first by numbers, then strings.
```c
变量 test = 【"c"，3，"b"，2，0，"d"，"a"，1】
test。排序（）
系统。打印行（test）  // 【0，1，2，3，a，b，c，d】
```
#### **排序**（关闭）
Sorts the list in ascending order by the given closure. The closure needs to take in 2 arguments and return a boolean.
```c
功能 比较（a，b）「
    返回 （a % 2）小（b % 2）
」
变量 test = 【1，5，1，6，45，8，7，6，53，2，458，93】
test。排序（比较）
系统。打印行（test）  // 【1，5，1，45，7，53，93，6，8，2，458，6】
```
#### **过滤**（关闭）
Returns a filtered list based on the given closure. The closure needs to take in 1 argument and return a boolean.
```c
功能 滤（a）「
    返回 （a % 2）等 1
」
变量 test = 【1，5，1，6，45，8，7，6，53，2，458，93】
系统。打印行（test。过滤（滤））  // 【1，5，1，45，7，53，93】
```