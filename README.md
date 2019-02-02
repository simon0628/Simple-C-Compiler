# Simple-C-Compiler

Still working on branch Grammar… 

Pull request is welcome!



## 简介

基础的C语言代码编译器，基于编译原理课设，内含较完整中文注释。



## 总体目标

1. 词法分析：增加记录词在源程序中的位置
1. 语法分析：改用SLR(1)分析
1. 语义分析：整体重构
1. 代码优化、目标代码生成：待定



## 文法

C语言文法参考来自**ANSI C Yacc grammar** (https://www.lysator.liu.se/c/ANSI-C-grammar-y.html#and-expression)

以条件分支语句为例，终结符均用引号包含，非终结符为下划线连接的短语

```
conditional_expression
	: logical_or_expression
	| logical_or_expression '?' expression ':' conditional_expression
	;
```

默认读取的文法文件为rules.dat



## TODOs

- [ ] 词法分析对新词的不支持
- [ ] 语法分析...