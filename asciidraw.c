#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * The maximum number of arguments a command
 * can take.
 */
const int ARGS_MAX = 4;

/*
 * All available commands the interpreter
 * can evaluate.
 */
enum Command {
  CHAR,
  CIRCLE,
  CLEAR,
  DISPLAY,
  END,
  GRID,
  INVALID,
  LINE,
  POINT,
  RECTANGLE
};

/*
 * A struct array that corresponds a `Command`
 * enum with a string.
 */
const static struct {
  enum Command command;
  const char *str;
} COMMAND_STRING [] = {
  { CHAR,      "CHAR"      },
  { CIRCLE,    "CIRCLE"    },
  { CLEAR,     "CLEAR"     },
  { DISPLAY,   "DISPLAY"   },
  { END,       "END"       },
  { GRID,      "GRID"      },
  { LINE,      "LINE"      },
  { POINT,     "POINT"     },
  { RECTANGLE, "RECTANGLE" }
};

/*
 * Find a corresponding `Command` enum from a specified string.
 *
 * @param str The string to associate with a `Command` enum.
 * @return The correspoding `Command` enum.
 */
enum Command command_from_string(const char *str) {
  int size = (
    sizeof(COMMAND_STRING) /
    sizeof(COMMAND_STRING[0])
  );

  for (int i = 0; i < size; ++i)
    if (!strcmp(str, COMMAND_STRING[i].str))
      return COMMAND_STRING[i].command;

  return INVALID;
}

/*
 * Represents an `operation`, which is essentially a command
 * that contains its arguments and its original name.
 */
struct Operation {
  char* name;
  enum Command cmd;
  int args[ARGS_MAX];
};

/*
 * The canvas the user can draw on, holds all
 * relevant state on itself.
 */
struct Grid {
  char **state;
  char character;
  int width;
  int height;
  int initialized;
};

/*
 * A helper to check if a point (`x`, `y`) is in bounds of
 * the drawing area.
 *
 * @param grid A grid struct
 * @param x The x coordinate
 * @param y The y coordinate
 * @return Whether or not the point (`x`, `y`) is in bounds.
 */
int in_bounds(struct Grid grid, int x, int y) {
  return (
    x >= 0 &&
    x < grid.width &&
    y >= 0 &&
    y < grid.height
  );
}

/*
 * A helper to plot a single point on the passed in grid.
 *
 * @param grid A pointer to a grid
 * @param x The x coordinate.
 * @param y The y coordinate.
 */
void plot(struct Grid *grid, int x, int y) {
  if (!in_bounds(*grid, x, y)) return;
  grid->state[x][y] = grid->character;
}

/*
 * Bresenham's line drawing algorithm.
 *
 * Details for understanding can be found here:
 * https://en.wikipedia.org/wiki/Bresenham's_line_algorithm
 *
 * @param grid A pointer to a grid.
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param dx The difference in x values.
 * @param dy The difference in y values.
 */
void bresenham_line(
  struct Grid *grid,
  int x1,
  int y1,
  int x2,
  int y2,
  int dx,
  int dy,
  int decide
) {
  int pk = 2 * dy - dx;

  for (int i = 0; i <= dx; ++i) {
    x1 < x2 ? ++x1 : --x1;

    if (pk < 0) {
      decide ? plot(grid, y1, x1) : plot(grid, x1, y1);
      pk = pk + 2 * dy;
    } else {
      y1 < y2 ? ++y1 : --y1;
      decide ? plot(grid, y1, x1) : plot(grid, x1, y1);
      pk = pk + 2 * dy - 2 * dx;
    }
  }
}

/*
 * Bresenham's circle drawing algorithm.
 *
 * Details for understanding can be found here:
 * https://www.javatpoint.com/computer-graphics-bresenhams-circle-algorithm
 *
 * @param grid A pointer to a grid.
 * @param xc The center x coordinate.
 * @param yc The center y coordinate.
 * @param radius The circle radius.
 */
void bresenham_circle(
  struct Grid *grid,
  int xc,
  int yc,
  int radius
) {
  int x = 0, y = radius, d = 3 - 2 * radius;

  plot(grid, xc + x, yc + y);
  plot(grid, xc - x, yc + y);
  plot(grid, xc + x, yc - y);
  plot(grid, xc - x, yc - y);
  plot(grid, xc + y, yc + x);
  plot(grid, xc - y, yc + x);
  plot(grid, xc + y, yc - x);
  plot(grid, xc - y, yc - x);

  while (y >= x) {
    ++x;

    if (d > 0) {
      y--;
      d = d + 4 * (x - y) + 10;
    } else {
      d = d + 4 * x + 6;
    }

    plot(grid, xc + x, yc + y);
    plot(grid, xc - x, yc + y);
    plot(grid, xc + x, yc - y);
    plot(grid, xc - x, yc - y);
    plot(grid, xc + y, yc + x);
    plot(grid, xc - y, yc + x);
    plot(grid, xc + y, yc - x);
    plot(grid, xc - y, yc - x);
  }
}

/*
 * Handler for the `CHAR` operation.
 *
 * @param grid A pointer to a grid.
 * @param args [character, ..].
 */
void character(struct Grid *grid, int args[]) {
  grid->character = (char)args[0];
}

/*
 * Handler for the `CIRCLE` operation.
 *
 * @param grid A pointer to a grid
 * @param args [x, y, radius, ..].
 */
void circle(struct Grid *grid, int args[]) {
  int x = args[0], y = args[1], radius = args[2];

  if (!grid->initialized) {
    printf("error: Grid isn't initialized\n");
    return;
  }

  bresenham_circle(grid, x, y, radius);
}

/*
 * Handler for the `CLEAR` operation.
 *
 * @param grid A pointer to a grid
 */
void clear(struct Grid *grid) {
  for (int i = 0; i < grid->height; ++i)
    for (int j = 0; j < grid->width; ++j)
      grid->state[i][j] = ' ';
}

/*
 * Handler for the `DISPLAY` operation.
 *
 * @param grid A grid struct.
 */
void display(struct Grid grid) {
  int wrap = 10;

  if (!grid.initialized) {
    printf("error: Grid isn't initialized\n");
    return;
  }

  for (int i = 0; i < grid.width; ++i) {
    printf("%d ", ((9 - i) % wrap + wrap) % wrap);
    for (int j = 0; j < grid.height; ++j)
      printf("%c", grid.state[j][i]);
    printf("\n");
  }

  printf(" ");

  for (int i = 0; i < grid.width; ++i)
    printf("%d", ((9 - i) % wrap + wrap) % wrap);

  printf("\n");
}

/*
 * Handler for the `GRID` operation.
 *
 * @param grid A pointer to a grid.
 * @param args [width, height, ..].
 */
void grid(struct Grid *grid, int args[]) {
  int width = args[0], height = args[1];

  if (grid->initialized) {
    printf("error: Grid has already been initialized\n");
    return;
  }

  grid->state = (char**)malloc(sizeof(char*)*width);

  for (int i = 0; i < height; ++i)
    grid->state[i] = (char*)malloc(sizeof(char)*height);

  for (int i = 0; i < height; ++i)
    for (int j = 0; j < width; ++j)
      grid->state[i][j] = ' ';

  grid->width = width;
  grid->height = height;
  grid->initialized = 1;
}

/*
 * Handler for the `LINE` operation.
 *
 * @param grid A pointer to a grid.
 * @param args [x1, y1, x2, y2].
 */
void line(struct Grid *grid, int args[]) {
  int x1 = args[0], y1 = args[1], x2 = args[2], y2 = args[3];

  if (!grid->initialized) {
    printf("error: Grid isn't initialized\n");
    return;
  }

  plot(grid, x1, y1);

  int dx = abs(x2 - x1), dy = abs(y2 - y1);

  dx > dy ?
    bresenham_line(grid, x1, y1, x2, y2, dx, dy, 0) :
    bresenham_line(grid, y1, x1, y2, x2, dy, dx, 1);
}

/*
 * Handler for the `POINT` operation.
 *
 * @param grid A pointer to a grid.
 * @param args [x, y, ..].
 */
void point(struct Grid *grid, int args[]) {
  int x = args[0], y = args[1];

  if (!grid->initialized) {
    printf("error: Grid isn't initialized\n");
    return;
  }

  plot(grid, x, y);
}

/*
 * Handler for the `RECTANGLE` operation.
 *
 * @param grid A pointer to a grid.
 * @param args [x1, y1, x2, y2].
 */
void rectangle(struct Grid *grid, int args[]) {
  int x1 = args[0], y1 = args[1], x2 = args[2], y2 = args[3];

  if (!grid->initialized) {
    printf("error: Grid isn't initialized\n");
    return;
  }

  line(grid, (int[]) { x1, y1, x1 + abs(x2 - x1), y1 });
  line(grid, (int[]) { x1 + abs(x2 - x1), y1, x2, y2 });
  line(grid, (int[]) { x2, y2, x1, y1 + abs(y2 - y1) });
  line(grid, (int[]) { x1, y1 + abs(y2 - y1), x1, y1 });
}

/*
 * The line parser responsible for turning lines read
 * from standard input into valid `Operation` structs.
 */
struct Parser {
  char line[LINE_MAX];
};

/*
 * Read a line in from standard input and set it on the
 * passed in parser.
 *
 * @param parser A pointer to a parser.
 */
void read(struct Parser *parser) {
  fgets(parser->line, LINE_MAX, stdin);
  parser->line[strcspn(parser->line, "\n")] = 0;
}

/*
 * Parse the current string that's set on the passed
 * in parser.
 *
 * It first splits on spaces `' '` and then handles each
 * individual token by splitting on commas `,`.
 *
 * @param parser A parser struct.
 * @return The parsed operation.
 */
struct Operation parse(struct Parser parser) {
  char *inner, *outer;

  struct Operation operation;

  char *token = strtok_r(parser.line, " ", &outer);

  operation.name = strdup(token);
  operation.cmd = command_from_string(token);

  int index = 0;

  for (;;) {
    token = strtok_r(NULL, " ", &outer);

    if (!token) break;

    char *local = strtok_r(token, ",", &inner);

    while (local) {
      operation.args[index++] = isdigit(*local) ?
        atoi(local) :
        (int)*local;
      local = strtok_r(NULL, ",", &inner);
    }
  }

  return operation;
}

/*
 * The struct responsible for evaluating operations
 * parsed by the parser.
 */
struct Interpreter {
  struct Grid grid;
  struct Operation op;
};

/*
 * Load an operation onto the passed in interpreter.
 *
 * @param i A pointer to an interpreter.
 * @param op An operation struct.
 */
void load(struct Interpreter *i, struct Operation op) {
  i->op = op;
}

/*
 * Evaluate the operation that's present on the passed in
 * interpreter.
 *
 * This method essentially associates commands with their
 * corresponding methods on `Grid`.
 *
 * @param i A pointer to an interpreter.
 */
void eval(struct Interpreter *i) {
  switch(i->op.cmd) {
    case CHAR:
      character(&i->grid, i->op.args);
      break;
    case CIRCLE:
      circle(&i->grid, i->op.args);
      break;
    case CLEAR:
      clear(&i->grid);
      break;
    case DISPLAY:
      display(i->grid);
      break;
    case END:
      exit(0);
    case GRID:
      grid(&i->grid, i->op.args);
      break;
    case INVALID:
      printf("error: Invalid command `%s`\n", i->op.name);
      break;
    case LINE:
      line(&i->grid, i->op.args);
      break;
    case POINT:
      point(&i->grid, i->op.args);
      break;
    case RECTANGLE:
      rectangle(&i->grid, i->op.args);
      break;
  }
}

/*
 * The program entrypoint.
 */
int main() {
  struct Parser parser;

  struct Interpreter interpreter = {
    .grid = {
      .character = '*',
      .initialized = 0
    }
  };

  for (;;) {
    // Display the prompt
    printf("> ");

    // Read a line in from stdin
    read(&parser);

    // Load the operation onto the interpreter
    load(&interpreter, parse(parser));

    // Evaluate the currently loaded operation
    eval(&interpreter);

  }
}
