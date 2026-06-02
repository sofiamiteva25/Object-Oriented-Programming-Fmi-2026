#pragma once

#include <vector>
#include <functional>
#include <optional>

using namespace std;

template<class T>
class Pool {
protected:
	vector<T> data;

public:
	void add(const T& el);
	void remove(function<bool(const T&)> pred);

	vector<T> filter(function<bool(const T&)> pred) const;
	optional<T> findFirst(function<bool(const T&)> pred) const;

	int size() const;
	bool isEmpty() const;

};

template<class T>
inline void Pool<T>::add(const T& el)
{
	data.push_back(el);
}

template<class T>
inline void Pool<T>::remove(function<bool(const T&)> pred)
{
	erase_if(data, pred);
}

template<class T>
inline vector<T> Pool<T>::filter(function<bool(const T&)> pred) const
{
	vector<T> res;

	for (const auto& el : data) {
		if (pred(el)) {
			res.push_back(el);
		}
	}

	return res;
}

template<class T>
inline optional<T> Pool<T>::findFirst(function<bool(const T&)> pred) const
{
	for (const auto& el : data) {
		if (pred(el)) {
			return el;
		}
	}

	return nullopt;
}

template<class T>
inline int Pool<T>::size() const
{
	return data.size();
}

template<class T>
inline bool Pool<T>::isEmpty() const
{
	return data.empty();
}
