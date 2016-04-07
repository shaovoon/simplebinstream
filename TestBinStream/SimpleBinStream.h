// The MIT License (MIT)
// Simplistic Binary Streams 0.9
// Copyright (C) 2014, by Wong Shao Voon (shaovoon@yahoo.com)
//
// http://opensource.org/licenses/MIT
//
// version 0.9.2   : Optimize mem_istream constructor for const char*
// version 0.9.3   : Optimize mem_ostream vector insert

#ifndef MiniBinStream_H
#define MiniBinStream_H

#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <stdexcept>
#include <iostream>

namespace simple
{

class file_istream
{
public:
	file_istream() {}
	file_istream(const char * file, std::ios_base::openmode mode) 
	{
		open(file, mode);
	}
	void open(const char * file, std::ios_base::openmode mode)
	{
		m_istm.open(file, mode);
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
};

template<>
void file_istream::read(std::vector<char>& vec)
{
	if(m_istm.read(reinterpret_cast<char*>(&vec[0]), vec.size()).bad())
	{
		throw std::runtime_error("Read Error!");
	}
}

template<typename T>
file_istream& operator >> (file_istream& istm, T& val)
{
	istm.read(val);

	return istm;
}

template<>
file_istream& operator >> (file_istream& istm, std::string& val)
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

		m_index += sizeof(T);
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

private:
	std::vector<char> m_vec;
	size_t m_index;
};

template<>
void mem_istream::read(std::vector<char>& vec)
{
	if(eof())
		throw std::runtime_error("Premature end of array!");
		
	if((m_index + vec.size()) > m_vec.size())
		throw std::runtime_error("Premature end of array!");

	std::memcpy(reinterpret_cast<void*>(&vec[0]), &m_vec[m_index], vec.size());

	m_index += vec.size();
}

template<typename T>
mem_istream& operator >> (mem_istream& istm, T& val)
{
	istm.read(val);

	return istm;
}

template<>
mem_istream& operator >> (mem_istream& istm, std::string& val)
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

class file_ostream
{
public:
	file_ostream() {}
	file_ostream(const char * file, std::ios_base::openmode mode)
	{
		open(file, mode);
	}
	void open(const char * file, std::ios_base::openmode mode)
	{
		m_ostm.open(file, mode);
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
		m_ostm.write(reinterpret_cast<const char*>(&t), sizeof(T));
	}
	void write(const char* p, size_t size)
	{
		m_ostm.write(p, size);
	}

private:
	std::ofstream m_ostm;

};

template<>
void file_ostream::write(const std::vector<char>& vec)
{
	m_ostm.write(reinterpret_cast<const char*>(&vec[0]), vec.size());
}

template<typename T>
file_ostream& operator << (file_ostream& ostm, const T& val)
{
	ostm.write(val);

	return ostm;
}

template<>
file_ostream& operator << (file_ostream& ostm, const std::string& val)
{
	int size = val.size();
	ostm.write(size);

	if(val.size()<=0)
		return ostm;

	ostm.write(val.c_str(), val.size());

	return ostm;
}

file_ostream& operator << (file_ostream& ostm, const char* val)
{
	int size = std::strlen(val);
	ostm.write(size);

	if(size<=0)
		return ostm;

	ostm.write(val, size);

	return ostm;
}

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
		std::memcpy(reinterpret_cast<void*>(&vec[0]), reinterpret_cast<const void*>(&t), sizeof(T));
		write(vec);
	}
	void write(const char* p, size_t size)
	{
		for(size_t i=0; i<size; ++i)
			m_vec.push_back(p[i]);
	}

private:
	std::vector<char> m_vec;
};

template<>
void mem_ostream::write(const std::vector<char>& vec)
{
	m_vec.insert(m_vec.end(), vec.begin(), vec.end());
}

template<typename T>
mem_ostream& operator << (mem_ostream& ostm, const T& val)
{
	ostm.write(val);

	return ostm;
}

template<>
mem_ostream& operator << (mem_ostream& ostm, const std::string& val)
{
	int size = val.size();
	ostm.write(size);

	if(val.size()<=0)
		return ostm;

	ostm.write(val.c_str(), val.size());

	return ostm;
}

mem_ostream& operator << (mem_ostream& ostm, const char* val)
{
	int size = std::strlen(val);
	ostm.write(size);

	if(size<=0)
		return ostm;

	ostm.write(val, size);

	return ostm;
}

} // ns simple

#endif // MiniBinStream_H