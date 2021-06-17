# 2048 Temporal Difference Learning
 A 2048 agent with N-Tuple Network trained using Backward Temporal Coherence Learning.
 
## Benchmark (Intel® Core™ i5-8300H Processor)
1 ply search is the trained model without any tree search algorithm
 
5 ply is depth 2 expectimax search with the trained model for evaluation function

(Further training and testing needed)
 
[//]: # (| Depth | Games | Scores | % 16384 | % 8192 | % 4096 | Moves/s |)
[//]: # (|-------|-------|--------|---------|--------|--------|---------|)
[//]: # (| 1 ply | 10000 | 208000 | 47      | 84     | 95     | 700000  |)
[//]: # (| 5 ply | 1000  | 320000 | 93      | 100    | 100    | 3000    |)

Trained model detail:

 - 4 Tuples with size of 6 (256MB total)
 - Temporal Coherence Learning with β = 1.0
 - λ = 0.5

## Optimizations
 To achieve high speed and fast learning, both the agent and training code is heavily optimized:
 
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

If you change the network structure, you'll have to delete the trained model and train the AI from scratch.

```sh
make STRUCTURE=[Structure]
```

Available structures:
| Structure | Size    | Speed (Intel® Core™ i5-8300H Processor) |
|-----------|---------|-----------------------------------------|
| nw5x4     | 1.25 MB | 3.9 million moves/s                     |
| nw9x5     | 36 MB   | 1.5 million moves/s                     |
| nw4x6     | 256 MB  | 1.8 million moves/s                     |
| nw5x6     | 320 MB  | 1.2 million moves/s                     |

### Train model

```
./train
```
You can train the model from scratch or a trained model.
 
Training last 100000 games.
 
### Run agent
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
