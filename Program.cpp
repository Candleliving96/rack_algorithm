#include <iostream>
#include <fstream>
#include <string>
#include "Program.h"

using namespace std;

void Program::read_data() {
    string filename;
    cout << "This is the Rack Distribution Program, please enter the name of a data file (must include .txt extension at end of name)" << endl;
    cin >> filename;

    ifstream infile;
    infile.open(filename);
    if (infile.fail()) {
        cout << "Error opening file. Check that file name is valid and that the file is located in the inner project folder" << endl;
        exit(EXIT_FAILURE);
    }
    while (!infile.eof()) {
        Source_Rack current_source;
        infile >> current_source.id;
        infile >> current_source.num_samples;
        all_sources.push_back(current_source);
    }
}

void Program::populate_frequencies() {
    //initialize sample_frequencies array with 0's
    for (int i = 1; i < rack_capacity; i++) {
        sample_frequencies[i] = 0;
    }

    //get how many 1's, 2's, 3's, etc there are
    for (int i = 0; i < all_sources.size(); i++) {
        sample_frequencies[all_sources.at(i).num_samples]++;
    }
}

//for testing purposes
void Program::print_frequencies() {
    cout << endl;
    for (auto it : sample_frequencies) {
        cout << it.first << ": " << it.second << endl;
    }
}

//for testing purposes
void Program::print_sources() {
    for (int i = 0; i < all_sources.size(); i++) {
        cout << all_sources.at(i).id << " " << all_sources.at(i).num_samples << endl;
    }
}

//returns the largest sample number left based on sample_frequencies
int Program::find_largest() {
    for (int i = rack_capacity - 1; i > 0; i--) {
        //starting from the highest number, return the first sample num with non-zero frequency
        if (sample_frequencies[i] != 0) {
            return i;
        }
    }
    //if all are 0, all source racks have been used
    cerr << "error, see line 64" << endl;
    exit(EXIT_FAILURE);
}

void Program::distribute_racks() {
    while (all_sources.size() > 19) {
        //choose the number of sources in the current batch
        int num_sources = choose_num_sources();
        create_new_batch(num_sources);
    }
    distribute_remainder();
}

void Program::distribute_remainder() {
    testing_array.clear();
    for (auto it: sample_frequencies) {
        int amount_to_add = it.second;
        for (int i = 0; i < it.second; i++) {
            add_in_order(testing_array, it.first);
        }
    }
    finished_batches.push_back(finalize_spots());
}

//95, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20
//could be: 5 source, 2 dest. 10 source, 3 dest. 15 source, 4 dest.
//89, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
// 
//take the highest + some amount of the lowest
// keep increasing num sources until total is greater than spots in num destinations
// then, use num sources - 1 as the number of sources
int Program::choose_num_sources() {
    testing_array.clear();

    int num_sources = 1;
    int destination_spots = (20 - num_sources) * 96;

    //add the largest value
    int largest = find_largest();
    add_in_order(testing_array, largest);

    //find the most sources we can add to the highest value without it exceeding the number of destination spots by only adding the lowest sources left
    while (total_testing_samples() <= destination_spots && num_sources < 20) {
        add_in_order(testing_array, find_smallest());
        num_sources++;
        destination_spots = (20 - num_sources) * 96;
    }
    //remove all values from testing array to reset for when the values are actually added
    int num_to_remove = testing_array.size();
    for (int i = 0; i < num_to_remove; i++) {
        remove_from_testing(testing_array.at(0));
    }
    //at the point, destination spots has been exceeded, so decrease num sources and return
    return num_sources - 1;
}

bool Program::is_valid(int num) {
    if (sample_frequencies[num] > 0) {
        return true;
    }
    return false;
}

void Program::export_results() {
    cout << "A text file version compatible with excel will be exported, please enter the name you would like to save the file as:" << endl;
    string output_name;
    cin >> output_name;
    string test = output_name.substr(output_name.size() - 4, 4);
    if (test != ".txt") {
        output_name += ".txt";
    }

    ofstream outfile;
    outfile.open(output_name);
    if (outfile.fail()) {
        cout << "Error creating file. Please check that the inputted file name is valid and re-run program" << endl;
    }

    string output = "Rack ID,Sample Number,Batch Number,Number of Samples In Batch\n";

    for (int i = 0; i < finished_batches.size(); i++) {
        for (int j = 0; j < finished_batches.at(i).batch_sources.size(); j++) {
            //add name/ID to output
            output += (finished_batches.at(i).batch_sources.at(j).id);
            output += ",";

            //add number of samples in rack to output
            output += to_string(finished_batches.at(i).batch_sources.at(j).num_samples);
            output += ",";

            //add batch number to output
            output += to_string(i + 1);
            output += ",";

            //add number of samples in batch
            int total_spots_filled = 0;
            for (int k = 0; k < finished_batches.at(i).batch_sources.size(); k++) {
                total_spots_filled += finished_batches.at(i).batch_sources.at(k).num_samples;
            }
            output += to_string(total_spots_filled);
            output += "\n";
        }
        output += "\n";
    }

    outfile << output << endl;
    cout << endl << "Your text file has been created with the name " << output_name << endl;
    cout << "Output file should be located in same folder as input file" << endl;

    outfile.close();
}

int Program::find_next_smallest_valid(int &current, bool &approximate) {
    if (valid_sample_nums.size() == 1) {
        approximate = true;
        return -1;
    }
    for (int i = 0; i < valid_sample_nums.size() - 1; i++) {
        if (current == valid_sample_nums.at(i)) {
            return valid_sample_nums.at(i + 1);
        }
    }
    //if made it to the end, then current doesn't exist in the valid sample numbers array
    cerr << "error: current isn't in valid list when adding next smallest" << endl;
    exit(EXIT_FAILURE);
}

int Program::find_next_highest_valid(int &current) {
    //cout << "finding next highest valid" << endl;
    if (current == valid_sample_nums.at(1)) {
        cerr << "error: only 1 lowest valid left, shouldn't go past this" << endl;
        return valid_sample_nums.at(0);
    }
    for (int i = valid_sample_nums.size() - 1; i > 0; i--) {
        if (current == valid_sample_nums.at(i)) {
            return valid_sample_nums.at(i - 1);
        }
    }
    //if made it to the end, then current doesn't exist in the valid sample numbers array
    cerr << "error: current does not exist in the valid sample nums or it is the smallest value already";
    exit(EXIT_FAILURE);
}


void Program::create_new_batch(int num_source_racks) {
    //add all values except one and find the ideal last spot
    testing_array.clear();   
    add_all_except_last(num_source_racks);
    fill_valid_sample_nums();
    int ideal_last_spot = ideal_last(num_source_racks);

    int to_add;
    while (ideal_last_spot < 1) {
        int second_largest = testing_array.at(testing_array.size() - 2);
        to_add = find_next_highest_valid(second_largest);
        remove_from_testing(second_largest);
        add_in_order(testing_array, to_add);
        to_add = find_next_highest_valid(to_add);
        ideal_last_spot = ideal_last(num_source_racks);
    }

    bool approximate = false;
    //if not valid, remove the current smallest and add the next valid smallest
    while (ideal_last_spot > 1 && ideal_last_spot < 96 && !is_valid(ideal_last_spot)) {
        int smallest_in_testing = valid_sample_nums.at(0);
        to_add = find_next_smallest_valid(smallest_in_testing, approximate);
        if (approximate) {
            break;
        }
        //this will increase the total in the testing array and decrease ideal last spot
        remove_from_testing(smallest_in_testing);
        add_in_order(testing_array, to_add);
        valid_sample_nums.erase(valid_sample_nums.begin());
        ideal_last_spot = ideal_last(num_source_racks);
    }
    if (approximate) {
        add_in_order(testing_array, find_largest());
    }
    else {
        add_in_order(testing_array, ideal_last_spot);
    }

    finished_batches.push_back(finalize_spots());    
}

void Program::print_summary() {
    string overview;
    cout << "The program has created a solution, would you like to see to see a reader-friendly overview before exporting as a text file? (y/n)" << endl;
    cin >> overview;

    if (overview == "y" || overview == "yes") {
        cout << "Number of batches is: " << finished_batches.size() << endl;
        for (int i = 0; i < finished_batches.size(); i++) {
            cout << "-- Batch number " << i + 1 << " --" << endl;
            cout << "Number of sources in this batch: " << finished_batches.at(i).batch_sources.size() << endl;

            int num_destinations;
            if (finished_batches.at(i).batch_sources.size() == 18 || finished_batches.at(i).batch_sources.size() == 19) {
                num_destinations = 20 - finished_batches.at(i).batch_sources.size();
            }
            else {
                num_destinations = 1;
            }
            cout << "Number of destinations in this batch: " << num_destinations << endl; // FIX LAST BATCH

            int total_spots_filled = 0;
            for (int k = 0; k < finished_batches.at(i).batch_sources.size(); k++) {
                total_spots_filled += finished_batches.at(i).batch_sources.at(k).num_samples;
            }
            cout << "Number of spots filled in destination racks: " << total_spots_filled << endl;
        }
        cout << endl;
    }
}

//fills only the non-zero values
void Program::fill_valid_sample_nums() {
    valid_sample_nums.clear();
    for (auto it : sample_frequencies) {
        if (it.second > 0) {
            add_in_order(valid_sample_nums, it.first);
        }
    }
}

//for testing purposes
void Program::print_valid_sample_nums() {
    cout << "printing valid sample numbers: ";
    for (int i = 0; i < valid_sample_nums.size(); i++) {
        cout << valid_sample_nums.at(i) << " ";
    }
    cout << endl;
}


Program::Batch Program::finalize_spots() {
    Batch curr_batch;
    curr_batch.batch_num = finished_batches.size() + 1;
    for (int i = 0; i < testing_array.size(); i++) {
        Source_Rack curr_source = find_source(testing_array.at(i));
        curr_batch.batch_sources.push_back(curr_source);
    }
    return curr_batch;
}

Program::Source_Rack Program::find_source(int sample_num) {
    for (int i = 0; i < all_sources.size(); i++) {
        if (all_sources.at(i).num_samples == sample_num) {
            Source_Rack my_source;
            my_source.id = all_sources.at(i).id;
            my_source.num_samples = sample_num;
            all_sources.erase(all_sources.begin() + i);
            return my_source;
        }
    }
    //if not found, something went wrong
    cerr << "source " << sample_num << "not found when finalizing spots, exiting now";
    exit(EXIT_FAILURE);
}

void Program::increase_testing_total(int &ideal_last_spot, int num_source_racks, int &to_remove) {
    int next_available = to_remove + 1;
    while (sample_frequencies[next_available] == 0) {
        next_available++;
    }
    remove_from_testing(to_remove);
    add_in_order(testing_array, next_available);
    to_remove = next_available;
    ideal_last_spot = ideal_last(num_source_racks);
}


void Program::add_all_except_last(int num_source_racks) {
    //add largest value
    add_in_order(testing_array, find_largest());

    //add frequencies based on ratios
    add_ratios(num_source_racks);

    //add extra of the smallest value to add until there is only 1 spot left
    while (testing_array.size() < num_source_racks - 1) {
        if (find_smallest() == -1) {
            cerr << "no more source racks to distribute, returning now" << endl;
            return;
        }
        else {
            add_in_order(testing_array, find_smallest());
        }
    }
}

int Program::ideal_last(int num_source_racks) {
    int goal_sample_num = (20 - num_source_racks) * rack_capacity;
    //cout << "total testing samples is " << total_testing_samples() << endl;
    return goal_sample_num - total_testing_samples();
}

int Program::total_testing_samples() {
    int total = 0;
    for (int i = 0; i < testing_array.size(); i++) {
        total += testing_array.at(i);
    }
    return total;
}


int Program::find_smallest() {
    for (int i = 1; i < rack_capacity; i++) {
        if (sample_frequencies[i] != 0) {
            return i;
        }
    }
    //if no source racks left, return -1
    return -1;
}

void Program::add_ratios(int num_source_racks) {
    //calculate how many 1's, 2's, etc to used based on frequencies
    //i is the sample number
    for (int i = 1; i < rack_capacity; i++) {
        //if there are no source_racks with that number of samples left, move on
        if (sample_frequencies[i] == 0) {
            continue;
        }
        else {
            //find amount to add based on ratio, truncate to an integer
            int amount_to_add = (sample_frequencies[i] * num_source_racks) / all_sources.size();
            //add that amount of the sample number to the array as long as there are enough
            while (amount_to_add > 0 && sample_frequencies[i] > 0) {
                add_in_order(testing_array, i);
                amount_to_add--;
            }
        }
    }

}

void Program::print_testing_array() {
    if (testing_array.size() == 0) {
        cerr << "testing array is empty" << endl;
        return;
    }
    cout << "testing array: ";
    for (int i = 0; i < testing_array.size(); i++) {
        cout << testing_array.at(i) << " ";
    }
    cout << endl;
}


void Program::add_in_order(vector<int> &which_vector, int to_add) {
    string s;
    if (which_vector == testing_array) {
        s = "testing array";
    }
    else {
        s = "valid sample nums";
    }

    //if testing array is empty or to_add is the largest value, add to back
    if (which_vector.size() == 0 || to_add >= which_vector.at(which_vector.size() - 1)) {
        which_vector.push_back(to_add);
    } else {
        //otherwise, add to the first spot that would be in order
        for (int i = 0; i < which_vector.size(); i++) {
            if (to_add < which_vector.at(i)) {
                which_vector.insert(which_vector.begin() + i, to_add);
                break;
            }
        }
    }
    if (which_vector == testing_array) {
        sample_frequencies[to_add]--;
    }
}

void Program::remove_from_testing(int to_remove) {
    //find index of element to erase
    int index = -1;

    for (int i = 0; i < testing_array.size(); i++) {
        if (testing_array.at(i) == to_remove) {
            index = i;
        }
    }
    if (index == -1) {
        cerr << to_remove << " is not in testing array" << endl;
    }
    testing_array.erase(testing_array.begin() + index);
    sample_frequencies[to_remove]++;
}
