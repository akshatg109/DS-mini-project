# Roulette Casino Mini Project (C + Raylib)

This is a university mini project that demonstrates basic data structure concepts through a roulette casino game.

## Data Structures Used

- **Queue** (`src/ds_queue.c`): stores game event messages in order
- **Stack** (`src/ds_stack.c`): supports undo of the last betting action
- **Linked List** (`src/ds_list.c`): stores round-by-round history records

## Game Rules Implemented

- Single number bet (`0-36`) -> payout `35:1`
- Red/Black bet -> payout `1:1`
- Odd/Even bet -> payout `1:1`
- `0` is treated as green (odd/even lose on `0`)

## Build and Run

```bash
make
make run
```

## Other Commands

```bash
make debug
make clean
```

## Dependencies

- C compiler (`gcc`)
- [raylib](https://www.raylib.com/)
- `pkg-config` (optional, but recommended for auto-detecting raylib flags)

If `pkg-config` cannot find raylib, the `Makefile` falls back to common Linux link flags.
