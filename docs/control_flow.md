# Control Flow
Control flow is used to determine which chunks of code are executed and how many times. *Branching* statements and expressions decide whether to execute some code and *looping* ones execute something more than once.

## 如果 (if) Statement
It is often useful to execute different pieces of code based on certain conditions. You might want to run an extra piece of code when an error occurs, or to display a message when a value becomes too high or too low. To do this, you make parts of your code conditional. In its simplest form, the ```如果``` statement has a single if condition. It executes a set of statements only if that condition is true:
```c
如果（真）「
    系统。打印行（"会打印"）  // 会打印
」
如果（假）「
    系统。打印行（"不会打印"）  // Won't print because statement is 假.
」
```
#### 否则 (else) Statement
The ```如果``` statement can provide an alternative set of statements, known as an else clause, for situations when the if condition is false. These statements are indicated by the ```否则``` keyword:
```c
如果（假）「
    系统。打印行（"不会打印"）  // Won't print because statement is 假.
」否则「
    系统。打印行（"会打印"）  // 会打印
」
```
#### 如果 否则 (if else) Statement
To chain extra conditions after the previous ones fail, you can combine the else clause with the if condition, forming ```否则 如果```.
```c
如果（假）「
    系统。打印行（"不会打印"）  // Won't print because statement is 假.
」否则 如果（真）「
    系统。打印行（"会打印"）  // 会打印
」否则 如果（真）「
    系统。打印行（"不会打印"）  // Won't print because previous condition evaluated to 真.
」否则「
    系统。打印行（"不会打印"）  // Won't print because an if condition evaluated to 真.
」
```

## 切换 (switch) Statement
There are many situations in which you need to match a specific value to many, many conditions. Sure, you can chain multiple ```如果``` statements, but up to a certain point the code becomes repetitive and hard to read:
```c
变量 数字 = 6
如果（数字 等 0）「
    系统。打印行（"零"）
」 
否则 如果（数字 等 1）「
    系统。打印行（"一"）
」 
否则 如果（数字 等 2）「
    系统。打印行（"二"）
」 
否则 如果（数字 等 3）「
    系统。打印行（"三"）
」 
否则 如果（数字 等 4）「
    系统。打印行（"四"）
」 
否则 如果（数字 等 5）「
    系统。打印行（"五"）
」 
否则 如果（数字 等 6）「
    系统。打印行（"六"）
」
// Output: 六
```
To make the above code shorter, a ```切换``` statement could be used. The switch statement is given a value, and compares it to the ```案例``` statements. It will only execute the code if the value and the case matches. 

Make sure to include a ```打断``` (break) statement at the end of the case if you don't want execution to jump into the next case.
```c
变量 数字 = 6
切换（数字）「
    案例 0：
        系统。打印行（"零"）
        打断
    案例 1：
        系统。打印行（"一"）
        打断
    案例 2：
        系统。打印行（"二"）
        打断
    案例 3：
        系统。打印行（"三"）
        打断
    案例 4：
        系统。打印行（"四"）
        打断
    案例 5：
        系统。打印行（"五"）
        打断
    案例 6：
        系统。打印行（"六"）
        打断
」
// Output: 六
```
#### 预设 (default) Statement
The ```预设``` statement is executed if the inputted value does not match any of the ```案例``` statements. 
```c
变量 数字 = 7
切换（数字）「
    案例 0：
        系统。打印行（"零"）
        打断
    案例 1：
        系统。打印行（"一"）
        打断
    案例 2：
        系统。打印行（"二"）
        打断
    案例 3：
        系统。打印行（"三"）
        打断
    案例 4：
        系统。打印行（"四"）
        打断
    案例 5：
        系统。打印行（"五"）
        打断
    案例 6：
        系统。打印行（"六"）
        打断
    预设：
        系统。打印行（"未知数字"）
        打断
」
// Output: 未知数字
```
#### Fallthrough Cases
If you want the code in the next case to be executed if the current case passes, simply remove the ```打断``` statement at the end of the case. This is useful if you want multiple value matches to execute the same code.
```c
变量 数字 = 1
切换（数字）「
    案例 0：
    案例 1：
    案例 2：
    案例 3：
    案例 4：
    案例 5：
        系统。打印行（"在1到5之间"）
        打断
    预设：
        系统。打印行（"没有在1到5之间"）
        打断
」
// Output: 在1到5之间
```