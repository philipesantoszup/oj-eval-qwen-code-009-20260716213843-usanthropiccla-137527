#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"

#include <climits>
#include <cstddef>

namespace sjtu
{
/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */
template<typename T>
class vector
{
private:
	T *data_;
	size_t size_;
	size_t capacity_;

	static T *allocate(size_t n)
	{
		if (n == 0) return nullptr;
		return static_cast<T *>(::operator new(n * sizeof(T)));
	}

	static void deallocate(T *p)
	{
		::operator delete(p);
	}

	void destroy_all()
	{
		for (size_t i = 0; i < size_; ++i) {
			data_[i].~T();
		}
	}

	void reserve(size_t new_cap)
	{
		if (new_cap <= capacity_) return;
		T *new_data = allocate(new_cap);
		size_t i = 0;
		try {
			for (; i < size_; ++i) {
				new (new_data + i) T(data_[i]);
			}
		} catch (...) {
			for (size_t j = 0; j < i; ++j) new_data[j].~T();
			deallocate(new_data);
			throw;
		}
		for (size_t j = 0; j < size_; ++j) data_[j].~T();
		deallocate(data_);
		data_ = new_data;
		capacity_ = new_cap;
	}

public:
	/**
	 * TODO
	 * a type for actions of the elements of a vector, and you should write
	 *   a class named const_iterator with same interfaces.
	 */
	/**
	 * you can see RandomAccessIterator at CppReference for help.
	 */
	class const_iterator;
	class iterator
	{
	public:
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = T*;
		using reference = T&;
		using iterator_category = std::output_iterator_tag;

	private:
		vector *container_;
		size_t index_;

	public:
		iterator() : container_(nullptr), index_(0) {}
		iterator(vector *container, size_t index) : container_(container), index_(index) {}

		iterator operator+(const int &n) const
		{
			return iterator(container_, index_ + n);
		}
		iterator operator-(const int &n) const
		{
			return iterator(container_, index_ - n);
		}
		// return the distance between two iterators,
		// if these two iterators point to different vectors, throw invaild_iterator.
		int operator-(const iterator &rhs) const
		{
			if (container_ != rhs.container_) throw invalid_iterator();
			return static_cast<int>(index_) - static_cast<int>(rhs.index_);
		}
		iterator& operator+=(const int &n)
		{
			index_ += n;
			return *this;
		}
		iterator& operator-=(const int &n)
		{
			index_ -= n;
			return *this;
		}
		iterator operator++(int)
		{
			iterator tmp = *this;
			++index_;
			return tmp;
		}
		iterator& operator++()
		{
			++index_;
			return *this;
		}
		iterator operator--(int)
		{
			iterator tmp = *this;
			--index_;
			return tmp;
		}
		iterator& operator--()
		{
			--index_;
			return *this;
		}
		T& operator*() const
		{
			return container_->data_[index_];
		}
		T* operator->() const
		{
			return &(container_->data_[index_]);
		}
		bool operator==(const iterator &rhs) const
		{
			return container_ == rhs.container_ && index_ == rhs.index_;
		}
		bool operator==(const const_iterator &rhs) const
		{
			return container_ == rhs.container_ && index_ == rhs.index_;
		}
		bool operator!=(const iterator &rhs) const
		{
			return !(*this == rhs);
		}
		bool operator!=(const const_iterator &rhs) const
		{
			return !(*this == rhs);
		}

		friend class vector;
		friend class const_iterator;
	};

	class const_iterator
	{
	public:
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = T*;
		using reference = T&;
		using iterator_category = std::output_iterator_tag;

	private:
		const vector *container_;
		size_t index_;

	public:
		const_iterator() : container_(nullptr), index_(0) {}
		const_iterator(const vector *container, size_t index) : container_(container), index_(index) {}

		const_iterator operator+(const int &n) const
		{
			return const_iterator(container_, index_ + n);
		}
		const_iterator operator-(const int &n) const
		{
			return const_iterator(container_, index_ - n);
		}
		int operator-(const const_iterator &rhs) const
		{
			if (container_ != rhs.container_) throw invalid_iterator();
			return static_cast<int>(index_) - static_cast<int>(rhs.index_);
		}
		const_iterator& operator+=(const int &n)
		{
			index_ += n;
			return *this;
		}
		const_iterator& operator-=(const int &n)
		{
			index_ -= n;
			return *this;
		}
		const_iterator operator++(int)
		{
			const_iterator tmp = *this;
			++index_;
			return tmp;
		}
		const_iterator& operator++()
		{
			++index_;
			return *this;
		}
		const_iterator operator--(int)
		{
			const_iterator tmp = *this;
			--index_;
			return tmp;
		}
		const_iterator& operator--()
		{
			--index_;
			return *this;
		}
		const T& operator*() const
		{
			return container_->data_[index_];
		}
		const T* operator->() const
		{
			return &(container_->data_[index_]);
		}
		bool operator==(const iterator &rhs) const
		{
			return container_ == rhs.container_ && index_ == rhs.index_;
		}
		bool operator==(const const_iterator &rhs) const
		{
			return container_ == rhs.container_ && index_ == rhs.index_;
		}
		bool operator!=(const iterator &rhs) const
		{
			return !(*this == rhs);
		}
		bool operator!=(const const_iterator &rhs) const
		{
			return !(*this == rhs);
		}

		friend class vector;
		friend class iterator;
	};

	vector() : data_(nullptr), size_(0), capacity_(0) {}

	vector(const vector &other) : data_(nullptr), size_(0), capacity_(0)
	{
		if (other.size_ > 0) {
			data_ = allocate(other.capacity_);
			capacity_ = other.capacity_;
			for (size_t i = 0; i < other.size_; ++i) {
				new (data_ + i) T(other.data_[i]);
				++size_;
			}
		}
	}

	~vector()
	{
		destroy_all();
		deallocate(data_);
		data_ = nullptr;
		size_ = 0;
		capacity_ = 0;
	}

	vector &operator=(const vector &other)
	{
		if (this == &other) return *this;
		destroy_all();
		deallocate(data_);
		data_ = nullptr;
		size_ = 0;
		capacity_ = 0;
		if (other.size_ > 0) {
			data_ = allocate(other.capacity_);
			capacity_ = other.capacity_;
			for (size_t i = 0; i < other.size_; ++i) {
				new (data_ + i) T(other.data_[i]);
				++size_;
			}
		}
		return *this;
	}

	T & at(const size_t &pos)
	{
		if (pos >= size_) throw index_out_of_bound();
		return data_[pos];
	}
	const T & at(const size_t &pos) const
	{
		if (pos >= size_) throw index_out_of_bound();
		return data_[pos];
	}

	T & operator[](const size_t &pos)
	{
		if (pos >= size_) throw index_out_of_bound();
		return data_[pos];
	}
	const T & operator[](const size_t &pos) const
	{
		if (pos >= size_) throw index_out_of_bound();
		return data_[pos];
	}

	const T & front() const
	{
		if (size_ == 0) throw container_is_empty();
		return data_[0];
	}
	const T & back() const
	{
		if (size_ == 0) throw container_is_empty();
		return data_[size_ - 1];
	}

	iterator begin() { return iterator(this, 0); }
	const_iterator begin() const { return const_iterator(this, 0); }
	const_iterator cbegin() const { return const_iterator(this, 0); }

	iterator end() { return iterator(this, size_); }
	const_iterator end() const { return const_iterator(this, size_); }
	const_iterator cend() const { return const_iterator(this, size_); }

	bool empty() const { return size_ == 0; }

	size_t size() const { return size_; }

	void clear()
	{
		destroy_all();
		deallocate(data_);
		data_ = nullptr;
		size_ = 0;
		capacity_ = 0;
	}

	iterator insert(iterator pos, const T &value)
	{
		size_t ind = pos.index_;
		return insert(ind, value);
	}

	iterator insert(const size_t &ind, const T &value)
	{
		if (ind > size_) throw index_out_of_bound();
		if (size_ == capacity_) {
			size_t new_cap = capacity_ == 0 ? 1 : capacity_ * 2;
			reserve(new_cap);
		}
		// construct new last element from previous last
		if (size_ > 0) {
			new (data_ + size_) T(data_[size_ - 1]);
			for (size_t i = size_ - 1; i > ind; --i) {
				data_[i] = data_[i - 1];
			}
			data_[ind] = value;
		} else {
			new (data_ + 0) T(value);
		}
		++size_;
		return iterator(this, ind);
	}

	iterator erase(iterator pos)
	{
		size_t ind = pos.index_;
		return erase(ind);
	}

	iterator erase(const size_t &ind)
	{
		if (ind >= size_) throw index_out_of_bound();
		for (size_t i = ind; i + 1 < size_; ++i) {
			data_[i] = data_[i + 1];
		}
		data_[size_ - 1].~T();
		--size_;
		return iterator(this, ind);
	}

	void push_back(const T &value)
	{
		if (size_ == capacity_) {
			size_t new_cap = capacity_ == 0 ? 1 : capacity_ * 2;
			reserve(new_cap);
		}
		new (data_ + size_) T(value);
		++size_;
	}

	void pop_back()
	{
		if (size_ == 0) throw container_is_empty();
		data_[size_ - 1].~T();
		--size_;
	}
};


}

#endif
