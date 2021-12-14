# Syntax
Qi's syntax is designed to be familiar to people coming from C-like languages such as C++, Go, and Swift. Most of the keywords, if translated, directly correlate to their english counterpart's function.

Scripts are stored in plan text files with a ```.qi``` file extension. Qi does not compile ahead of time: programs are run directly from source, from top to bottom like a typical scripting language. (Internally, programs are compiled to bytecode for [efficiency](performance.md), but that’s an implementation detail.)

## Comments
Line comments start with // and end at the end of the line:

```c
// This is a comment.
```
Block comments start with /* and end with */. They can span multiple lines:
```c
/* This
   is
   a
   multi-line
   comment.*/
```

## Reserved Words
Like many other programming languages Qi has some reserved keywords that assume a very specific meaning in the context of the source code:
```c
打断 继续 类 切换 案例 预设 否则 功能 而 对于 如果 空 返回 超 真 
假 这 变量 和 或 等 不等 打等 小等
```

## Identifiers
Naming rules are similar to other programming languages. Identifiers start with a letter or underscore and may contain letters, digits, and underscores. Case is sensitive.
```c
hi
camelCase
PascalCase
_under_score
abc123
ALL_CAPS
你好
one二3
```
> All Chinese characters and punctuation are supported.

## Semicolons
Semicolons (；) are meaningful in Qi. They are used for separating statements:
```c
// Two statements:
打印行（"你好"）；// Newline.
打印行（"再见"）；
```
Sometimes, though, a statement doesn’t fit on a single line and a line without a semicolon would trip it up. To handle that, Qi has a very simple rule: It ignores a newline following any token that can’t end a statement.
```c
打印行（ // Newline here is ignored.
"你好"）；
```
In practice, this means you can put each statement on its own line and wrap them across lines as needed without too much trouble.

# Blocks
Qi uses corner brackets (「」) to define blocks. You can use a block anywhere a statement is allowed, like in [control flow]() statements. [Method]() and [function]() bodies are also blocks. For example, here we have a block for the if case, and a single statement for the else:
```c
如果（高兴 和 知道）「
    手。拍；
」 否则 打印行（"伤心"）；
```
Typically, blocks contain a series of statements like:
```c
{
    打印行（"一"）；
    打印行（"二"）；
    打印行（"三"）；
}
```

## Precedence Table
| PREC 	| OPERATOR  	| DESCRIPTION                                       	| ASSOCIATES	|
|---	|-----------	|---------------------------------------------------	|------------	|
| 1 	| 【】        	| Subscript                                         	| Left-to-Right	|
| 2 	| 。（）++ --   	| Method Call, Grouping, Postfix Increment & Decrement	| Left-to-Right	|
| 3 	| 不 - ++ --  	| Not, Negate, Prefix Increment & Decrement            	| Right-to-Left	|
| 4 	| * /       	| Multiply, Divide                                  	| Left-to-Right	|
| 5 	| + -       	| Add, Subtract                                     	| Left-to-Right	|
| 6 	| 大 小 大等 小等	| Comparison                                        	| Left-to-Right	|
| 7 	| 等 不等    	| Equality                                           	| Left-to-Right	|
| 8 	| 和        	| Logical And                                       	| Left-to-Right	|
| 9 	| 或        	| Logical Or                                        	| Left-to-Right	|
| 10 	| = += -=   	| Assignment, Setter                                	| Right-to-Left	|
