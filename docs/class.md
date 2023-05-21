# 类 (Class)
Most values in Qi are objects, and every object is an instance of a class. 

Classes define an object's behavior and state. Behavior is defined by methods which live in the class. Every object of the same class supports the same methods. State is defined in fields, whose values are stored in each instance.

## Defining a Class
To declare a class, use the ```类``` keyword followed by the class name:
```c
类 树 「
」
```

## Methods
Methods are functions that are declared within a class and adds functionality to the objects that belong in that class.
```c
类 树 「
    打印（）「
        系统。打印行（"我是一个树！"）
    」
」

变量 式 = 树（）
系统。打印行（式。打印（））  // 我是一个树！
```
Like regular functions, you can add parameters to methods.
```c
类 树 「
    打印（形容）「
        系统。打印行（"我是一个" + 形容 + "树！"）
    」
」

变量 式 = 树（）
系统。打印行（式。打印（"大"））  // 我是一个大树！
```

## Constructors
Constructors are called on initialization of a new object in a class. This is usually where class properties are declared and initialized. You are allowed to pass custom parameters into the constructor.
```c
类 树 「
    初始化（高度）「
        系统。打印行（"初始化新树"）
    」
」

变量 式 = 树（）  // 初始化新树
```

## Properties
Properties are variables that are declared within a class that adds states to the objects that belong in that class.
```c
类 树 「
    初始化（高度）「
        这。高度 = 高度
    」
」

变量 式 = 树（3）
```
#### Getting and Setting Properties
To get a property of an object, specify the object name and the property, separated by the ```。``` operator.
```c
类 树 「
    初始化（高度）「
        这。高度 = 高度
    」
」

变量 式 = 树（3）
系统。打印行（式。高度）  // 3
```
To set a property of an object, do the same thing that you would do with a normal variable using a ```=``` operator.
```c
类 树 「
    初始化（高度）「
        这。高度 = 高度
    」
」

变量 式 = 树（3）
系统。打印行（式。高度）  // 3
式。高度 = 4
系统。打印行（式。高度）  // 4
```
#### 这 (this) Keyword
The keyword ```这``` is used to specify that the variable you are accessing or modifying belongs to the enclosing class. This is especially useful in constructors, where the compiler may not be able to differentiate between the constructor parameter and the class property.
```c
类 树 「
    初始化（高度）「
        这。高度 = 高度
    」
    多高（）「
        系统。打印行（这。高度）
    」
」
```

## Inheritance
A class can inherit from a “parent” or superclass. When you invoke a method on an object of some class, if it can’t be found, it walks up the chain of superclasses looking for it there.

To specify a parent class, use the ```：``` operator.
```c
类 树 「
    打印（）「
        系统。打印行（"我是一个树！"）
    」
」

类 橡木：树「
」

变量 式 = 橡木（）
系统。打印行（式。打印（））  // 我是一个树！
```
When a class inherits from a superclass, all of the parent class's methods and properties are "passed down" to the child class.
#### 超 (super) Keyword
The keyword ```超``` is used within child classes to execute methods that they inherited from parent classes. This is useful when you are trying to execute a method when you want to access the superclass method being overridden.  
```c
类 树 「
    打印（种）「
        系统。打印行（"我是一个" + 种 + "树！"）
    」
」

类 橡木：树「
    打印（）「
        超。打印（"橡木"）
    」
」

变量 式 = 橡木（）
系统。打印行（式。打印（））  // 我是一个橡木树！
```
