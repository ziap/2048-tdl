#ifndef STATE_H
#define STATE_H

class State {
public:
	board_t before;
	board_t after;
	int reward;
	int action;
	float value = 0;
	State(board_t b, int dir) {
		action = dir;
		before = b;
		after = move(b, dir);
		reward = move.Score(b, dir);
	}
	bool isValid() { return before != after; }
};

#endif // STATE_H