#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include "C:\Users\Yonghui\source\repos\Rack_Final\Program.h"

using namespace std;
const int SOURCE_SPOTS = 96;

int main() {
	Program my_program(SOURCE_SPOTS);
	my_program.read_data();

	//TODO: print info about how many racks there are

	my_program.populate_frequencies();
	my_program.distribute_racks();
	my_program.print_summary();
	my_program.export_results();
}