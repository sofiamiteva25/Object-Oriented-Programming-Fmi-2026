#pragma once
#include <vector>

class Grid {
	std::vector<std::vector<bool>> grid;

	static constexpr int MIN_SIZE = 4;
	static constexpr int MAX_SIZE = 9;

public:
	Grid(int n);

	bool isInside(int x, int y) const;

	void mark(int x, int y);
	void unmark(int x, int y);

	bool isMarked(int x, int y) const;

	int getSize() const;

	static void gridThrowOutside();

	static constexpr char MARKED_SYMBOL = 'X';
	static constexpr char UNMARKED_SYMBOL = '-';
};
