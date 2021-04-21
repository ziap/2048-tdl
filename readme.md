# 2048 Temporal Difference Learning
 A 2048 agent with N-Tuple Network trained using Delayed Backward Temporal Difference Learning.
 
## Benchmark (Intel® Core™ i5-8300H Processor)
1 ply search is the trained model without any tree search algorithm
 
5 ply is depth 2 expectimax search with the trained model for evaluation function
 
| Depth | Games | Scores | % 16384 | % 8192 | % 4096 | Moves/s |
|-------|-------|--------|---------|--------|--------|---------|
| 1 ply | 10000 | 208000 | 47      | 84     | 95     | 700000  |
| 5 ply | 1000  | 320000 | 93      | 100    | 100    | 3000    |

Trained model detail:

 - 4 Tuple with size of 6 (256MB total)
 - α = 1
 - λ = 0.5 (5 step return)

## Optimizations
 To achieve high speed and fast learning, both the agent and training code is heavily optimized:
 
 - 64-bit bitboard representation.
 - Table lookup for movement and reward.
 - Transposistion table with Zobrist Hash.
 - Bit optimizations.
 - Fast fixed-size N-Tuple Network implementation.

## Usage

Trained model not available (yet).

### Build

```
make
```
### Train model

```
./2048
```
You can train the model from scratch or from a trained model.
 
The training process can be stopped anytime my pressing x, or after 10 billion moves which takes around 5.5 hours.
 
This model is trained and fine-tuned (lower the learning rate after score is saturated) many times to achieve the above result without automatic step-size adjusting or multi-stage learning.
 
### Run agent
Parameters:
 
 + **-d [Depth]** - The search depth (default: 0)
 + **-i [Iterations]** - Number of games to play (default: 1)
 + **-s** - Show the board when the AI is running **AI speed will be caped down to text rendering speed of your terminal**
  
Example:

```sh
./2048 -d2 -i100 # 5 ply, 100 games
./2048 -d4 -s    # 11 ply, 1 game, show board 	
```

A game with show board option enabled:

```
Tuple 012345 loaded from weights.bin
Tuple 456789 loaded from weights.bin
Tuple 012456 loaded from weights.bin
Tuple 45689A loaded from weights.bin
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
