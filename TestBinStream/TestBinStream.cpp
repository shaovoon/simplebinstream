// TestBinStream.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include "SimpleBinStream.h"
#include "CustomOperators.h"

void TestMem();
void TestMemPtr();
void TestFile();
void TestMemCustomOperatorsOnVec();
void TestMemPtrCustomOperatorsOnVec();
void TestFileCustomOperatorsOnVec();
void TestMemCustomOperators();
void TestMemPtrCustomOperators();
void TestFileCustomOperators();

using namespace std;
int main(int argc, char* argv[])
{
	TestMem();
	std::cout << "=============" << std::endl;
	TestMemPtr();
	std::cout << "=============" << std::endl;
	TestFile();
	std::cout << "=============" << std::endl;
	TestMemCustomOperatorsOnVec();
	std::cout << "=============" << std::endl;
	TestMemPtrCustomOperatorsOnVec();
	std::cout << "=============" << std::endl;
	TestFileCustomOperatorsOnVec();
	std::cout << "=============" << std::endl;
	TestMemCustomOperators();
	std::cout << "=============" << std::endl;
	TestMemPtrCustomOperators();
	std::cout << "=============" << std::endl;
	TestFileCustomOperators();
	std::cout << "=============" << std::endl;
	return 0;
}

void TestMem()
{
	simple::mem_ostream<std::true_type> out;
	out << 23 << 24 << "Hello world!";

	simple::mem_istream<std::true_type> in(out.get_internal_vec());
	int num1 = 0, num2 = 0;
	std::string str;
	in >> num1 >> num2 >> str;

	cout << num1 << "," << num2 << "," << str << endl;
}

void TestMemPtr()
{
	using same_endian_type = std::is_same<simple::LittleEndian, simple::BigEndian>;
	simple::mem_ostream<same_endian_type> out;
	out << (int64_t)23 << (int64_t)24 << "Hello world!";

	simple::ptr_istream<same_endian_type> in(out.get_internal_vec());
	int64_t num1 = 0, num2 = 0;
	std::string str;
	in >> num1 >> num2 >> str;

	cout << num1 << "," << num2 << "," << str << endl;
}

void TestFile()
{
	simple::file_ostream<std::true_type> out("file.bin", std::ios_base::out | std::ios_base::binary);
	out << 23 << 24 << "Hello world!";
	out.flush();
	out.close();

	simple::file_istream<std::true_type> in("file.bin", std::ios_base::in | std::ios_base::binary);
	int num1 = 0, num2 = 0;
	std::string str;
	in >> num1 >> num2 >> str;

	cout << num1 << "," << num2 << "," << str << endl;
}

void TestMemCustomOperatorsOnVec()
{
	std::vector<Product> vec_src;
	vec_src.push_back(Product("Book", 10.0f, 50));
	vec_src.push_back(Product("Phone", 25.0f, 20));
	vec_src.push_back(Product("Pillow", 8.0f, 10));
	simple::mem_ostream<std::true_type> out;
	out << vec_src;

	simple::mem_istream<std::true_type> in(out.get_internal_vec());
	std::vector<Product> vec_dest;
	in >> vec_dest;

	print_products(vec_dest);
}

void TestMemPtrCustomOperatorsOnVec()
{
	std::vector<Product> vec_src;
	vec_src.push_back(Product("Book", 10.0f, 50));
	vec_src.push_back(Product("Phone", 25.0f, 20));
	vec_src.push_back(Product("Pillow", 8.0f, 10));
	simple::mem_ostream<std::true_type> out;
	out << vec_src;

	simple::ptr_istream<std::true_type> in(out.get_internal_vec());
	std::vector<Product> vec_dest;
	in >> vec_dest;

	print_products(vec_dest);
}

void TestFileCustomOperatorsOnVec()
{
	std::vector<Product> vec_src;
	vec_src.push_back(Product("Book", 10.0f, 50));
	vec_src.push_back(Product("Phone", 25.0f, 20));
	vec_src.push_back(Product("Pillow", 8.0f, 10));
	simple::file_ostream<std::true_type> out("file.bin", std::ios_base::out | std::ios_base::binary);
	out << vec_src;
	out.flush();
	out.close();

	simple::file_istream<std::true_type> in("file.bin", std::ios_base::in | std::ios_base::binary);
	std::vector<Product> vec_dest;
	in >> vec_dest;

	print_products(vec_dest);
}

void TestMemCustomOperators()
{
	simple::mem_ostream<std::true_type> out;
	out << Product("Book", 10.0f, 50);
	out << Product("Phone", 25.0f, 20);

	simple::mem_istream<std::true_type> in(out.get_internal_vec());
	Product product;
	in >> product;
	print_product(product);
	in >> product;
	print_product(product);
}

void TestMemPtrCustomOperators()
{
	simple::mem_ostream<std::true_type> out;
	out << Product("Book", 10.0f, 50);
	out << Product("Phone", 25.0f, 20);

	simple::ptr_istream<std::true_type> in(out.get_internal_vec());
	Product product;
	in >> product;
	print_product(product);
	in >> product;
	print_product(product);
}

void TestFileCustomOperators()
{
	simple::file_ostream<std::true_type> out("file2.bin", std::ios_base::out | std::ios_base::binary);
	out << Product("Book", 10.0f, 50);
	out << Product("Phone", 25.0f, 20);
	out.flush();
	out.close();

	simple::file_istream<std::true_type> in("file2.bin", std::ios_base::in | std::ios_base::binary);
	Product product;
	in >> product;
	print_product(product);
	in >> product;
	print_product(product);
}
