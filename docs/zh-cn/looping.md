# Looping
It’s hard to write a useful program without executing some chunk of code repeatedly. To do that, you use looping statements. There are two in Qi, and they should be familiar if you’ve used other imperative languages.
## 而 (while) Loop
A ```而``` loop performs a set of statements until a condition becomes false. These kinds of loops are best used when the number of iterations is not known before the first iteration begins.
```c
变量 i = 0；
变量 j = 0；
而（i 小 100）「
    j += i；
    如果（i % 2 等 0）「
        i += 3；
    」 否则「
        i += 1；
    」
」
打印行（j）；  // 2475
```
## 对于 (for) Loop
```而``` loops are useful when you want to loop indefinitely or according to some complex condition. But in most cases, you’re looping through a list, a series of numbers, or some other “sequence” object. ```对于``` loops can make the process significantly easier.

There are 3 optional clauses that you can to pass into the ```对于``` loop:
- The initializer can be a variable declaration or an expression. It runs once at the beginning of the statement.
- The condition clause is an expression. Like in a ```而``` loop, we exit the loop when it evaluates to something ```假```.
- The increment expression runs once at the end of each loop iteration.
```c
变量 列表 = 【"一"，"二"，"三"，"四"，"五"】；
对于（变量 i = 0；i 小 列表。长度（）；i++）「
    打印（列表【i】+ " "）；
」
打印行（""）；
// Output: 一 二 三 四 五
```

## 打断 (break) Statement
To immediately exit an executing loop, a ```打断``` statement can be used. This effectively bails you out of the innermost enclosing ```对于``` or ```而``` loop.
```c
变量 日志 = 【"好"，"好"，"好"，"错误"，"好"，"好"】；
对于（变量 i = 0；i 小 日志。长度（）；i++）「
    如果（日志【i】等 "错误"）「
        打印行（"错误在：" + 数到串（i）+ "行"）；
        打断；
    」
」
// Output: 错误在：3行
```
## 继续 (continue) Statement
To immediately skip over the rest of the current iteration, a ```继续``` statement can be used. This effectively jumps the execution to the beginning of the next iteration of the  ```对于``` or ```而``` loop.
```c
变量 列表 = 【482，9654，861，6720，5738，2045，18397】；
对于（变量 i = 0；i 小 列表。长度（）；i++）「
    如果（列表【i】% 2 等 0）继续；
    打印（数到串（列表【i】）+ " "）；
」
打印行（""）；
// Output: 861 2045 18397
```