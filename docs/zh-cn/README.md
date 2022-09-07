<div align="center">
  <a href="https://github.com/anonymousaaardvark/qi">
    <img src="assets/images/logo.png" alt="Logo" width="80" height="80">
  </a>
</div>

<h2 align="center">气 (qi) 语</h3>

  <p align="center">
    气是一种轻量级、快速、开源的中文编程语言。
    <br />
  </p>

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
    - [ ] 语言指南



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
[background-img]: docs/images/background.jpeg
[performance-img]: docs/images/performance.png