// TestBinStream.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include "SimpleBinStream.h"
#include "CustomOperators.h"

void TestMissingString();
void TestMem();
void TestMemPtr();
void TestFile();
void TestMemFile();
void TestMemCustomOperatorsOnVec();
void TestMemPtrCustomOperatorsOnVec();
void TestFileCustomOperatorsOnVec();
void TestMemFileCustomOperatorsOnVec();
void TestMemCustomOperators();
void TestMemPtrCustomOperators();
void TestFileCustomOperators();
void TestMemFileCustomOperators();

using namespace std;
int main(int argc, char* argv[])
{
	TestMissingString();
	std::cout << "=============" << std::endl;
	
	std::cout << "-------------" << std::endl;
	/*
	TestMem();
	std::cout << "=============" << std::endl;
	TestMemPtr();
	std::cout << "=============" << std::endl;
	TestFile();
	std::cout << "=============" << std::endl;
	TestMemFile();
	std::cout << "=============" << std::endl;
	TestMemCustomOperatorsOnVec();
	std::cout << "=============" << std::endl;
	TestMemPtrCustomOperatorsOnVec();
	std::cout << "=============" << std::endl;
	TestFileCustomOperatorsOnVec();
	std::cout << "=============" << std::endl;
	TestMemFileCustomOperatorsOnVec();
	std::cout << "=============" << std::endl;
	TestMemCustomOperators();
	std::cout << "=============" << std::endl;
	TestMemPtrCustomOperators();
	std::cout << "=============" << std::endl;
	TestFileCustomOperators();
	std::cout << "=============" << std::endl;
	TestMemFileCustomOperators();
	std::cout << "=============" << std::endl;*/
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

void TestMissingString()
{
	simple::mem_ostream<std::true_type> out;
	out << "Jack" << "" << "Hello world!";

	simple::mem_istream<std::true_type> in(out.get_internal_vec());
	std::string str;
	in >> str;
	cout << str << ",";
	in >> str;
	cout << str << ",";
	in >> str;
	cout << str << ",";
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
	simple::file_ostream<std::true_type> out("file.bin");
	out << 23 << 24 << "Hello world!";
	out.flush();
	out.close();

	simple::file_istream<std::true_type> in("file.bin");
	int num1 = 0, num2 = 0;
	std::string str;
	in >> num1 >> num2 >> str;

	cout << num1 << "," << num2 << "," << str << endl;
}

void TestMemFile()
{
	simple::memfile_ostream<std::true_type> out;
	out << 23 << 24 << "Hello world!";
	out.write_to_file("file2.bin");

	simple::memfile_istream<std::true_type> in("file2.bin");
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
	simple::file_ostream<std::true_type> out("file.bin");
	out << vec_src;
	out.flush();
	out.close();

	simple::file_istream<std::true_type> in("file.bin");
	std::vector<Product> vec_dest;
	in >> vec_dest;

	print_products(vec_dest);
}

void TestMemFileCustomOperatorsOnVec()
{
	std::vector<Product> vec_src;
	vec_src.push_back(Product("Book", 10.0f, 50));
	vec_src.push_back(Product("Phone", 25.0f, 20));
	vec_src.push_back(Product("Pillow", 8.0f, 10));
	simple::memfile_ostream<std::true_type> out;
	out << vec_src;
	out.write_to_file("file4.bin");

	simple::memfile_istream<std::true_type> in("file4.bin");
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
	simple::file_ostream<std::true_type> out("file2.bin");
	out << Product("Book", 10.0f, 50);
	out << Product("Phone", 25.0f, 20);
	out.flush();
	out.close();

	simple::file_istream<std::true_type> in("file2.bin");
	Product product;
	in >> product;
	print_product(product);
	in >> product;
	print_product(product);
}

void TestMemFileCustomOperators()
{
	simple::memfile_ostream<std::true_type> out;
	out << Product("Book", 10.0f, 50);
	out << Product("Phone", 25.0f, 20);
	out.write_to_file("file3.bin");

	simple::memfile_istream<std::true_type> in("file3.bin");
	Product product;
	in >> product;
	print_product(product);
	in >> product;
	print_product(product);
}
