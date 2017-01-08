LuaDisAss
=========
A lightweight disassembler and assembler for lua 5.3 bytecode

Usage
-----

### Disassembling
To disassemble a function dumped with string.dump, run
```
luadisass -d <dump> <output>
```

Example:
```
luadisass -d bytecode.luac disass.luas
```

This will create a assembly file that can be assembled using luadisass -a

### Assembling
To assemble a function into bytecode, run
```
luadisass -a <assembly> <dump>
```

Example:
```
luadisass -a disass.luas bytecode.luac
```





Notes
-----
* don't look at the code unless you hate yourself
* don't look at Assembler.cpp unless you really hate yourself