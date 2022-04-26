# 2048 Temporal Difference Learning
 A 2048 agent with N-Tuple Network trained using Backward Temporal Coherence Learning.
 
## Benchmark (Intel® Core™ i5-8300H Processor)
1 ply search is the trained model without any tree search algorithm
 
5 ply is depth 2 expectimax search with the trained model for evaluation function

| Depth | Games | Scores | % 32768 | % 16384 | % 8192 | Moves/s |
|-------|-------|--------|---------|---------|--------|---------|
| 1 ply | 10000 | 169236 | 1.45    | 30.19   | 68.08  | 2349354 |
| 5 ply | 1000  | 390726 | 30.8    | 80.3    | 93.8   | 10982   |

You can achieve similar results with:

```sh
# Build the program with the 4x6 tuple network
make STRUCTURE=nw4x6

# Train the network with 400000 games, α = 1.0, λ = 0.5
./train -i 400 -a 1.0 -l 0.5 -w

# Run the agent for 10000 games with no search
./agent -i 10000

# Run the agent for 1000 games with 5 ply search
./agent -i 1000 -d 2
```

## Optimizations

 To achieve high speed and fast learning, both the agent and training code are heavily optimized:
 
 - 64-bit bitboard representation.
 - Table lookup for movement and reward.
 - Transposistion table with Zobrist Hash.
 - Bit optimizations.
 - Efficient N-Tuple Network implementation with static structure.
 
## Usage

Download and unzip the trained model (4x6tuple network) [here](../../releases/latest).

### Build

You can specify a N-tuple network structure when building the AI. The default structure is 4x6 tuples.

```sh
make STRUCTURE=[Structure]
```

Available structures:
| Structure | Size    | Speed          |
|-----------|---------|----------------|
| nw5x4     | 1.25 MB | 5.1E+6 moves/s |
| nw4x5     | 16 MB   | 4.3E+6 moves/s |
| nw6x5     | 24 MB   | 3.2E+6 moves/s |
| nw4x6     | 256 MB  | 2.3E+6 moves/s |
| nw5x6     | 320 MB  | 1.8E+6 moves/s |
| nw8x6     | 512 MB  | 1.2E+6 moves/s |

### Train model

```
./train
```
Parameters:
 + **-a [Rate]** - The learning rate (default: 1.0)
 + **-l [Lambda]** - The trace decay parameter (default: 0.5)
 + **-i [Games]** - Number of training games * 1000 (default: 1)
 + **-r** - Enable reading from a binary file
 + **-w** - Enable writing to a binary file
 + **-t** - Enable restart strategy
 
### Run agent

```sh
./agent
```
Parameters:
 
 + **-d [Depth]** - The search depth (default: 0)
 + **-i [Iterations]** - Number of games to play (default: 1)
 + **-s** - Show the board when the AI is running **AI speed will be capped down to the text rendering speed of your terminal**
  
Example:

```sh
./agent -d2 -i100 # 5 ply, 100 games
./agent -d4 -s    # 11 ply, 1 game, show board 	
```

A game with show board option enabled:

```
seed = 2081095579       depth = 2
+-------+-------+-------+-------+
| 32768 |  1024 |     4 |     2 |
+-------+-------+-------+-------+
|  2048 |   256 |    16 |     4 |
+-------+-------+-------+-------+
|  4096 |   128 |    32 |     2 |
+-------+-------+-------+-------+
|  8192 |    64 |     2 |     8 |
+-------+-------+-------+-------+
progress: 1/1
average score: 626268.00
average speed: 4477.88 moves per second
        32768   100.00%
```

# License

 This app is licensed under the MIT license.
