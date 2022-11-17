## asciidraw 🎨

**asciidraw** is a program that lets you draw ascii shapes in your terminal.

#### Launching the interpreter

You can compile **asciidraw** using the standard C compiler `gcc`, and then
invoking the resulting binary to launch the interpreter:

```bash
$ gcc -o asciidraw asciidraw.c
$ ./asciidraw
```

#### Example

Below is a sample asciidraw program:

```
GRID 20 20
CIRCLE 10,10,5
LINE 0,0 19,19
LINE 19,0 0,19
DISPLAY
END
```

That will produce the following beautiful result:

```
0 *                  *
1  *                *
2   *              *
3    *            *
4     *    ***   *
5      *  *   * *
6       **     *
7       **    * *
8      *  *  *   *
9      *   **    *
0      *   **    *
1       * *  *  *
2        *    **
3       * *   **
4      *   ***  *
5     *          *
6    *            *
7   *              *
8  *                *
9 *                  *
 01234567890123456789
```
