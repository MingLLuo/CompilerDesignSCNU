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

