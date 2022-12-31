[![Lines of Code][loc-shield]][loc-url]
[![License][license-shield]][license-url]
[![Website][website-shield]][website-url]
[![Size][size-shield]][size-url]
<!-- PROJECT LOGO -->
<br />
<div align="center">
  <a href="https://github.com/anonymousaaardvark/qi">
    <img src="docs/assets/images/logo.png" alt="Logo" width="80" height="80">
  </a>

<h2 align="center">气 (Qi) 语</h3>

  <p align="center">
    气是一种轻量级、快速、开源的中文编程语言。
    <br />
    <a href="https://qilang.tk"><strong>qilang.tk »</strong></a>
    <br /><br />
    <a href="https://github.com/AnonymousAAArdvark/qi/blob/master/README.md">English</a> |
    中文
  </p>
</div>

[![Background][background-img]]()

<!-- ABOUT THE PROJECT -->
## 关于

在读了一篇关于如何创建编程语言的文章后，我受到了启发，自己创建了一个解释器。我查看了几种语言是如何实现的，并意识到它们都是基于英语的。我想在“外国”自然语言（如汉语）的基础上开发一种编程语言，看看它可能会带来哪些缺点。我从参考网上发布的各种资源开始（最著名的是 [Crafting Interpreters](https://github.com/munificent/craftinginterpreters/) )，然后慢慢地开始主要靠我自己实施概念。事实证明，从头开始创建一个完整的编程语言是相当困难的，但这正是它的乐趣所在。

<ins>为什么叫「Qi」</ins>？Qi是从汉字的拼音中衍生出来的「气」, 这在英语中是「空气」的意思。我希望这种语言能够快速、轻巧，并且完全透明，就像空气一样！
```c
打印行（"你好，世界"）；
```
### 特征
- 气**在中文基础上**。说英语的人通常不会想当然地认为他们所学的编程语言都是基于他们已经很流利的母语。以英语为母语的人可以通过阅读关键字（“if”、“int”、“while” 等）合理地推断Python代码的功能。让这种语言以外语为基础，可以让全球更多的人更容易地使用编程。


- 气**速度很快**。气使用单通道编译器编译出高效的字节码，这种字节码可以异常精确地运行。以及各种优化，例如 [Nan Boxing](https://sean.cm/a/nan-boxing) 通过哈希表探测，气的速度与竞争对手的动态语言相当。


- 气**是轻的**。由于不到5000行代码，整个项目可以在一天内阅读。虽然代码很短，但所有内容都格式良好，易于理解。最终的可执行文件大小只有约140 kb，允许您在几乎任何可以想象的设备上运行它。


- 气**是面向对象的**。将类放在前面和中心，[面向对象](https://en.wikipedia.org/wiki/Object-oriented_programming )编程提供了一个清晰的模块化结构，通过[抽象](https://en.wikipedia.org/wiki/Abstraction_principle_(computer_programming) )和[封装](https://en.wikipedia.org/wiki/Encapsulation_(computer_programming) )等概念，它在处理复杂性方面比函数式编程要好得多。
## 性能

虽然运行一些基准测试并不太擅长判断性能，但它们看起来很酷。。。这里有几个。

每个基准测试运行5次，每种语言的平均运行时间。

[![Performance Graph][performance-img]]()

这些测试在我的 Macbook Air 2017 上运行，配备双核 i5 CPU 和 8 GB RAM。

基准测试的代码可以在[test 文件夹](https://github.com/AnonymousAAArdvark/qi/tree/master/test/benchmark )中找到。

<!-- Quick Start -->
## 开始
### 先决条件
* make
    * macOS
      ```bash
      brew install make
      ```
    * Windows
      ```bash
      choco install make
      ```
    * Linux
        * 应该被纳入系统

### 安装

1. 克隆或下载git repo的zip。
6. 打开终端，然后 ```cd``` 到```/src/cmake-build-release```
7. 运行 ```make```.
8. 全部完成！



## 单元测试
单元测试非常重要（特别是对于编程语言），我已经包含了一个完整的气测试套件，以确保解释器的每个方面都做它应该做的事情。测试程序可以在```/test```目录中找到[这里]( https://github.com/AnonymousAAArdvark/qi/tree/master/test )。为了自动化测试过程，[这里]( https://github.com/AnonymousAAArdvark/qi/blob/master/utils/test.go ) 提供的 Go 程序 ```/utils/test.go``` 运行每个使用输入的解释器测试程序，捕获输出并验证结果。
###运行测试程序
要运行测试，您需要将 Go 安装到您的系统上。从[此处]( https://go.dev/doc/install )下载并安装它。

执行 Go 程序相当容易。只需运行 ```go run test.go``` 并将所需解释器的路径传递到 ```-interpreter=``` 参数中。
```bash
go run test.go -interpreter=解释器的路径在这里
```
如果您忘记实现否定运算符，以下是测试程序的输出：
```bash
$ go run test.go -interpreter=PATH
FAIL ../test/array/indexing.qi
     Expected output '假' on line 7 and got 'b'.
FAIL ../test/array/methods.qi
     Expected output '【零，1，2，3】' on line 9 and got '【零，2，3，4】'.
FAIL ../test/for/scope.qi
     Expected output '-1' on line 10 and got '1'.
FAIL ../test/number/literals.qi
     Expected output '-0' on line 4 and got '0'.
     Expected output '-0.001' on line 7 and got '0.001'.
FAIL ../test/operator/negate.qi
     Expected output '-3' on line 1 and got '3'.
FAIL ../test/string/indexing.qi
     Expected output 'g' on line 7 and got 't'.
268 tests passed. 6 tests failed.
exit status 1
```
如果所有测试都通过，这里是输出：
```bash
$ go run test.go -interpreter=PATH
All 274 tests passed (641 expectations).
```


## 句法

[入门]()页面是安装和设置语言的绝佳指南。

代码示例请参考[测试](https://github.com/AnonymousAAArdvark/qi/tree/master/test/).

要更深入地了解气，请访问[语言指南]().



<!-- ROADMAP -->
## 路线图

- [x] 类
- [x] 继承
- [x] 列表
- [x] 递增/递减运算符
- [x] 切换/预设语句
- [x] 优化
- [x] 将所有内容翻译成中文
- [x] 制作徽标
- [x] 完成自述文件
- [x] 将自述文件翻译成中文
- [x] 创建网站
    - [x] 首页
    - [x] 入门
    - [x] 语言指南
- [x] 转义序列
- [ ] 按位运算符
- [ ] 删除分号
- [ ] 支持科学记数法、二进制等数字
- [ ] 更多字符串方法
- [ ] 更多列表方法


<!-- CONTRIBUTING -->
## 贡献

贡献使开源社区成为学习、启发和创造的绝佳场所。 **非常感谢您做出的任何贡献**。

如果您有更好的建议，请 fork 存储库并创建拉取请求。您也可以简单地打开带有“增强”标签的问题。
别忘了给项目打星！再次感谢！

1. Fork 项目
2. 创建您的功能 branch (`git checkout -b feature/AmazingFeature`)
3. Commit 你的更改 (`git commit -m 'Add some AmazingFeature'`)
4. Push 到 Branch (`git push origin feature/AmazingFeature`)
5. 打开一个 Pull Request



<!-- LICENSE -->
## 执照

在 MIT 许可下分发。有关更多信息，请参阅“LICENSE.txt”。


<!-- CONTACT -->
## 接触

Andrew Yang - [@anonymousaaardvark](https://twitter.com/anonymousaaardvark) - andrewhuiyang0830@gmail.com

项目链接: [https://github.com/anonymousaaardvark/qi](https://github.com/anonymousaaardvark/qi)



<!-- ACKNOWLEDGMENTS -->
## 致谢

* [Crafting Interpreters](craftinginterpreters.com/)
* [Gravity](https://github.com/marcobambini/gravity)
* [Loxido](https://github.com/ceronman/loxido)

<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[loc-shield]: https://img.shields.io/tokei/lines/github/anonymousaaardvark/qi?style=for-the-badge
[loc-url]: https://github.com/anonymousaaardvark/qi
[last-commit-shield]: https://img.shields.io/github/last-commit/anonymousaaardvark/qi?style=for-the-badge
[last-commit-url]: https://github.com/anonymousaaardvark/qi
[commit-activity-shield]: https://img.shields.io/github/commit-activity/m/anonymousaaardvark/qi?style=for-the-badge
[commit-activity-url]: https://github.com/anonymousaaardvark/qi
[license-shield]: https://img.shields.io/github/license/anonymousaaardvark/qi?style=for-the-badge
[license-url]: https://github.com/anonymousaaardvark/qi
[website-shield]: https://img.shields.io/website?down_color=lightgrey&down_message=offline&style=for-the-badge&up_color=blue&up_message=online&url=https%3A%2F%2Fqilang.tk
[website-url]: https://qilang.tk
[size-shield]: https://img.shields.io/github/repo-size/anonymousaaardvark/qi?style=for-the-badge
[size-url]: https://github.com/anonymousaaardvark/qi
[background-img]: docs/assets/images/background.jpeg
[performance-img]: docs/assets/images/performance.png