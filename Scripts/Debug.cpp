#include "Debug.h"

using namespace Engine;

template<typename T>
void Debug::printArray(std::vector<T> array)
{
	std::cout << '{';
	for (int i = 0; i < array.size(); i++)
	{
		std::cout << array[i] << ", ";
	}
	std::cout << '}';
}
template<typename T>
void Debug::printArray(T array[], int size)
{
	std::cout << '{';
	for (int i = 0; i < size; i++)
	{
		std::cout << array[i] << ", ";
	}
	std::cout << '}';
}