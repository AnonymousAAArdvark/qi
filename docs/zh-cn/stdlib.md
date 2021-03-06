# 标准库
标准库是核心语言中内置的有用函数的集合。

## 功能
#### **系统。打印**（值）
将指定的消息打印到控制台。
```c
系统。打印（"你好"）  
系统。打印（"，世界"）
// 输出：
// 你好，世界
```
#### **系统。打印行**（值）
将指定的消息打印到控制台，后跟换行符。
```c
系统。打印行（"你好"）  
系统。打印行（"，世界"）
// 输出：
// 你好
// ，世界
```
#### **系统。扫描**（）
从控制台获取用户输入并将其作为```字符串``` 返回。
```c
系统。打印（"请输入您的姓名："）
名字 = 系统。扫描（）
系统。打印行（"你好，" + 名字）
// 输出：
// 请输入您的姓名：奕辰
// 你好，奕辰
```
#### **系统。时钟**（）
以十进制形式返回自程序启动以来的秒数。这通常用于基准测试。
```c
变量 开始 = 系统。时钟（）
变量 结果 = 0
对于（变量 i = 0；i 小 10000000；i++）结果 += 2
系统。打印行（结果）
系统。打印行（系统。时钟（） - 开始）
// 输出：
// 2e+07
// 0.490107
```

#### **系统。型**（值）
返回输入值的类型。
```c
系统。打印行（系统。型（1234）） // 数字
```