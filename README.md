# ICS2021 Programming Assignment
The ics2021 PA lab of NJU, finished PA0-PA4.4. This PA repository presently implements:
1. <b>nemu</b> is a computer emulator based on RISCV32 architecture with a single CPU, timer, serial, keyboard, VGA and etc;
2. <b>abstract-machine</b> with trm, ioe, ctx,vme and other kernal libs supports OS implemtation or other am applications;
3. <b>nanos-lite</b> is build on nemu and abstract-machine, which supports several system calls, context exchange, virtual memory mechanisim and a simple process scheduling method.
4. <b>navy</b> offering a series of runtime environment like libos, libc, libndl, libminisdl and etc to support  riscv32 application.

However, some unimportant parts of the PA remains unimplemented, including
  * trace tools in [基础设施(2)](https://nju-projectn.github.io/ics-pa-gitbook/ics2021/2.4.html), they are some debug architectures in PA;
  * libam in [精彩纷呈的应用程序](https://nju-projectn.github.io/ics-pa-gitbook/ics2021/3.5.html), which supports the am applications like typing-game running on the nano-lite
  * some optional parts including: 
    1. sound: sound device in nemu and abstract-machine is implemented, sound deivce read & write support in nano-lite and sound suuport of libndl and libminisdl in navy-apps is unimplemented.
    2. optional part in [编写不朽的传奇](https://nju-projectn.github.io/ics-pa-gitbook/ics2021/4.5.html)
  
The above may be implemented in the future

----
# Below Is the Initial README.md of PA
----
# ICS2021 Programming Assignment

This project is the programming assignment of the class ICS(Introduction to Computer System)
in Department of Computer Science and Technology, Nanjing University.

For the guide of this programming assignment,
refer to http://nju-ics.gitbooks.io/ics2021-programming-assignment/content/

To initialize, run
```bash
bash init.sh subproject-name
```
See `init.sh` for more details.

The following subprojects/components are included. Some of them are not fully implemented.
* [NEMU](https://github.com/NJU-ProjectN/nemu)
* [Abstract-Machine](https://github.com/NJU-ProjectN/abstract-machine)
* [Nanos-lite](https://github.com/NJU-ProjectN/nanos-lite)
* [Navy-apps](https://github.com/NJU-ProjectN/navy-apps)
