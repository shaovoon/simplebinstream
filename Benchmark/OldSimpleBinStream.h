// The MIT License (MIT)
// Simplistic Binary Streams 0.9
// Copyright (C) 2014 - 2016, by Wong Shao Voon (shaovoon@yahoo.com)
//
// http://opensource.org/licenses/MIT
//
// version 0.9.2   : Optimize mem_istream constructor for const char*
// version 0.9.3   : Optimize mem_ostream vector insert
// version 0.9.4   : New ptr_istream class
// version 0.9.5   : Add Endianness Swap with compile time check

#ifndef OldBinStream_H
#define OldBinStream_H

#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <stdexcept>
#include <iostream>
#include <stdint.h>

namespace old
{
	enum class Endian
	{
		Big,
		Little
	};
	using BigEndian = std::integral_constant<Endian, Endian::Big>;
	using LittleEndian = std::integral_constant<Endian, Endian::Little>;

	template<typename T>
	void swap_endian8(T& ui)
	{
		union EightBytes
		{
			T ui;
			uint8_t arr[8];
		};

		EightBytes fb;
		fb.ui = ui;
		// swap the endian
		std::swap(fb.arr[0], fb.arr[7]);
		std::swap(fb.arr[1], fb.arr[6]);
		std::swap(fb.arr[2], fb.arr[5]);
		std::swap(fb.arr[3], fb.arr[4]);

		ui = fb.ui;
	}

	template<typename T>
	void swap_endian4(T& ui)
	{
		union FourBytes
		{
			T ui;
			uint8_t arr[4];
		};

		FourBytes fb;
		fb.ui = ui;
		// swap the endian
		std::swap(fb.arr[0], fb.arr[3]);
		std::swap(fb.arr[1], fb.arr[2]);

		ui = fb.ui;
	}

	template<typename T>
	void swap_endian2(T& ui)
	{
		union TwoBytes
		{
			T ui;
			uint8_t arr[2];
		};

		TwoBytes fb;
		fb.ui = ui;
		// swap the endian
		std::swap(fb.arr[0], fb.arr[1]);

		ui = fb.ui;
	}

	template<typename T>
	void swap_if_integral(T& val, std::true_type)
	{
		switch (sizeof(T))
		{
		case 2u: swap_endian2(val); break;
		case 4u: swap_endian4(val); break;
		case 8u: swap_endian8(val); break;
		}
	}

	template<typename T>
	void swap_if_integral(T& val, std::false_type)
	{
		// T is not integral so do nothing
	}

	template<typename T>
	void swap(T& val, std::false_type)
	{
		std::is_integral<T> is_integral_type;
		swap_if_integral(val, is_integral_type);
	}

	template<typename T>
	void swap(T& val, std::true_type)
	{
		// same endian so do nothing.
	}

template<typename same_endian_type>
class file_istream
{
public:
	file_istream() {}
	file_istream(const char * file) 
	{
		open(file);
	}
	void open(const char * file)
	{
		m_istm.open(file, std::ios_base::in | std::ios_base::binary);
	}
	void close()
	{
		m_istm.close();
	}
	bool is_open()
	{
		return m_istm.is_open();
	}
	bool eof() const
	{
		return m_istm.eof();
	}
	std::ifstream::pos_type tellg()
	{
		return m_istm.tellg();
	}
	void seekg (std::streampos pos)
	{
		m_istm.seekg(pos);
	}
	void seekg (std::streamoff offset, std::ios_base::seekdir way)
	{
		m_istm.seekg(offset, way);
	}

	template<typename T>
	void read(T& t)
	{
		if(m_istm.read(reinterpret_cast<char*>(&t), sizeof(T)).bad())
		{
			throw std::runtime_error("Read Error!");
		}
		old::swap(t, m_same_type);
	}
	void read(typename std::vector<char>& vec)
	{
		if (m_istm.read(reinterpret_cast<char*>(&vec[0]), vec.size()).bad())
		{
			throw std::runtime_error("Read Error!");
		}
	}
	void read(char* p, size_t size)
	{
		if(m_istm.read(p, size).bad())
		{
			throw std::runtime_error("Read Error!");
		}
	}
private:
	std::ifstream m_istm;
	same_endian_type m_same_type;
};

template<typename same_endian_type, typename T>
 file_istream<same_endian_type>& operator >> ( file_istream<same_endian_type>& istm, T& val)
{
	istm.read(val);

	return istm;
}

template<typename same_endian_type>
 file_istream<same_endian_type>& operator >> ( file_istream<same_endian_type>& istm, std::string& val)
{
	int size = 0;
	istm.read(size);

	if(size<=0)
		return istm;

	std::vector<char> vec((size_t)size);
	istm.read(vec);
	val.assign(&vec[0], (size_t)size);

	return istm;
}

template<typename same_endian_type>
class mem_istream
{
public:
	mem_istream() : m_index(0) {}
	mem_istream(const char * mem, size_t size) 
	{
		open(mem, size);
	}
	mem_istream(const std::vector<char>& vec) 
	{
		m_index = 0;
		m_vec.reserve(vec.size());
		m_vec.assign(vec.begin(), vec.end());
	}
	void open(const char * mem, size_t size)
	{
		m_index = 0;
		m_vec.clear();
		m_vec.reserve(size);
		m_vec.assign(mem, mem + size);
	}
	void close()
	{
		m_vec.clear();
	}
	bool eof() const
	{
		return m_index >= m_vec.size();
	}
	std::ifstream::pos_type tellg()
	{
		return m_index;
	}
	bool seekg (size_t pos)
	{
		if(pos<m_vec.size())
			m_index = pos;
		else 
			return false;

		return true;
	}
	bool seekg (std::streamoff offset, std::ios_base::seekdir way)
	{
		if(way==std::ios_base::beg && offset < m_vec.size())
			m_index = offset;
		else if(way==std::ios_base::cur && (m_index + offset) < m_vec.size())
			m_index += offset;
		else if(way==std::ios_base::end && (m_vec.size() + offset) < m_vec.size())
			m_index = m_vec.size() + offset;
		else
			return false;

		return true;
	}

	const std::vector<char>& get_internal_vec()
	{
		return m_vec;
	}

	template<typename T>
	void read(T& t)
	{
		if(eof())
			throw std::runtime_error("Premature end of array!");

		if((m_index + sizeof(T)) > m_vec.size())
			throw std::runtime_error("Premature end of array!");

		std::memcpy(reinterpret_cast<void*>(&t), &m_vec[m_index], sizeof(T));

		old::swap(t, m_same_type);

		m_index += sizeof(T);
	}

	void read(typename std::vector<char>& vec)
	{
		if (eof())
			throw std::runtime_error("Premature end of array!");

		if ((m_index + vec.size()) > m_vec.size())
			throw std::runtime_error("Premature end of array!");

		std::memcpy(reinterpret_cast<void*>(&vec[0]), &m_vec[m_index], vec.size());

		m_index += vec.size();
	}

	void read(char* p, size_t size)
	{
		if(eof())
			throw std::runtime_error("Premature end of array!");

		if((m_index + size) > m_vec.size())
			throw std::runtime_error("Premature end of array!");

		std::memcpy(reinterpret_cast<void*>(p), &m_vec[m_index], size);

		m_index += size;
	}

	void read(std::string& str, const unsigned int size)
	{
		if (eof())
			throw std::runtime_error("Premature end of array!");

		if ((m_index + str.size()) > m_vec.size())
			throw std::runtime_error("Premature end of array!");

		str.assign(&m_vec[m_index], size);

		m_index += str.size();
	}

private:
	std::vector<char> m_vec;
	size_t m_index;
	same_endian_type m_same_type;
};

template<typename same_endian_type, typename T>
 mem_istream<same_endian_type>& operator >> ( mem_istream<same_endian_type>& istm, T& val)
{
	istm.read(val);

	return istm;
}

template<typename same_endian_type>
mem_istream<same_endian_type>& operator >> (mem_istream<same_endian_type>& istm, std::string& val)
{
	int size = 0;
	istm.read(size);

	if(size<=0)
		return istm;

	istm.read(val, size);

	return istm;
}

template<typename same_endian_type>
class ptr_istream
{
public:
	ptr_istream() : m_arr(nullptr), m_size(0), m_index(0) {}
	ptr_istream(const char * mem, size_t size) : m_arr(nullptr), m_size(0), m_index(0)
	{
		open(mem, size);
	}
	ptr_istream(const std::vector<char>& vec)
	{
		m_index = 0;
		m_arr = vec.data();
		m_size = vec.size();
	}
	void open(const char * mem, size_t size)
	{
		m_index = 0;
		m_arr = mem;
		m_size = size;
	}
	void close()
	{
		m_arr = nullptr; m_size = 0; m_index = 0;
	}
	bool eof() const
	{
		return m_index >= m_size;
	}
	std::ifstream::pos_type tellg()
	{
		return m_index;
	}
	bool seekg(size_t pos)
	{
		if (pos<m_size)
			m_index = pos;
		else
			return false;

		return true;
	}
	bool seekg(std::streamoff offset, std::ios_base::seekdir way)
	{
		if (way == std::ios_base::beg && offset < m_size)
			m_index = offset;
		else if (way == std::ios_base::cur && (m_index + offset) < m_size)
			m_index += offset;
		else if (way == std::ios_base::end && (m_size + offset) < m_size)
			m_index = m_size + offset;
		else
			return false;

		return true;
	}

	template<typename T>
	void read(T& t)
	{
		if (eof())
			throw std::runtime_error("Premature end of array!");

		if ((m_index + sizeof(T)) > m_size)
			throw std::runtime_error("Premature end of array!");

		std::memcpy(reinterpret_cast<void*>(&t), &m_arr[m_index], sizeof(T));

		old::swap(t, m_same_type);

		m_index += sizeof(T);
	}

	void read(typename std::vector<char>& vec)
	{
		if (eof())
			throw std::runtime_error("Premature end of array!");

		if ((m_index + vec.size()) > m_size)
			throw std::runtime_error("Premature end of array!");

		std::memcpy(reinterpret_cast<void*>(&vec[0]), &m_arr[m_index], vec.size());

		m_index += vec.size();
	}

	void read(char* p, size_t size)
	{
		if (eof())
			throw std::runtime_error("Premature end of array!");

		if ((m_index + size) > m_size)
			throw std::runtime_error("Premature end of array!");

		std::memcpy(reinterpret_cast<void*>(p), &m_arr[m_index], size);

		m_index += size;
	}

	void read(std::string& str, const unsigned int size)
	{
		if (eof())
			throw std::runtime_error("Premature end of array!");

		if ((m_index + str.size()) > m_size)
			throw std::runtime_error("Premature end of array!");

		str.assign(&m_arr[m_index], size);

		m_index += str.size();
	}

private:
	const char* m_arr;
	size_t m_size;
	size_t m_index;
	same_endian_type m_same_type;
};


template<typename same_endian_type, typename T>
 ptr_istream<same_endian_type>& operator >> ( ptr_istream<same_endian_type>& istm, T& val)
{
	istm.read(val);

	return istm;
}

template<typename same_endian_type>
 ptr_istream<same_endian_type>& operator >> ( ptr_istream<same_endian_type>& istm, std::string& val)
{
	int size = 0;
	istm.read(size);

	if (size <= 0)
		return istm;

	istm.read(val, size);

	return istm;
}

template<typename same_endian_type>
class file_ostream
{
public:
	file_ostream() {}
	file_ostream(const char * file)
	{
		open(file);
	}
	void open(const char * file)
	{
		m_ostm.open(file, std::ios_base::out | std::ios_base::binary);
	}
	void flush()
	{
		m_ostm.flush();
	}
	void close()
	{
		m_ostm.close();
	}
	bool is_open()
	{
		return m_ostm.is_open();
	}
	template<typename T>
	void write(const T& t)
	{
		T t2 = t;
		old::swap(t2, m_same_type);
		m_ostm.write(reinterpret_cast<const char*>(&t2), sizeof(T));
	}
	void write(const std::vector<char>& vec)
	{
		m_ostm.write(reinterpret_cast<const char*>(&vec[0]), vec.size());
	}
	void write(const char* p, size_t size)
	{
		m_ostm.write(p, size);
	}

private:
	std::ofstream m_ostm;
	same_endian_type m_same_type;
};

template<typename same_endian_type, typename T>
file_ostream<same_endian_type>& operator << (file_ostream<same_endian_type>& ostm, const T& val)
{
	ostm.write(val);

	return ostm;
}

template<typename same_endian_type>
 file_ostream<same_endian_type>& operator << ( file_ostream<same_endian_type>& ostm, const std::string& val)
{
	int size = val.size();
	ostm.write(size);

	if(val.size()<=0)
		return ostm;

	ostm.write(val.c_str(), val.size());

	return ostm;
}

template<typename same_endian_type>
 file_ostream<same_endian_type>& operator << ( file_ostream<same_endian_type>& ostm, const char* val)
{
	int size = std::strlen(val);
	ostm.write(size);

	if(size<=0)
		return ostm;

	ostm.write(val, size);

	return ostm;
}

template<typename same_endian_type>
class mem_ostream
{
public:
	mem_ostream() {}
	void close()
	{
		m_vec.clear();
	}
	const std::vector<char>& get_internal_vec()
	{
		return m_vec;
	}
	template<typename T>
	void write(const T& t)
	{
		std::vector<char> vec(sizeof(T));
		T t2 = t;
		old::swap(t2, m_same_type);
		std::memcpy(reinterpret_cast<void*>(&vec[0]), reinterpret_cast<const void*>(&t2), sizeof(T));
		write(vec);
	}
	void write(const std::vector<char>& vec)
	{
		m_vec.insert(m_vec.end(), vec.begin(), vec.end());
	}
	void write(const char* p, size_t size)
	{
		for(size_t i=0; i<size; ++i)
			m_vec.push_back(p[i]);
	}

private:
	std::vector<char> m_vec;
	same_endian_type m_same_type;
};

template<typename same_endian_type, typename T>
 mem_ostream<same_endian_type>& operator << ( mem_ostream<same_endian_type>& ostm, const T& val)
{
	ostm.write(val);

	return ostm;
}

template<typename same_endian_type>
 mem_ostream<same_endian_type>& operator << ( mem_ostream<same_endian_type>& ostm, const std::string& val)
{
	int size = val.size();
	ostm.write(size);

	if(val.size()<=0)
		return ostm;

	ostm.write(val.c_str(), val.size());

	return ostm;
}

template<typename same_endian_type>
 mem_ostream<same_endian_type>& operator << ( mem_ostream<same_endian_type>& ostm, const char* val)
{
	int size = std::strlen(val);
	ostm.write(size);

	if(size<=0)
		return ostm;

	ostm.write(val, size);

	return ostm;
}

} // ns old

#endif // OldBinStream_H