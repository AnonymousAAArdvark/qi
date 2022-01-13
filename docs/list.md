# 列表 (List)
Lists are simple sequence of values with dynamic size. Each element can be quickly accessed with a number index. You can create a array by placing a sequence of comma-separated expressions inside square brackets (【】):
```c
【真，2，"三"】
```
The list elements don't have to be the same type.

## Static Methods

#### list。**长度**（）
Returns the length of the list.
```c
变量 test = 【"一"，"二"，"三"，"四"】
打印行（test。长度（））  // 4
```
#### list。**推**（value）
Push a value to the end of a list increasing the list's length by 1
```c
变量 test = 【"一"，"二"，"三"，"四"】
test。推（"五"）
打印行（test）  // 【"一"，"二"，"三"，"四"，"五"】
```
#### list。**弹**（）
Pop a value from the end of a list decreasing the list's length by 1.
```c
变量 test = 【"一"，"二"，"三"，"四"】
test。弹（）
打印行（test）  // 【"一"，"二"，"三"】
```
#### list。**插**（value，number）
Insert a value to the specified index of a list increasing the list's length by 1.
```c
变量 test = 【"一"，"二"，"三"，"四"】
test。插（"零"，0）
打印行（test）  // 【"零"，"一"，"二"，"三"，"四"】
```
#### list。**删**（number）
Delete an item from a list at the given index.
```c
变量 test = 【"一"，"二"，"三"，"四"】
test。插（0）
打印行（test）  // 【"二"，"三"，"四"】
```