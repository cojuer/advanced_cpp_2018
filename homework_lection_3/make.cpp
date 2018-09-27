#include <iostream>
#include <ctime>
#include <vector>
#include <functional>
#include <cassert>
#include <algorithm>

struct TItem {
	int value;
	time_t timestamp;
	TItem(int v)
		: value(v)
		, timestamp(std::time(nullptr)) {}
};
using Items = std::vector<TItem>;

template <int32_t... elements >
Items MakeItemsSimple()
{
	return std::vector<TItem>{ TItem{elements}... };
}

std::function<bool (int32_t)> MakePredicate(const Items& items)
{
	return [items](int32_t elem)
	{
		return std::any_of(
			items.begin(), items.end(), 
			[elem](auto x) { return x.value == elem; });
	};
}

int main() {
	Items items = MakeItemsSimple<0, 1, 4, 5, 6>();
	Items newItems = MakeItemsSimple<7, 15, 1>();
	auto isFound = MakePredicate(items);
	auto isFoundNew = MakePredicate(newItems);
	assert(isFound(0) == true);
	assert(isFound(7) == false);
	assert(isFoundNew(7) == true);
	assert(isFoundNew(6) == false);
}
