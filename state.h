#ifndef STATE_H
#define STATE_H

class State {
public:
	board_t before;
	board_t after;
	float reward;
	int action;
	float value = 0;
	State(board_t b, int dir) {
		action = dir;
		before = b;
		if (dir == -1) {
			after = b;
			reward = 0;
		}
		else {
			after = move(b, dir);
			reward = move.Score(b, dir);
		}
	}
	bool isValid() { return before != after; }
	void operator=(State s) {
		action = s.action;
		before = s.before;
		after = s.after;
		value = s.value;
		reward = s.reward;
	}
};

#endif // STATE_H