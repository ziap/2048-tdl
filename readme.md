# 2048 Temporal Difference Learning
 A 2048 agent with N-Tuple Network trained using Backward Temporal Coherence Learning.
 
## Benchmark (Intel® Core™ i5-8300H Processor)
1 ply search is the trained model without any tree search algorithm
 
5 ply is depth 2 expectimax search with the trained model for evaluation function

**Need further traing and testing**

[//]: # (| Depth | Games | Scores | % 16384 | % 8192 | % 4096 | Moves/s |)
[//]: # (|-------|-------|--------|---------|--------|--------|---------|)
[//]: # (| 1 ply | 10000 | 221758 | 51.68   | 86.94  | 96.26  | 1737604 |)
[//]: # (| 5 ply | 1000  | 322862 | 91.7    | 99.3   | 99.8   | 7301    |)

4 games with 5 ply search reached 32768

Trained model detail:

 - 4 Tuples with size of 6 (256MB total)
 - Temporal Coherence Learning with β = 1.0
 - λ = 0.5

## Optimizations
 To achieve high speed and fast learning, both the agent and training code are heavily optimized:
 
 - 64-bit bitboard representation.
 - Table lookup for movement and reward.
 - Transposistion table with Zobrist Hash.
 - Bit optimizations.
 - Efficient N-Tuple Network implementation.
 - Template based isomorphic pattern structure.

## Usage

Download and unzip the trained model (4x6tuple network) [here](../../releases/latest).

### Build

You can specify a N-tuple network structure when building the AI. The default structure is 4x6 tuples.

```sh
make STRUCTURE=[Structure]
```

Available structures:
| Structure | Size    |
|-----------|---------|
| nw5x4     | 1.25 MB |
| nw4x5     | 16 MB   |
| nw6x5     | 24 MB   |
| nw4x6     | 256 MB  |
| nw5x6     | 320 MB  |

### Train model

```
./train
```
Parameters:
 + **-a [Rate]** - The learning rate (default: 1.0)
 + **-l [Lambda]** - The trace decay parameter (default: 0.5)
 + **-i [Games]** - Number of training games * 100k (default: 1)
 + **-r** - Enable reading from a binary file
 + **-w** - Enable writing to a binary file
 
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
seed = 2899209538       depth = 4
+-------+-------+-------+-------+
|   128 |    64 |     4 |     2 |
+-------+-------+-------+-------+
|   512 |    16 |     8 |       |
+-------+-------+-------+-------+
|  1024 |     8 |       |       |
+-------+-------+-------+-------+
|  2048 |     2 |     2 |       |
+-------+-------+-------+-------+
```

# License
 This app is licensed under the MIT license.
