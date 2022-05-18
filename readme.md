# 2048 Temporal Difference Learning

A 2048 agent with N-Tuple Network trained using Backward Temporal Coherence Learning.

## Benchmark (Intel® Core™ i5-8300H Processor)

1 ply search is the trained model without any tree search algorithm

5 ply is depth 2 expectimax search with the trained model for evaluation function

| Depth | Games | Scores | % 32768 | % 16384 | % 8192 | Moves/s |
| ----- | ----- | ------ | ------- | ------- | ------ | ------- |
| 1 ply | 10000 | 254275 | 7.99    | 54.29   | 84.47  | 1405922 |
| 5 ply | 300   | 420477 | 37.33   | 83.67   | 96.67  | 8246    |

You can achieve similar results with:

```sh
# Build the program with the 8x6 tuple network
make STRUCTURE=nw8x6

# Train the network with 400000 episodes, α = 1.0, λ = 0.5 and restart strategy
./train -e 400 -a 1.0 -l 0.5 -o -r

# Run the agent for 10000 games with no search
./agent -e 10000

# Run the agent for 300 games with 5 ply search
./agent -e 300 -d 2
```

**Note:** Because of overhead, using N cores doesn't make the program N times faster, especially on weaker CPUs like the one in the benchmark.
So it's generally not recommended to use all the cores available.
For example on the CPU used in the benchmark (4 cores, 8 threads), the speed of using different number of threads is:

| Depth | Threads | Speedup   |
| ----- | ------- | --------- |
| 1 ply | 1       | +0%       |
| 1 ply | 2       | +16%      |
| 1 ply | 4       | **+19%**  |
| 1 ply | 8       | +10%      |
| - - - | - - -   | - - -     |
| 5 ply | 1       | +0%       |
| 5 ply | 2       | +69%      |
| 5 ply | 4       | **+187%** |
| 5 ply | 8       | +156%     |

## Features

- Backward TD(λ) learning.
- N-Tuple Network with configurable structure.
- Optional temporal coherence learning and restart strategy.
- Expectimax search with configurable depth.
- [Webview](https://github.com/webview/webview) based GUI application.
- Multi-threaded training and evaluation.

To achieve high speed and fast learning, both the agent and training code are heavily optimized:

- 64-bit bitboard representation.
- Table lookup for movement and reward.
- Transposistion table with Zobrist Hash.
- Bit optimizations.
- Efficient N-Tuple Network implementation with static structure.

## Usage

Download and unzip the trained model (8x6tuple network) [here](../../releases/latest).

### Build

GCC and GNU Make are required to build the program.
To build the GUI, you also need the GTK+ development libraries and webkit2gtk-4.0.

Currently, the makefile only supports Linux. Building the program on other platforms should be possible, but not tested.
Cross platform build script is planned.

Building the program on Google Colaboratory is also supported if you disable the GUI.

Make sure to clone the repository recursively or initalize the submodules. (only required if you are using the GUI)

#### Build steps

```sh
make [Options]
```

Parameters:

- **STRUCTURE**: The tuple network structure. (default: nw4x6)
- **ENABLE_GUI**: Enable the GUI. (default: true)
- **EXTRAS**: Extra compiler options for profiling, etc.

Available structures:

| Structure | Size    | Speed          |
| --------- | ------- | -------------- |
| nw5x4     | 1.25 MB | 5.1E+6 moves/s |
| nw4x5     | 16 MB   | 4.3E+6 moves/s |
| nw6x5     | 24 MB   | 3.2E+6 moves/s |
| nw4x6     | 256 MB  | 2.3E+6 moves/s |
| nw5x6     | 320 MB  | 1.8E+6 moves/s |
| nw8x6     | 512 MB  | 1.4E+6 moves/s |

### Train model

```sh
./2048 train [Options]
```

Parameters:

```
  -a <alpha>    -- Set the learning rate
                   default: 0.1
  -l <lambda>   -- Set the trace decay
                   default: 0.5
  -e <episodes> -- Set the number of training games * 1000
                   default: 1
  -t <threads>  -- Set the number of threads
                   default: 1 (0 uses all threads)
  -i            -- Enable reading from a binary file
  -o            -- Enable writing to a binary file
  -c            -- Enable temporal coherence learning
  -r            -- Enable restart strategy  
```

### Run agent

```sh
./2048 agent [Options]
```

Parameters:

```
  -d <depth>    -- Set the search depth
                   default: 0
  -e <episodes> -- Set the number of games to play
                   default: 1
  -t <threads>  -- Set the number of threads
                   default: 1 (0 uses all threads)
  -g            -- Enable the GUI
  -h            -- Show this message                        
  -r            -- Enable restart strategy
```

Example:

```sh
./agent -d2 -i100 -t 0 # 5 ply, 100 games, multi-threaded
./agent -d4 -g         # 11 ply, enable GUI
```

Example game with the GUI:

![](gui.png)

## Todo

- [ ] Add multi-stage learning
- [ ] Add more settings to the GUI application

# License

This app is licensed under the MIT license.
