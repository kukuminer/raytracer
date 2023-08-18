#include "raytracer.cpp"
#include <iostream>

int main(int argc, char* argv[])
{
	printf("argc %i\n", argc);
	for(auto c = 1; c < argc; c++)
	{
		raytracer r(argv[c]);
		r.makeImg();
	}	
	cout << "~~~" << endl;
	cout << "All inputs complete!" << endl;
	return 0;
}