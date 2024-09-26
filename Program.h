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
	void read_data();
	void populate_frequencies();
	void distribute_racks();
	void distribute_remainder();

	int find_largest();
	int find_smallest();
	int ideal_last(int num_source_spots);
	int total_testing_samples();

	void create_new_batch(int num_source_spots);

	int choose_num_sources();
	void increase_testing_total(int &ideal_last, int num_source_racks, int &to_remove);

	void add_all_except_last(int num_source_spots);
	void add_ratios(int num_source_spots);

	void add_in_order(vector<int> &which_vector, int to_add);
	void remove_from_testing(int to_remove);
	int find_next_smallest_valid(int &current, bool &approximate);
	int find_next_highest_valid(int &current);

	void print_frequencies();
	void print_sources();
	void print_testing_array();
	void print_valid_sample_nums();
	void print_summary();
	void export_results();

	void fill_valid_sample_nums();
	bool is_valid(int num);

private:
	int rack_capacity;
	vector<int> testing_array;

	//definition for Source_Rack
	struct Source_Rack {
		string id;
		int num_samples;
	};

	//definition for Batch
	struct Batch {
		int batch_num;
		vector<Source_Rack> batch_sources;
	};


	vector<Source_Rack> all_sources;
	vector<Batch> finished_batches;

	//hash map to store frequencies of each sample number
	unordered_map<int, int> sample_frequencies;

	//hash map to store only the non-zero sample numbers
	vector<int> valid_sample_nums;

	//declare after to avoid compiler errors
	Batch finalize_spots();
	Source_Rack find_source(int sample_num);
};

#endif
