# Falling Sand

Falling-sand cellular simulation, in C.

A small, self-contained demo written in **pure C** — no external libraries,
just the standard library and POSIX. Part of the [Corg-Labs](https://github.com/Corg-Labs)
collection of single-file C programs.

---

## How It Works

1. An emitter drops grains from a wandering point at the top
2. Each grain falls straight down if the cell below is empty
3. Otherwise it slides diagonally left or right at random
4. The pile builds up from the bottom over time

---

# Tutorial

This tutorial walks through every stage of `sand.c` — a single-file, ~80-line
C program — so you understand exactly how a falling-sand simulation works from
grid storage to terminal rendering.

## 1. The Grid and Its Dimensions

The entire world is a 2-D array of `int`, where `0` means empty and `1` means a
grain of sand occupies that cell.

```c
#define W 80
#define H 40

static int g[H][W];
memset(g, 0, sizeof(g));
```

`W` and `H` are the terminal width and height the simulation targets. Because
the array is declared `static` inside `main`, it lives in BSS and is guaranteed
to be zero-initialised — the `memset` makes the intent explicit.

## 2. The Wandering Emitter

A single integer `emit` tracks the horizontal column from which new grains are
dropped each frame.

```c
int emit = W/2;
...
g[0][emit] = 1;
if (frame % 3 == 0) {
    emit += (rand() % 3) - 1;
    if (emit < 2)   emit = 2;
    if (emit > W-3) emit = W-3;
}
```

Every frame, a grain is placed in row 0 at column `emit`. Every third frame the
emitter drifts one step left, stays, or moves one step right with equal
probability. The clamp (`< 2`, `> W-3`) keeps the emitter away from the edges
so diagonal slides never go out-of-bounds.

## 3. The Physics Update Loop

Grains are processed **bottom-up** (highest row index first) so that a grain
that just fell does not get processed a second time in the same frame.

```c
for (int y = H-2; y >= 0; y--)
    for (int x = 0; x < W; x++)
        if (g[y][x]) {
            if (!g[y+1][x]) {
                g[y+1][x] = 1; g[y][x] = 0;          /* fall straight down */
            } else {
                int l = (x > 0   && !g[y+1][x-1]);
                int r = (x < W-1 && !g[y+1][x+1]);
                if (l && r) {
                    if (rand() & 1) { g[y+1][x-1] = 1; }
                    else            { g[y+1][x+1] = 1; }
                    g[y][x] = 0;
                } else if (l) { g[y+1][x-1] = 1; g[y][x] = 0; }
                else if (r)   { g[y+1][x+1] = 1; g[y][x] = 0; }
                /* if neither l nor r, grain is fully buried — stays put */
            }
        }
```

The three-rule priority is: fall straight, then slide to a free diagonal (with
a coin-flip when both are free), otherwise rest in place.

## 4. Terminal Rendering with ANSI Escape Codes

Before the main loop the cursor is hidden and the screen is cleared once:

```c
printf("\033[2J\033[?25l");
```

Each frame the cursor is moved back to the home position (`\033[H`) rather than
clearing the screen again, which avoids flicker:

```c
printf("\033[H");
for (int y = 0; y < H; y++) {
    for (int x = 0; x < W; x++)
        putchar(g[y][x] ? 'o' : ' ');
    putchar('\n');
}
fflush(stdout);
```

Each occupied cell is printed as `'o'`; empty cells are spaces. `fflush` ensures
the full frame reaches the terminal before the program sleeps.

## 5. Frame Timing and the Main Loop

After rendering, the program sleeps for 25 ms, giving roughly 40 frames per
second:

```c
usleep(25000);
frame++;
```

`frame` is the only piece of persistent loop state besides the grid and the
emitter position. It is used solely for the every-third-frame emitter drift.

## 6. Graceful Shutdown via Signal Handling

A `volatile` flag lets the SIGINT handler (Ctrl-C) communicate with the main
loop without undefined behaviour:

```c
static volatile int running = 1;
static void stop(int s) { (void)s; running = 0; }
...
signal(SIGINT, stop);
```

When `running` becomes `0` the `while (running)` loop exits and the cleanup
sequence restores the terminal — re-enables the cursor, clears the screen, and
resets colours:

```c
printf("\033[0m\033[?25h\033[2J\033[H");
```

---

## Build

```
gcc sand.c -o sand
```

## Run

```
./sand
```

## Controls

Press **Ctrl-C** to quit.
