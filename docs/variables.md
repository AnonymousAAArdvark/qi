# Variables

Variables store data values. In Qi, variables are declared using the `变量` keyword.

## Declaration

```c
变量 名字 = "Alice"；
变量 年龄 = 25；
变量 身高 = 1.75；
变量 是学生 = 真；
```

A variable without an initializer defaults to `空` (nil):

```c
变量 未知；
打印行（未知）；// 空
```

## Assignment

After declaration, assign a new value using `=`:

```c
变量 计数 = 0；
计数 = 计数 + 1；
打印行（计数）；// 1
```

## Scope

Variables are scoped to the block (`「」`) in which they are declared. Inner blocks can read and write outer variables:

```c
变量 外 = 10；
「
    变量 内 = 20；
    外 = 外 + 内；  // inner can access outer
    打印行（外）；   // 30
」
打印行（外）；       // 30
// 内 is not accessible here
```

## Naming rules

Variable names follow the same rules as identifiers (see [Syntax](syntax.md)):

- Start with a letter, underscore, or any Chinese character
- May contain letters, digits, underscores, or Chinese characters
- Case-sensitive

```c
变量 myVar = 1；
变量 _私有 = 2；
变量 变量名 = 3；   // Chinese identifiers are fine
```

## Constants

Qi does not have a dedicated constant declaration. By convention, use ALL_CAPS names for values you do not intend to reassign:

```c
变量 最大值 = 100；
```

## See also

- [Types](types.md)
- [Operators](operators.md)
- [Syntax](syntax.md)
