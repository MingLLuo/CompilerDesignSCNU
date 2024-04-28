Chinese:
- 华南师范大学计算机学院2021级编译原理项目源代码仓库
- Task1: 词法分析器生成器
  - 生成NFA/DFA
  - 生成词法分析器
  - 生成Token化文本(单词编码), 供Task2使用
- Task2: 语法分析器生成器
  - 生成LR0/SLR1分析器
  - 读取Token化文本, 生成语法树
- 源代码使用C++编写, GUI使用Qt6编写, 使用CMake构建
- 请使用C++17标准编译Lexer.cpp(由Task1中的CodeGen生成), 例如`g++ --std=c++17 lexer.cpp`

---

English:
- need more "clarify"
- also need more documentation
- many there are base English, only in GUI setting will show some Chinese

- Simple Description:
    - task 1:
        - Read a special classify text, Gen NFA/DFA to distinguish them
        - Gen Lexer
        - Gen Tokenized Text, work for task2
    - task 2:
        - Read a specail grammar text, include the prodcution rules
        - Support LR0/SLR1 Parse
        - Read tokenized text then Gen a parse tree~

- Whole project is first done in CUI mode, then transform to fit GUI, some CV action might make small difference in same task folder, but i think they will function well?(Maybe qaq

- Make sure to use higher Cpp Standard to compile Lexer.cpp(generate by CodeGen in task1), for example `g++ --std=c++17 lexer.cpp`

- GUI interface use Qt6, with CMake backend, QMake sometimes(always) work worse in my mac, banned now.

- Use CLion, VSCode and Qt-Creator with Debug may be a better choice?

