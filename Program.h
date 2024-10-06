/*
Program.h: interface for the Program class.

The Program class contains the Batch and Source_Rack struct definitions, a vector that
holds all the Source_Rack objects in the program, and a vector containing all the 
completed Batch objects. The methods of the Program class execute all of the functioning
that combines Source_Racks into a Batch, utilizing additional data members like the 
sample_frequencies hash map and a testing vector to guide Batch creation.

*/

#ifndef PROGRAM_H
#define PROGRAM_H

#include <vector>
#include <array>
#include <unordered_map>

using namespace std;

class Program {
public:
	//constructor
	Program(int rack_capacity) {
		this->rack_capacity = rack_capacity;
	}
	
	//read and analyze data about the rack sample numbers
	void read_data();
	void populate_frequencies();

	//create all batches
	void distribute_racks();

	//print methods for displaying results
	void print_summary();
	void export_results();

private:
	//number of spots in a rack, usually 96
	int rack_capacity;  
	
	//temporary vector finding the best combination of sample numbers before modifying actual all_sources array, cleared with each new batch
	vector<int> testing_array;

	//definition for Source_Rack
	struct Source_Rack {
		string id;
		int num_samples;
	};

	//definition for Batch
	struct Batch {
		int batch_num;
		//hold sources in batch
		vector<Source_Rack> batch_sources;
	};

	//all undistributed sources in program, samples are removed once added to a batch
	vector<Source_Rack> all_sources;

	//all currently finished batches in the program
	vector<Batch> finished_batches;

	//hash map to store frequencies of each sample number (key is sample number, value is frequency)
	unordered_map<int, int> sample_frequencies;

	//hash map to store only the non-zero sample numbers
	vector<int> valid_sample_nums;

	//higher-level methods for creating batches
	void create_new_batch(int num_source_spots);
	int choose_num_sources();
	void add_all_except_last(int num_source_spots);
	void add_ratios(int num_source_spots);
	Batch finalize_spots();
	Source_Rack find_source(int sample_num);
	void distribute_remainder();

	//lower-level helper methods for creating batches
	void fill_valid_sample_nums();
	bool is_valid(int num);
	int find_largest();
	int find_smallest();
	int ideal_last(int num_source_spots);
	int total_testing_samples();
	void increase_testing_total(int& ideal_last, int num_source_racks, int& to_remove);
	void add_in_order(vector<int>& which_vector, int to_add);
	void remove_from_testing(int to_remove);
	int find_next_smallest_valid(int& current, bool& approximate);
	int find_next_highest_valid(int& current);

	//print methods for testing purposes
	void print_frequencies();
	void print_sources();
	void print_testing_array();
	void print_valid_sample_nums();
};

#endif
