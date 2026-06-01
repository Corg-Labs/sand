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
