#ifndef LEARNING_H
#define LEARNING_H

#include "pattern.h"
#include "state.h"
#include <numeric>
#include <unordered_map>

template<class...Features>
class Learning {
private:

	// The tuple network
	template <class...Targs> struct TupleNetwork;
	template<class T>
	struct TupleNetwork<T> {

		// Estimate the value of a board
		static float Estimate(board_t b) {
			return T::Estimate(b);
		}

		// Update a given board and return it's updated value
		static float Update(board_t b, float u) {
			return T::Update(b, u);
		}

		// Save the model to a binary file
		static void Save(std::ostream& out, std::string path) {
			T::Save(out, path);
		}

		// Load the model from a binary file
		static void Load(std::istream& in, std::string path) {
			T::Load(in, path);
		}
	};
	template<class T, class...Targs>
	struct TupleNetwork<T, Targs...> {
		static float Estimate(board_t b) {
			return TupleNetwork<T>::Estimate(b) + TupleNetwork<Targs...>::Estimate(b);
		}
		static float Update(board_t b, float u) {
			return TupleNetwork<T>::Update(b, u) + TupleNetwork<Targs...>::Update(b, u);
		}
		static void Save(std::ostream& out, std::string path) {
			TupleNetwork<T>::Save(out, path);
			TupleNetwork<Targs...>::Save(out, path);
		}
		static void Load(std::istream& in, std::string path) {
			TupleNetwork<T>::Load(in, path);
			TupleNetwork<Targs...>::Load(in, path);
		}
	};

	float alpha = 0.1f;

	float lambda = 0.0f;

	std::vector<int> scores;
	std::vector<int> max_tile;
public:

	float Estimate(board_t b) {
		return TupleNetwork<Features...>::Estimate(b);
	}

	float Update(board_t b, float u) {
		u /= float(sizeof...(Features));
		return TupleNetwork<Features...>::Update(b, u);
	}

	void Save(std::string file_name) {
		std::ofstream fout(file_name.c_str(), std::ios::out | std::ios::binary);
		TupleNetwork<Features...>::Save(fout, file_name);
	}

	void Load(std::string file_name) {
		std::ifstream fin(file_name.c_str(), std::ios::out | std::ios::binary);
		if (fin.is_open()) TupleNetwork<Features...>::Load(fin, file_name);
	}

	std::vector<State> path;

	Learning(float a = 0.1f, float l = 0.0f) {
		alpha = a;
		lambda = l;
	}

	State SelectBestMove(board_t b) {
		State best(b, -1);
		for (int i = 0; i < 4; i++) {
			State after(b, i);
			if (after.isValid()) {
				after.value = after.reward + Estimate(after.after);
				if (!best.isValid() || after.value > best.value) best = after;
			}
		}
		return best;
	}

	void BackwardLearning() {
		std::unordered_map<board_t, float> delta;
		float exact = 0;
		for (path.pop_back();path.size(); path.pop_back()) {
			State move = path.back();
			float error = exact - (move.value - move.reward);
			float curr_lambda = 1;
			for (std::vector<State>::reverse_iterator i = path.rbegin(); i != path.rend() && i != path.rbegin() + 5; i++) {
				delta[i->after] += curr_lambda * error; // Delayed update
				curr_lambda *= lambda;
			}
			exact = move.reward + Update(move.after, alpha * delta[move.after]);
			delta.erase(delta.find(move.after));
		}
	}

	void MakeStat(int n, board_t b, int score, double progress) {
		scores.push_back(score);
		max_tile.push_back(MaxRank(b));
		if (n % 1000 == 0) {
			int sum = 0;
			int max = 0;
			for (int i : scores) {
				sum += i;
				max = std::max(max, i);
			}
			int stat[16] = { 0 };
			for (int i : max_tile) stat[i]++;
			float mean = float(sum) / 1000;
			std::cout << n << "\tmean = " << mean;
			std::cout << "\tmax = " << max;
			std::cout << "\tprogress: " << progress * 100 << '%';
			std::cout << '\n';
			float accu = 0.0f;
			for (int i = 0xf; i > 0; i--) {
				if (stat[i]) {
					accu += float(stat[i]);
					std::cout << '\t' << (1 << i) << '\t' << accu * 0.1f << "%\t" << float(stat[i]) * 0.1f << "%\n";
				}
			}
			scores.clear();
			max_tile.clear();
		}
	}
};

#endif // LEARNING_H
