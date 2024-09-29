#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <algorithm>

int main()
{
	std::ifstream libin( "opening.lib", std::ios_base::binary );
	std::ofstream libout( "output.lib", std::ios_base::binary );

	if( libin.bad() || libout.bad() ) {
		std::cout << "Cannot open library. Exiting..." << std::endl;
		return -1;
	}

	std::string content( std::istreambuf_iterator<char>{libin}, std::istreambuf_iterator<char>() );

	std::for_each( content.begin(), content.end(), [](char& c){ c = (c == 0x3F) ? c : 0; });		// keep invalid moves
	// std::for_each( content.begin(), content.end(), [](char& c){ c &= 0x40; });		// keep invalid moves

	libout << content;

	libin.close();
	libout.close();

	std::cout << "Done!" << std::endl;
	return 0;
}