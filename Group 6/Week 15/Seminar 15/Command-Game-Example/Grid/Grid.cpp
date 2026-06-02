#include "Grid.h"

#include <stdexcept>

Grid::Grid(int n) {
	if (n < MIN_SIZE || n > MAX_SIZE) {
		throw std::invalid_argument("Map size must be between 4x4 and 9x9");
	}

	grid = std::move(std::vector<std::vector<bool>>(n, std::vector<bool>(n, false)));
}

bool Grid::isInside(int x, int y) const {
	return x >= 0 && x < getSize() && y >= 0 && y < getSize();
}

void Grid::mark(int x, int y) {
	if (!isInside(x, y)) {
		gridThrowOutside();
	}

	grid[x][y] = true;
}

void Grid::unmark(int x, int y) {
	if (!isInside(x, y)) {
		gridThrowOutside();
	}

	grid[x][y] = false;
}

bool Grid::isMarked(int x, int y) const {
	return grid[x][y];
}

int Grid::getSize() const {
	return grid.size();
}

void Grid::gridThrowOutside() {
	throw std::out_of_range("Cannot go outside the grid.");
}
