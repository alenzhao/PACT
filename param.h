/* param.h
This contains all the global variables used by pact, can be modified later if necessary
*/

#ifndef PARAM_H
#define PARAM_H

#include <cmath>

#include <vector>
using std::vector;

#define INF pow(double(10),double(100)) // 10^100 (~infinity)

class Parameters {

public:
	Parameters();						// constructor, imports parameters from in.param if available

	void print();						// prints parameter listing

	// PARAMETERS
	bool push_times_back;
	vector<double> push_times_back_values;	// start, stop
	
	bool prune_to_trunk;
	
	bool prune_to_label;
	vector<double> prune_to_label_values;	// label
	
	bool trim_ends;
	vector<double> trim_ends_values;		// start, stop
	
	bool section_tree;
	vector<double> section_tree_values;		// start, window, step
	
	bool time_slice;
	vector<double> time_slice_values;		// time
	
	bool print_hp_tree;
	
	bool summary_tmrca;		
	bool summary_length;			
	bool summary_proportions;	
	bool summary_coal_rates;		
	bool summary_mig_rates;		
	bool summary_diversity;		
	bool summary_fst;				
	bool summary_tajima_d;	
		
	vector<double> skyline_values;			// start, stop, step
	
	bool skyline_tmrca;	
	bool skyline_proportions;
	bool skyline_coal_rates;
	bool skyline_mig_rates;
	bool skyline_diversity;
	bool skyline_fst;
	bool skyline_tajima_d;
	
private:
	void importLine(string);			// reads a string and attempts to extract parameters from it
	
	
};

#endif