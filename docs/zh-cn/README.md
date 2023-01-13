<div align="center">
  <a href="https://github.com/anonymousaaardvark/qi">
    <img src="assets/images/logo_transp.png" alt="Logo" width="80" height="80">
  </a>
</div>

<h2 align="center">气 (Qi) 语</h3>

  <p align="center">
    气是一种轻量级、快速、开源的中文编程语言。
    <br />
  </p>

<!-- ABOUT THE PROJECT -->
## 关于

在读了一篇关于如何创建编程语言的文章后，我受到了启发，自己创建了一个解释器。我查看了几种语言是如何实现的，并意识到它们都是基于英语的。我想在“外国”自然语言（如汉语）的基础上开发一种编程语言，看看它可能会带来哪些缺点。我从参考网上发布的各种资源开始（最著名的是 [Crafting Interpreters](https://github.com/munificent/craftinginterpreters/) )，然后慢慢地开始主要靠我自己实施概念。事实证明，从头开始创建一个完整的编程语言是相当困难的，但这正是它的乐趣所在。

<ins>为什么叫「Qi」</ins>？Qi是从汉字的拼音中衍生出来的「气」, 这在英语中是「空气」的意思。我希望这种语言能够快速、轻巧，并且完全透明，就像空气一样！
```c
系统。打印行（"你好，世界"）
```
#### 特征
- 气**在中文基础上**。说英语的人通常不会想当然地认为他们所学的编程语言都是基于他们已经很流利的母语。以英语为母语的人可以通过阅读关键字（“if”、“int”、“while” 等）合理地推断Python代码的功能。让这种语言以外语为基础，可以让全球更多的人更容易地使用编程。


- 气**速度很快**。气使用单通道编译器编译出高效的字节码，这种字节码可以异常精确地运行。以及各种优化，例如 [Nan Boxing](https://sean.cm/a/nan-boxing) 通过哈希表探测，气的速度与竞争对手的动态语言相当。


- 气**是轻的**。由于不到5000行代码，整个项目可以在一天内阅读。虽然代码很短，但所有内容都格式良好，易于理解。最终的可执行文件大小只有约140 kb，允许您在几乎任何可以想象的设备上运行它。


- 气**是面向对象的**。将类放在前面和中心，[面向对象](https://en.wikipedia.org/wiki/Object-oriented_programming )编程提供了一个清晰的模块化结构，通过[抽象](https://en.wikipedia.org/wiki/Abstraction_principle_(computer_programming) )和[封装](https://en.wikipedia.org/wiki/Encapsulation_(computer_programming) )等概念，它在处理复杂性方面比函数式编程要好得多。

## 句法

[入门](quick_start.md)页面是安装和设置语言的绝佳指南。

代码示例请参考[测试](https://github.com/AnonymousAAArdvark/qi/tree/master/test/).

要更深入地了解气，请访问[语言指南](syntax.md).




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
- [x] 按位运算符
- [x] 删除分号
- [x] 支持科学记数法、二进制等数字
- [x] 更多字符串方法
- [x] 更多列表方法
- [ ] 模块系统


<!-- LICENSE -->
## 执照

在 MIT 许可下分发。有关更多信息，请参阅```LICENSE.txt```。


<!-- CONTACT -->
## 接触

Andrew Yang - [@anonymousaaardvark](https://twitter.com/anonymousaaardvark) - andrewhuiyang0830@gmail.com

项目链接: [https://github.com/anonymousaaardvark/qi](https://github.com/anonymousaaardvark/qi)



<!-- ACKNOWLEDGMENTS -->
## 致谢

* [Crafting Interpreters](https://craftinginterpreters.com/)
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