#ifndef LEARNING_H
#define LEARNING_H

#include <numeric>

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

		// Printing the stat of the network
		static void Print() {
			std::cout << T::name << " size = " << T::len;
			size_t usage = sizeof(T::weights);
			if (usage >= (1 << 30)) {
				std::cout << " (" << (usage >> 30) << "GB)";
			}
			else if (usage >= (1 << 20)) {
				std::cout << " (" << (usage >> 20) << "MB)";
			}
			else if (usage >= (1 << 10)) {
				std::cout << " (" << (usage >> 10) << "KB)";
			}
			std::cout << '\n';
		};
	};
	template<class T, class...Targs>
	struct TupleNetwork<T, Targs...> {
		static void Print() {
			TupleNetwork<T>::Print();
			TupleNetwork<Targs...>::Print();
		}
		static float Estimate(board_t b) {
			return TupleNetwork<T>::Estimate(b) + TupleNetwork<Targs...>::Estimate(b);
		}
		static float Update(board_t b, float u) {
			return TupleNetwork<T>::Update(b, u) + TupleNetwork<Targs...>::Update(b, u);
		}
	};

	float alpha = 0.1f;

	float Estimate(board_t b) {
		return TupleNetwork<Features...>::Estimate(b);
	}

	float Update(board_t b, float u) {
		u /= sizeof...(Features);
		return TupleNetwork<Features...>::Update(b, u);
	}

	std::vector<int> scores;
	std::vector<int> max_tile;
public:

	std::vector<State> path;

	Learning(float a = 0.1f) {
		TupleNetwork<Features...>::Print();
		alpha = a;
		std::cout << "alpha = " << alpha << '\n';
	}

	State SelectBestMove(board_t b) {
		State best(0, 0);
		best.action = -1;
		best.value = -(1 << 22);
		for (int i = 0; i < 4; i++) {
			State after(b, i);
			if (after.isValid()) {
				after.value = after.reward + Estimate(after.after);
				if (after.value > best.value) best = after;
			}
		}
		return best;
	}

	void UpdateEpisode() {
		float exact = 0;
		for (path.pop_back(); path.size(); path.pop_back()) {
			State move = path.back();
			float error = exact - (move.value - move.reward);
			exact = move.reward + Update(move.after, alpha * error);
		}
	}

	void MakeStat(int n, board_t b, int score) {
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
