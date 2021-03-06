/* io.cpp
Copyright 2009-2012 Trevor Bedford <t.bedford@ed.ac.uk>
Member function definitions for IO class
*/

/*
This file is part of PACT.

PACT is free software: you can redistribute it and/or modify it under the terms of the GNU General 
Public License as published by the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

PACT is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the 
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General 
Public License for more details.

You should have received a copy of the GNU General Public License along with PACT.  If not, see 
<http://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <fstream>
#include <sstream>
using std::ifstream;
using std::ofstream;
using std::cout;
using std::endl;
using std::ios;
using std::unitbuf;
using std::stringstream;

#include <stdexcept>
using std::runtime_error;
using std::out_of_range;

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <set>
using std::set;
using std::multiset;

#include <cmath>
using std::abs;

#include "io.h"
#include "coaltree.h"
#include "series.h"

IO::IO() {

	// PARAMETER INPUT ////////////////
	// automatically loaded by declaring Parameter object in header
	param.print();

	// TREE INPUT /////////////////////
	inputFile = "in.trees";
	cout << "Reading trees from in.trees" << endl;
	
	ifstream inStream;
	inStream.open( inputFile.c_str(),ios::out);

	string line;
	int pos;
	int treesread = 0;
	if (inStream.is_open()) {
		while (! inStream.eof() ) {
			getline (inStream,line);
			
			if (line.size() > 0) {
				if (line[0] != '#') {
			
					// Catching log probabilities of trees
					string annoString;
					
					// migrate annotation
					annoString = "ln(L) = ";
					pos = line.find(annoString);
		
					if (pos >= 0) {
						string thisString;
						thisString = line.substr(pos+annoString.size());
						thisString.erase(thisString.find(' '));
						double ll = atof(thisString.c_str());
						problist.push_back(ll);
						line = "";								// ignore rest of line
					}
					
					// beast annotation
					annoString = "[&lnP=";
					pos = line.find(annoString);
		
					if (pos >= 0) {
						string thisString;
						thisString = line.substr(pos+annoString.size());
						thisString.erase(thisString.find(']'));
						double ll = atof(thisString.c_str());
						problist.push_back(ll);				
					}
					
					// find first occurance of '(' in line
					pos = line.find('(');
					if (pos >= 0) {
					
						// if burnin has finished
						if (param.burnin) {
							if ( treesread > (param.burnin_values)[0] ) {
						
								string paren = line.substr(pos);
								CoalescentTree ct(paren);
								treelist.push_back(ct);
						//		cout << "tree " << treelist.size() + (param.burnin_values)[0] << " read" << endl;
								cout << unitbuf << ".";
						
							}
						
						}
						
						else {

							string paren = line.substr(pos);
							CoalescentTree ct(paren);
							treelist.push_back(ct);	
						//	cout << "tree " << treelist.size() << " read" << endl;		
							cout << unitbuf << ".";
						
						}
						
						treesread++;
							
					}

				}
			}			
		}
		inStream.close();
	}
	else {
		throw runtime_error("tree file in.trees not found");
	}
	
	cout << endl;
	
	if (treelist.size() == 0) {
		throw runtime_error("no suitable trees on which to perform analysis");
	}
	
	// ZEROING OUTPUT FILES ///////////
	// append from now on
	// only zero files that will be used later
	outputPrefix = "out";
	ofstream outStream;
	string outputFile;

	if (param.printtree()) {
		outputFile = outputPrefix + ".rules";
		outStream.open( outputFile.c_str(),ios::out);
		outStream.close();
	}

	if (param.summary()) {
		outputFile = outputPrefix + ".stats";
		outStream.open( outputFile.c_str(),ios::out);
		outStream.close();
	}
	
	if (param.tips()) {
		outputFile = outputPrefix + ".tips";
		outStream.open( outputFile.c_str(),ios::out);
		outStream.close();
	}	

	if (param.skyline()) {
		outputFile = outputPrefix + ".skylines";
		outStream.open( outputFile.c_str(),ios::out);
		outStream.close();	
	}
	
	if (param.pairs()) {
		outputFile = outputPrefix + ".pairs";
		outStream.open( outputFile.c_str(),ios::out);
		outStream.close();	
	}	
	
}

/* go through treelist and perform tree manipulation operations */
void IO::treeManip() {

	if (param.manip()) {

		cout << "Performing tree manipulation operations"  << endl;
		
		for (int i = 0; i < treelist.size(); i++) {
		
			// PUSH TIMES BACK
			if (param.push_times_back) {
				if ( (param.push_times_back_values).size() == 1 ) {
					double stop = (param.push_times_back_values)[0];
					treelist[i].pushTimesBack(stop);
				}
				if ( (param.push_times_back_values).size() == 2 ) {
					double start = (param.push_times_back_values)[0];
					double stop = (param.push_times_back_values)[1];
					treelist[i].pushTimesBack(start,stop);
				}			
			}

			// REDUCE TIPS
			if (param.reduce_tips) {
				double pro = (param.reduce_tips_values)[0];
				treelist[i].reduceTips(pro);
			}	

			// RENEW TRUNK
			if (param.renew_trunk) {
				double time = (param.renew_trunk_values)[0];
				treelist[i].renewTrunk(time);
			}
						
			// TRIM ENDS
			if (param.trim_ends) {
				double start = (param.trim_ends_values)[0];
				double stop = (param.trim_ends_values)[1];
				treelist[i].trimEnds(start,stop);
			}
			
			// SECTION TREE
			if (param.section_tree) {
				double start = (param.section_tree_values)[0];
				double window = (param.section_tree_values)[1];
				double step = (param.section_tree_values)[2];
				treelist[i].sectionTree(start,window,step);
			}
		
			// TIME SLICE
			if (param.time_slice) {
				double time = (param.time_slice_values)[0];
				treelist[i].timeSlice(time);
			}
						
			// PRUNE TO LABEL
			if (param.prune_to_label) {
				string label = (param.prune_to_label_values)[0];
				treelist[i].pruneToLabel(label);
			}	
			
			// PRUNE TO TIPS
			if (param.prune_to_tips) {
				treelist[i].pruneToTips(param.prune_to_tips_values);
			}	
			
			// REMOVE TIPS
			if (param.remove_tips) {
				treelist[i].removeTips(param.remove_tips_values);
			}							
			
			// PRUNE TO TRUNK
			if (param.prune_to_trunk) {
				treelist[i].pruneToTrunk();
			}	

			// PRUNE TO TIME
			if (param.prune_to_time) {
				double start = (param.prune_to_time_values)[0];
				double stop = (param.prune_to_time_values)[1];
				treelist[i].pruneToTime(start,stop);
			}				

			// PAD MIGRATION EVENTS
			if (param.pad_migration_events) {
				treelist[i].padMigrationEvents();
			}	

			// COLLAPSE LABELS
			if (param.collapse_labels) {
				treelist[i].collapseLabels();
			}	
			
			// ROTATE
			if (param.rotate) {
				double deg = (param.rotate_values)[0];
				treelist[i].rotateLoc(deg);
			}
			
			// ACCUMULATE
			if (param.accumulate) {
				treelist[i].accumulateLoc();
			}				
			
			// ADD TAIL
			if (param.add_tail) {
				double setback = (param.add_tail_values)[0];
				treelist[i].addTail(setback);
			}	
			
			// SET COORDS
			if (param.ordering) {
				treelist[i].setCoords(param.ordering_values);
			}				
		
		}

	}

}

/* go through problist and treelist and print highest posterior probability tree */
void IO::printTree() {

	if (param.print_tree || param.print_circular_tree) {

		string outputFile = outputPrefix + ".rules";
		cout << "Printing tree with highest posterior probability to " << outputFile << endl;		
		int index = getBestTree();
		
		if (!param.ordering && !param.print_circular_tree) {
			treelist[index].printRuleList(outputFile, false);
		}
		else if (param.ordering && !param.print_circular_tree) {
			treelist[index].printRuleListWithOrdering(outputFile,param.ordering_values);
		}
		else if (!param.ordering && param.print_circular_tree) {
			treelist[index].printRuleList(outputFile, true);
		}		
		
	}

	if (param.print_all_trees) {

		cout << "Printing trees with to trees/ directory" << endl;
		
		for (int i = 0; i < treelist.size(); i++) {

			stringstream ss;
			ss << i;
			string outputFile = "trees/" + outputPrefix + "_" + ss.str() + ".rules";
			
			ofstream outStream;
			outStream.open( outputFile.c_str(),ios::out);
			outStream.close();
			
			CoalescentTree ct = treelist[i];
					
			if (!param.ordering) {
				ct.printRuleList(outputFile, false);
			}
			else {
				ct.printRuleListWithOrdering(outputFile,param.ordering_values);
			}			
			
		}
			
	}


}

/* go through treelist and summarize coalescent statistics */
void IO::printStatistics() {

	if (param.summary()) {

		/* initializing output stream */
		string outputFile = outputPrefix + ".stats";
		ofstream outStream;
		outStream.open( outputFile.c_str(),ios::app);
		
		outStream << "statistic\tlower\tmean\tupper" << endl; 
		
		set<string>::const_iterator is;
		set<string>::const_iterator js;
		set<string> lset = treelist[0].getLabelSet();		
		
		// TMRCA  //////////////
		if (param.summary_tmrca) {
			cout << "Printing TMRCA summary to " << outputFile << endl;
			Series s;
			for (int i = 0; i < treelist.size(); i++) {
				CoalescentTree ct = treelist[i];
				double n = ct.getTMRCA();
				s.insert(n);
			}
			outStream << "tmrca" << "\t";
			outStream << s.quantile(0.025) << "\t" << s.mean() << "\t" << s.quantile(0.975) << endl;
		}
		
		// LENGTH  //////////////
		if (param.summary_length) {
			cout << "Printing length summary to " << outputFile << endl;
			Series s;
			for (int i = 0; i < treelist.size(); i++) {
				CoalescentTree ct = treelist[i];
				double n = ct.getLength();
				s.insert(n);
			}
			outStream << "length" << "\t";
			outStream << s.quantile(0.025) << "\t" << s.mean() << "\t" << s.quantile(0.975) << endl;
		}		

		// ROOT PROPORTIONS //////////////
		if (param.summary_root_proportions) {
			cout << "Printing root proportion summary to " << outputFile << endl;
			for (is = lset.begin(); is != lset.end(); ++is) {
				Series s;
				for (int i = 0; i < treelist.size(); i++) {
					CoalescentTree ct = treelist[i];
					double n = ct.getRootLabelPro(*is);
					s.insert(n);
				}
				outStream << "rootpro_" << *is << "\t";
				outStream << s.quantile(0.025) << "\t" << s.mean() << "\t" << s.quantile(0.975) << endl;		
			}
		}

		// LABEL PROPORTIONS //////////////
		if (param.summary_proportions) {
			cout << "Printing trunk proportion summary to " << outputFile << endl;
			for (is = lset.begin(); is != lset.end(); ++is) {
				Series s;
				for (int i = 0; i < treelist.size(); i++) {
					CoalescentTree ct = treelist[i];
					double n = ct.getLabelPro(*is);
					s.insert(n);
				}
				outStream << "pro_" << *is << "\t";
				outStream << s.quantile(0.025) << "\t" << s.mean() << "\t" << s.quantile(0.975) << endl;		
			}
		}
	
		// COALESCENCE /////////////////////
		if (param.summary_coal_rates) {
			cout << "Printing coalescent summary to " << outputFile << endl;	
			if (lset.size()>1) {
				for (is = lset.begin(); is != lset.end(); ++is) {
					Series s;
					for (int i = 0; i < treelist.size(); i++) {
						double n = treelist[i].getCoalRate(*is);
						s.insert(n);
					}
					outStream << "coal_" << *is << "\t";
					outStream << s.quantile(0.025) << "\t" << s.mean() << "\t" << s.quantile(0.975) << endl;
				}
			}
			else {
				Series s;
				for (int i = 0; i < treelist.size(); i++) {
					double n = treelist[i].getCoalRate();
					s.insert(n);
				}
				outStream << "coal\t";
				outStream << s.quantile(0.025) << "\t" << s.mean() << "\t" << s.quantile(0.975) << endl;
			}
		}
		
		// MIGRATION ///////////////////////
		if (param.summary_mig_rates) {		
			cout << "Printing migration summary to " << outputFile << endl;
			
			Series s;
			for (int i = 0; i < treelist.size(); i++) {
				double n = treelist[i].getMigRate();
				s.insert(n);
			}
			outStream << "mig_all\t";
			outStream << s.quantile(0.025) << "\t" << s.mean() << "\t" << s.quantile(0.975) << endl;
						
			for (is = lset.begin(); is != lset.end(); ++is) {
				for (js = lset.begin(); js != lset.end(); ++js) {
					string from = *is;
					string to = *js;
					if (from != to) {
						Series s;
						for (int i = 0; i < treelist.size(); i++) {
							double n = treelist[i].getMigRate(from,to);
							s.insert(n);
						}
						outStream << "mig_" << from << "_" << to << "\t";
						outStream << s.quantile(0.025) << "\t" << s.mean() << "\t" << s.quantile(0.975) << endl;
		
					}
				}	
			}
		}

		// SUBS RATE  //////////////
		if (param.summary_sub_rates) {
			cout << "Printing substitution rate summary to " << outputFile << endl;
			Series s;
			for (int i = 0; i < treelist.size(); i++) {
				CoalescentTree ct = treelist[i];
				double n = ct.getMeanRate();
				s.insert(n);
			}
			outStream << "subrate" << "\t";
			outStream << s.quantile(0.025) << "\t" << s.mean() << "\t" << s.quantile(0.975) << endl;
		}	

		// DIVERSITY  //////////////
		if (param.summary_diversity) {
			cout << "Printing diversity summary to " << outputFile << endl;
			if (lset.size()>1) {
				for (is = lset.begin(); is != lset.end(); ++is) {
					Series s;
					for (int i = 0; i < treelist.size(); i++) {
						double n = treelist[i].getDiversity(*is);
						s.insert(n);
					}
					outStream << "div_" << *is << "\t";
					outStream << s.quantile(0.025) << "\t" << s.mean() << "\t" << s.quantile(0.975) << endl;
				}
			}
			else {
				Series s;
				for (int i = 0; i < treelist.size(); i++) {
					double n = treelist[i].getDiversity();
					s.insert(n);
				}
				outStream << "div" << "\t";
				outStream << s.quantile(0.025) << "\t" << s.mean() << "\t" << s.quantile(0.975) << endl;
			}
		}	
		
		// FST  //////////////
		if (param.summary_fst) {
			cout << "Printing FST summary to " << outputFile << endl;
			Series s;
			for (int i = 0; i < treelist.size(); i++) {
				CoalescentTree ct = treelist[i];
				double n = ct.getFst();
				s.insert(n);
			}
			outStream << "fst" << "\t";
			outStream << s.quantile(0.025) << "\t" << s.mean() << "\t" << s.quantile(0.975) << endl;
		}	
		
		// TAJIMA'S D  //////////////
		if (param.summary_tajima_d) {
			cout << "Printing Tajima's D summary to " << outputFile << endl;
			Series s;
			for (int i = 0; i < treelist.size(); i++) {
				CoalescentTree ct = treelist[i];
				double n = ct.getTajimaD();
				s.insert(n);
			}
			outStream << "tajimad" << "\t";
			outStream << s.quantile(0.025) << "\t" << s.mean() << "\t" << s.quantile(0.975) << endl;
		}			
		
		// PERSISTENCE ///////////////////////
		if (param.summary_persistence) {		
			cout << "Printing persistence summary to " << outputFile << endl;
			
			Series s;
			for (int i = 0; i < treelist.size(); i++) {
				double n = treelist[i].getPersistence();
				s.insert(n);
			}
			double avg = s.mean();
			s.clear();
			for (int i = 0; i < treelist.size(); i++) {
				double n = treelist[i].getPersistenceQuantile(0.25);
				s.insert(n);
			}
			double lower = s.mean();
			s.clear();	
			for (int i = 0; i < treelist.size(); i++) {
				double n = treelist[i].getPersistenceQuantile(0.75);
				s.insert(n);
			}
			double upper = s.mean();
			s.clear();			
			
			outStream << "persistence_all\t";
			outStream << lower << "\t" << avg << "\t" << upper << endl;
						
			for (is = lset.begin(); is != lset.end(); ++is) {
				string label = *is;

				Series s;
				for (int i = 0; i < treelist.size(); i++) {
					double n = treelist[i].getPersistence(label);
					s.insert(n);
				}
				double avg = s.mean();
				s.clear();
				for (int i = 0; i < treelist.size(); i++) {
					double n = treelist[i].getPersistenceQuantile(0.25, label);
					s.insert(n);
				}
				double lower = s.mean();
				s.clear();	
				for (int i = 0; i < treelist.size(); i++) {
					double n = treelist[i].getPersistenceQuantile(0.75, label);
					s.insert(n);
				}
				double upper = s.mean();
				s.clear();		

				outStream << "persistence_" << label << "\t";
				outStream << lower << "\t" << avg << "\t" << upper << endl;
			}
		}		
		
		// Diffusion coefficient  //////////////
		if (param.summary_diffusion_coefficient) {

			double lowerQuantile = 0.25;
			double upperQuantile = 0.75;		
		
			cout << "Printing coefficients of diffusion to " << outputFile << endl;
			Series s;
			for (int i = 0; i < treelist.size(); i++) {
				CoalescentTree ct = treelist[i];
				double n = ct.getDiffusionCoefficient();
				s.insert(n);
			}
			outStream << "diffusionCoefficient" << "\t";
			outStream << s.quantile(lowerQuantile) << "\t" << s.mean() << "\t" << s.quantile(upperQuantile) << endl;
			
			s.clear();
			for (int i = 0; i < treelist.size(); i++) {
				CoalescentTree ct = treelist[i];
				double n = ct.getDiffusionCoefficientTrunk();
				s.insert(n);
			}
			outStream << "diffusionCoefficientTrunk" << "\t";
			outStream << s.quantile(lowerQuantile) << "\t" << s.mean() << "\t" << s.quantile(upperQuantile) << endl;
			
			s.clear();
			for (int i = 0; i < treelist.size(); i++) {
				CoalescentTree ct = treelist[i];
				double n = ct.getDiffusionCoefficientSideBranches();
				s.insert(n);
			}
			outStream << "diffusionCoefficientSideBranches" << "\t";
			outStream << s.quantile(lowerQuantile) << "\t" << s.mean() << "\t" << s.quantile(upperQuantile) << endl;
			
			s.clear();
			for (int i = 0; i < treelist.size(); i++) {
				CoalescentTree ct = treelist[i];
				double n = ct.getDiffusionCoefficientInternalBranches();
				s.insert(n);
			}
			outStream << "diffusionCoefficientInternalBranches" << "\t";
			outStream << s.quantile(lowerQuantile) << "\t" << s.mean() << "\t" << s.quantile(upperQuantile) << endl;			
					
			s.clear();
			for (int i = 0; i < treelist.size(); i++) {
				CoalescentTree ct = treelist[i];
				double n = ct.getDiffusionCoefficientTrunk();
				double d = ct.getDiffusionCoefficientSideBranches();
				s.insert(n/d);
			}
			outStream << "diffusionCoefficientTSRatio" << "\t";
			outStream << s.quantile(lowerQuantile) << "\t" << s.mean() << "\t" << s.quantile(upperQuantile) << endl;		
			
			s.clear();
			for (int i = 0; i < treelist.size(); i++) {
				CoalescentTree ct = treelist[i];
				double n = ct.getDiffusionCoefficientTrunk();
				double d = ct.getDiffusionCoefficientInternalBranches();
				s.insert(n/d);
			}
			outStream << "diffusionCoefficientTIRatio" << "\t";
			outStream << s.quantile(lowerQuantile) << "\t" << s.mean() << "\t" << s.quantile(upperQuantile) << endl;					
			
		}	
		
		// Drift   //////////////
		if (param.summary_drift_rate) {
		
			double lowerQuantile = 0.25;
			double upperQuantile = 0.75;
		
			cout << "Printing drift rate to " << outputFile << endl;
			Series s;
			for (int i = 0; i < treelist.size(); i++) {
				CoalescentTree ct = treelist[i];
				double n = ct.getDriftRate();
				s.insert(n);
			}
			outStream << "driftRate" << "\t";
			outStream << s.quantile(lowerQuantile) << "\t" << s.mean() << "\t" << s.quantile(upperQuantile) << endl;
			
			s.clear();
			for (int i = 0; i < treelist.size(); i++) {
				CoalescentTree ct = treelist[i];
				double n = ct.getDriftRateTrunk();
				s.insert(n);
			}
			outStream << "driftRateTrunk" << "\t";
			outStream << s.quantile(lowerQuantile) << "\t" << s.mean() << "\t" << s.quantile(upperQuantile) << endl;
			
			s.clear();
			for (int i = 0; i < treelist.size(); i++) {
				CoalescentTree ct = treelist[i];
				double n = ct.getDriftRateSideBranches();
				s.insert(n);
			}
			outStream << "driftRateSideBranches" << "\t";
			outStream << s.quantile(lowerQuantile) << "\t" << s.mean() << "\t" << s.quantile(upperQuantile) << endl;
			
			s.clear();
			for (int i = 0; i < treelist.size(); i++) {
				CoalescentTree ct = treelist[i];
				double n = ct.getDriftRateInternalBranches();
				s.insert(n);
			}
			outStream << "driftRateInternalBranches" << "\t";
			outStream << s.quantile(lowerQuantile) << "\t" << s.mean() << "\t" << s.quantile(upperQuantile) << endl;		

			s.clear();
			for (int i = 0; i < treelist.size(); i++) {
				CoalescentTree ct = treelist[i];
				double n = ct.getDriftRateTrunk();
				double d = ct.getDriftRateSideBranches();
				s.insert(n/d);
			}
			outStream << "driftRateTSRatio" << "\t";
			outStream << s.quantile(lowerQuantile) << "\t" << s.mean() << "\t" << s.quantile(upperQuantile) << endl;	
			
			s.clear();
			for (int i = 0; i < treelist.size(); i++) {
				CoalescentTree ct = treelist[i];
				double n = ct.getDriftRateTrunk();
				double d = ct.getDriftRateInternalBranches();
				s.insert(n/d);
			}
			outStream << "driftRateTIRatio" << "\t";
			outStream << s.quantile(lowerQuantile) << "\t" << s.mean() << "\t" << s.quantile(upperQuantile) << endl;					
			
		}			


		outStream.close();
	
	}

}

/* go through treelist and calculate skyline statistics */
void IO::printSkylines() {

	if (param.skyline()) {

		/* initializing output stream */
		string outputFile = outputPrefix + ".skylines";
		ofstream outStream;
		outStream.open( outputFile.c_str(),ios::app);
		
		outStream << "statistic\ttime\tlower\tmean\tupper" << endl; 
		
		set<string>::const_iterator is;
		set<string>::const_iterator js;
		set<string> lset = treelist[0].getLabelSet();	

		double start = param.skyline_values[0];
		double stop = param.skyline_values[1];
		double step = param.skyline_values[2];

		// TMRCA /////////////////////
		if (param.skyline_tmrca) {
			cout << "Printing TMRCA skyline to " << outputFile << endl;
			for (double t = start; t + step <= stop; t += step) {
				Series s;
				for (int i = 0; i < treelist.size(); i++) {
					CoalescentTree ct = treelist[i];
					ct.timeSlice(t + step / (double) 2);
					double n = ct.getTMRCA();
					s.insert(n);
				}
				outStream << "tmrca" << "\t";
				outStream << t + step / (double) 2 << "\t";
				outStream << s.quantile(0.025) << "\t" << s.mean() << "\t" << s.quantile(0.975) << endl;
			}
		}
		
		// LENGTH /////////////////////
		if (param.skyline_length) {
			cout << "Printing length skyline to " << outputFile << endl;
			for (double t = start; t + step <= stop; t += step) {
				Series s;
				for (int i = 0; i < treelist.size(); i++) {
					CoalescentTree ct = treelist[i];
					ct.timeSlice(t + step / (double) 2);
					double n = ct.getLength();
					s.insert(n);
				}
				outStream << "length" << "\t";
				outStream << t + step / (double) 2 << "\t";
				outStream << s.quantile(0.025) << "\t" << s.mean() << "\t" << s.quantile(0.975) << endl;
			}
		}		

		// LABEL PROPORTIONS /////////////////////
		if (param.skyline_proportions) {
			cout << "Printing proportions skyline to " << outputFile << endl;
			
			for (is = lset.begin(); is != lset.end(); ++is) {
				for (double t = start; t + step <= stop; t += step) {
			
					Series s;
					for (int i = 0; i < treelist.size(); i++) {
						CoalescentTree ct = treelist[i];
						ct.trimEnds(t,t+step);
					//	ct.pruneToTrunk();
						double n = ct.getLabelPro(*is);
						s.insert(n);
					}
					outStream << "pro_" << *is << "\t";
					outStream << t + step / (double) 2 << "\t";
					outStream << s.quantile(0.025) << "\t" << s.mean() << "\t" << s.quantile(0.975) << endl;
					
				}
			}
		}
	
		// COALESCENCE /////////////////////
		if (param.skyline_coal_rates) {
			cout << "Printing coalescent skyline to " << outputFile << endl;
			
			for (is = lset.begin(); is != lset.end(); ++is) {
				for (double t = start; t + step <= stop; t += step) {
			
					Series s;
					for (int i = 0; i < treelist.size(); i++) {
						CoalescentTree ct = treelist[i];
						ct.trimEnds(t,t+step);
						double n = ct.getCoalRate(*is);
						s.insert(n);
					}
					outStream << "coal_" << *is << "\t";
					outStream << t + step / (double) 2 << "\t";
					outStream << s.quantile(0.025) << "\t" << s.mean() << "\t" << s.quantile(0.975) << endl;
					
				}
			}
		}
		
		// MIGRATION ///////////////////////
		if (param.skyline_mig_rates) {		
			cout << "Printing migration skyline to " << outputFile << endl;
			
			for (double t = start; t + step <= stop; t += step) {
				Series s;
				for (int i = 0; i < treelist.size(); i++) {
					CoalescentTree ct = treelist[i];
					ct.trimEnds(t,t+step);				
					double n = ct.getMigRate();
					s.insert(n);
				}
				outStream << "mig_all\t";
				outStream << t + step / (double) 2 << "\t";
				outStream << s.quantile(0.025) << "\t" << s.mean() << "\t" << s.quantile(0.975) << endl;
			}
			
			for (is = lset.begin(); is != lset.end(); ++is) {
				for (js = lset.begin(); js != lset.end(); ++js) {	
					string from = *is;
					string to = *js;
					if (from != to) {
						for (double t = start; t + step <= stop; t += step) {
		
							Series s;
							for (int i = 0; i < treelist.size(); i++) {
								CoalescentTree ct = treelist[i];
								ct.trimEnds(t,t+step);
								double n = ct.getMigRate(from,to);
								s.insert(n);
							}
							outStream << "mig_" << from << "_" << to << "\t";
							outStream << t + step / (double) 2 << "\t";
							outStream << s.quantile(0.025) << "\t" << s.mean() << "\t" << s.quantile(0.975) << endl;
		
						}
					}
				}	
			}
		}		
		
		// PROPORTION HISTORY FROM TIPS ///////////////////////
		if (param.skyline_pro_history_from_tips) {		
			cout << "Printing proportion history skyline to " << outputFile << endl;
			for (is = lset.begin(); is != lset.end(); ++is) {
				for (js = lset.begin(); js != lset.end(); ++js) {			
					string startingLabel = *is;
					string endingLabel = *js;
					for (double t = start; t + step <= stop; t += step) {
		
						Series s;
						for (int i = 0; i < treelist.size(); i++) {
							CoalescentTree ct = treelist[i];
							double n = ct.getLabelProFromTips(endingLabel, t, startingLabel);
							s.insert(n);
						}
						outStream << "prohist_" << startingLabel << "_" << endingLabel << "\t";
						outStream << t + step / (double) 2 << "\t";
						outStream << s.quantile(0.025) << "\t" << s.mean() << "\t" << s.quantile(0.975) << endl;
	
					}
				}
			}
		}				
		
		// DIVERSITY /////////////////////
		if (param.skyline_diversity) {
			cout << "Printing diversity skyline to " << outputFile << endl;
			for (double t = start; t + step <= stop; t += step) {
				Series s;
				for (int i = 0; i < treelist.size(); i++) {
					CoalescentTree ct = treelist[i];
					ct.timeSlice(t + step / (double) 2);
					double n = ct.getDiversity();
					s.insert(n);
				}
				outStream << "div" << "\t";
				outStream << t + step / (double) 2 << "\t";
				outStream << s.quantile(0.025) << "\t" << s.mean() << "\t" << s.quantile(0.975) << endl;
			}
		}	
		
		// FST /////////////////////
		if (param.skyline_fst) {
			cout << "Printing FST skyline to " << outputFile << endl;
			for (double t = start; t + step <= stop; t += step) {
				Series s;
				for (int i = 0; i < treelist.size(); i++) {
					CoalescentTree ct = treelist[i];
					ct.timeSlice(t + step / (double) 2);
					double n = ct.getFst();
					s.insert(n);
				}
				outStream << "fst" << "\t";
				outStream << t + step / (double) 2 << "\t";
				outStream << s.quantile(0.025) << "\t" << s.mean() << "\t" << s.quantile(0.975) << endl;
			}
		}
		
		// TAJIMA D /////////////////////
		if (param.skyline_tajima_d) {
			cout << "Printing Tajima D skyline to " << outputFile << endl;
			for (double t = start; t + step <= stop; t += step) {
				Series s;
				for (int i = 0; i < treelist.size(); i++) {
					CoalescentTree ct = treelist[i];
					ct.timeSlice(t + step / (double) 2);
					double n = ct.getTajimaD();
					s.insert(n);
				}
				outStream << "tajimad" << "\t";
				outStream << t + step / (double) 2 << "\t";
				outStream << s.quantile(0.025) << "\t" << s.mean() << "\t" << s.quantile(0.975) << endl;
			}
		}		
		
		// TIME TO FIX /////////////////////
		if (param.skyline_timetofix) {
			cout << "Printing fixation time skyline to " << outputFile << endl;
			for (double t = start; t + step <= stop; t += step) {
				Series s;
				for (int i = 0; i < treelist.size(); i++) {
					CoalescentTree ct = treelist[i];
					double present = t + step / (double) 2;
					ct.trunkSlice(present);
					double future = ct.getPresentTime();
					s.insert(future-present);
				}
				outStream << "timetofix" << "\t";
				outStream << t + step / (double) 2 << "\t";
				outStream << s.quantile(0.025) << "\t" << s.mean() << "\t" << s.quantile(0.975) << endl;
			}
		}		
		

		// X LOCATION /////////////////////
		if (param.skyline_xmean) {
			cout << "Printing X mean skyline to " << outputFile << endl;
			for (double t = start; t + step <= stop; t += step) {
				Series s;
				for (int i = 0; i < treelist.size(); i++) {
					CoalescentTree ct = treelist[i];
					ct.timeSlice(t);
					double n = ct.getMeanX();
					s.insert(n);
				}
				outStream << "xmean" << "\t";
				outStream << t << "\t";
				outStream << s.quantile(0.25) << "\t" << s.mean() << "\t" << s.quantile(0.75) << endl;
			}
		}	
		
		// Y LOCATION /////////////////////
		if (param.skyline_ymean) {
			cout << "Printing Y mean skyline to " << outputFile << endl;
			for (double t = start; t + step <= stop; t += step) {
				Series s;
				for (int i = 0; i < treelist.size(); i++) {
					CoalescentTree ct = treelist[i];
					ct.timeSlice(t);
					double n = ct.getMeanY();
					s.insert(n);
				}
				outStream << "ymean" << "\t";
				outStream << t << "\t";
				outStream << s.quantile(0.25) << "\t" << s.mean() << "\t" << s.quantile(0.75) << endl;
			}
		}
				
		// X DRIFT /////////////////////
		if (param.skyline_xdrift) {
			cout << "Printing X drift skyline to " << outputFile << endl;
			for (double t = start; t + step <= stop; t += step) {
				Series s;
				for (int i = 0; i < treelist.size(); i++) {
					CoalescentTree bt = treelist[i];
					bt.timeSlice(t);
					double b = bt.getMeanX();
					CoalescentTree at = treelist[i];
					at.timeSlice(t-step);
					double a = at.getMeanX();
					s.insert(b-a);
				}
				outStream << "xdrift" << "\t";
				outStream << t << "\t";
				outStream << s.quantile(0.25) << "\t" << s.mean() << "\t" << s.quantile(0.75) << endl;
			}
		}			
		
		// RATE /////////////////////
		if (param.skyline_ratemean) {
			cout << "Printing rate mean skyline to " << outputFile << endl;
			for (double t = start; t + step <= stop; t += step) {
				Series s;
				for (int i = 0; i < treelist.size(); i++) {
					CoalescentTree ct = treelist[i];
					ct.timeSlice(t + step / (double) 2);
					double n = ct.getMeanRate();
					s.insert(n);
				}
				outStream << "ratemean" << "\t";
				outStream << t + step / (double) 2 << "\t";
				outStream << s.quantile(0.25) << "\t" << s.mean() << "\t" << s.quantile(0.75) << endl;
			}
		}	
		
		// X LOCATION TRUNK DIFFERENCE /////////////////////
		if (param.skyline_xtrunkdiff) {
			cout << "Printing X trunk different to " << outputFile << endl;
			for (double t = start; t + step <= stop; t += step) {
				Series s;
				for (int i = 0; i < treelist.size(); i++) {
					CoalescentTree ct = treelist[i];
					ct.timeSlice(t);
					double all = ct.getMeanX();
					ct = treelist[i];
					ct.pruneToTrunk();
					ct.timeSlice(t);
					double trunk = ct.getMeanX();
					s.insert(trunk-all);
				}
				outStream << "xtrunkdiff" << "\t";
				outStream << t << "\t";
				outStream << s.quantile(0.025) << "\t" << s.mean() << "\t" << s.quantile(0.975) << endl;
			}
		}			
		
		// LOC SAMPLE /////////////////////
		if (param.skyline_locsample) {
			cout << "Printing loc sample skyline to " << outputFile << endl;
			for (double t = start; t + step <= stop; t += step) {
				Series s;
				outStream << "locsample" << "\t" << t + step / (double) 2;
				for (int i = 0; i < treelist.size(); i++) {
					CoalescentTree ct = treelist[i];
					ct.timeSlice(t + step / (double) 2);
					vector<double> xlocs = ct.getTipsX();
					vector<double> ylocs = ct.getTipsY();
					int length = xlocs.size();
					if (length > 50000) { length = 50000; }
					for (int i = 0; i < length; i++) {
						double x = xlocs[i];
						double y = ylocs[i];
						if (x < 0.001 && x > -0.001) { x = 0.0; }
						if (y < 0.001 && y > -0.001) { y = 0.0; }
						outStream << "\t{" << x << "," << y << "}";
					}
				}
				outStream << endl;
			}
		}			
		
		// LOC GRID /////////////////////
		if (param.skyline_locgrid) {
			cout << "Printing loc grid skyline to " << outputFile << endl;
			for (double t = start; t + step <= stop; t += step) {
				
				outStream << "locgrid" << "\t" << t;
				
				multiset< vector<double> > locset;
				
				for (int i = 0; i < treelist.size(); i++) {
					CoalescentTree ct = treelist[i];
					ct.timeSlice(t + step / (double) 2);
					vector<double> xlocs = ct.getTipsX();
					vector<double> ylocs = ct.getTipsY();
					for (int i = 0; i < xlocs.size(); i++) {
						vector<double> v;
						v.push_back(xlocs[i]);
						v.push_back(ylocs[i]);
						locset.insert(v);
					}
				
				}
				
				double step = 0.25;
				for (double x = -2.0; x <= 50.0; x += step) {
					for (double y = -6.0; y <= 6.0; y += step) {
					
						int count = 0;
						for (multiset< vector<double> >::iterator is = locset.begin(); is != locset.end(); is++ ) {
							double thisx = (*is).at(0);
							double thisy = (*is).at(1);
							if (thisx < x + 0.5*step && thisx > x - 0.5*step && thisy < y + 0.5*step && thisy > y - 0.5*step) {
								count++;
							}
						}
						outStream << "\t" << count;
				
					}
				}
				
				outStream << endl;
			}
		}					

		// 1D DRIFT RATE FROM TIPS /////////////////////
		if (param.skyline_drift_rate_from_tips) {
			cout << "Printing skyline of 1D drift rate from tips " << outputFile << endl;
			for (double t = start; t + step <= stop; t += step) {
				Series s;
				for (int i = 0; i < treelist.size(); i++) {
					CoalescentTree ct = treelist[i];
					double n = ct.get1DRateFromTips(t, step);	// need to account for undefined cases
					s.insert(n);
				}
				outStream << "1dratefromtips" << "\t";
				outStream << t + step / (double) 2 << "\t";
				outStream << s.quantile(0.25) << "\t" << s.mean() << "\t" << s.quantile(0.75) << endl;
			}
		}		
		
		// 2D DRIFT RATE FROM TIPS /////////////////////
		if (param.skyline_drift_rate_from_tips) {
			cout << "Printing skyline of 2D drift rate from tips " << outputFile << endl;
			for (double t = start; t + step <= stop; t += step) {
				Series s;
				for (int i = 0; i < treelist.size(); i++) {
					CoalescentTree ct = treelist[i];
					double n = ct.get2DRateFromTips(t, step);	// need to account for undefined cases
					s.insert(n);
				}
				outStream << "2dratefromtips" << "\t";
				outStream << t + step / (double) 2 << "\t";
				outStream << s.quantile(0.25) << "\t" << s.mean() << "\t" << s.quantile(0.75) << endl;
			}
		}			
				
		outStream.close();
	
	}

}

/* go through treelist and summarize tip statistics */
void IO::printTips() {

	if (param.tips()) {

		/* initializing output stream */
		string outputFile = outputPrefix + ".tips";
		ofstream outStream;
		outStream.open( outputFile.c_str(),ios::app);
		
		outStream << "statistic\tname\tlabel\ttime\tlower\tmean\tupper" << endl; 
		
		/* get vector of tip names */
		vector<string> tipNames = treelist[0].getTipNames();
		
		// TIME TO TRUNK //////////////
		if (param.tips_time_to_trunk) {
			cout << "Printing time to trunk for tips to " << outputFile << endl;
			for (int n = 0; n < tipNames.size(); n++) {
				string tip = tipNames[n];
				Series s;
				for (int i = 0; i < treelist.size(); i++) {
					double n = treelist[i].timeToTrunk(tip);
					s.insert(n);
				}
				outStream << "time_to_trunk" << "\t";
				outStream << tip << "\t";
				outStream << treelist[0].getLabel(tip) << "\t";
				outStream << treelist[0].getTime(tip) << "\t";				
				outStream << s.quantile(0.025) << "\t" << s.mean() << "\t" << s.quantile(0.975) << endl;		
			}
		}
		
		// X LOC HISTORY //////////////
		if (param.x_loc_history) {
		
			cout << "Printing x loc history for tips to " << outputFile << endl;
			double start = param.x_loc_history_values[0];
			double stop = param.x_loc_history_values[1];
			double step = param.x_loc_history_values[2];
			
			for (int n = 0; n < tipNames.size(); n++) {
			
				string tip = tipNames[n];
				outStream << "x_loc_history" << "\t";
				outStream << tip << "\t";
								
				vector<CoalescentTree> subtreelist;
				for (vector<CoalescentTree>::iterator it = treelist.begin(); it != treelist.end(); it++ ) {
					CoalescentTree ct = *it;
					ct.pruneToName(tip);
		//			ct.assignLocation();
					subtreelist.push_back(ct);
				}		
									
				double endTime = subtreelist[0].getPresentTime();
									
				for (double t = start; t <= endTime; t += step) {
				
					Series sx;
					for (vector<CoalescentTree>::iterator it = subtreelist.begin(); it != subtreelist.end(); it++ ) {
						CoalescentTree ct = *it;
						ct.timeSlice(t);
						double x = ct.getMeanX();
						sx.insert(x);
					}		
					double xmean = sx.quantile(0.5);
					double xlower = sx.quantile(0.25);
					double xupper = sx.quantile(0.75);	
			//		double xmean = sx.mean();
			//		double xlower = sx.sdrange(-1);
			//		double xupper = sx.sdrange(1);				
					if (t < 0.0001 && t > -0.0001) { t = 0.0; }					
					if (xmean < 0.0001 && xmean > -0.0001) { xmean = 0.0; }
					if (xlower < 0.0001 && xlower > -0.0001) { xlower = 0.0; }
					if (xupper < 0.0001 && xupper > -0.0001) { xupper = 0.0; }					
					outStream << "\t{" << t << "," << xlower << "," << xmean << "," << xupper << "}";
					
				}
				
				outStream << endl;
				
			}
		}
		
		// Y LOC HISTORY //////////////
		if (param.y_loc_history) {
		
			cout << "Printing y loc history for tips to " << outputFile << endl;
			double start = param.y_loc_history_values[0];
			double stop = param.y_loc_history_values[1];
			double step = param.y_loc_history_values[2];
			
			for (int n = 0; n < tipNames.size(); n++) {
			
				string tip = tipNames[n];
				outStream << "y_loc_history" << "\t";
				outStream << tip << "\t";
								
				vector<CoalescentTree> subtreelist;
				for (vector<CoalescentTree>::iterator it = treelist.begin(); it != treelist.end(); it++ ) {
					CoalescentTree ct = *it;
					ct.pruneToName(tip);
				//	ct.assignLocation();
					subtreelist.push_back(ct);
				}		
									
				double endTime = subtreelist[0].getPresentTime();
									
				for (double t = start; t <= endTime; t += step) {
				
					Series sy;
					for (vector<CoalescentTree>::iterator it = subtreelist.begin(); it != subtreelist.end(); it++ ) {
						CoalescentTree ct = *it;
						ct.timeSlice(t);
						double y = ct.getMeanY();
						sy.insert(y);
					}		
					double ymean = sy.quantile(0.5);
					double ylower = sy.quantile(0.025);
					double yupper = sy.quantile(0.975);	
			//		double ymean = sy.mean();
			//		double ylower = sy.sdrange(-1);
			//		double yupper = sy.sdrange(1);				
					if (t < 0.0001 && t > -0.0001) { t = 0.0; }					
					if (ymean < 0.0001 && ymean > -0.0001) { ymean = 0.0; }
					if (ylower < 0.0001 && ylower > -0.0001) { ylower = 0.0; }
					if (yupper < 0.0001 && yupper > -0.0001) { yupper = 0.0; }					
					outStream << "\t{" << t << "," << ylower << "," << ymean << "," << yupper << "}";
					
				}
				
				outStream << endl;
				
			}
		}		
		
		outStream.close();
	
	}

}

/* go through treelist and summarize pair statistics */
void IO::printPairs() {
	
	if (param.pairs()) {
		
		/* initializing output stream */
		string outputFile = outputPrefix + ".pairs";
		ofstream outStream;
		outStream.open( outputFile.c_str(),ios::app);
		
		outStream << "statistic\tnameA\tnameB\tlower\tmean\tupper" << endl; 
		
		/* get vector of tip names */
		vector<string> tipNames = treelist[0].getTipNames();
		
		// PAIRWISE DIVERSITY //////////////
		if (param.pairs_diversity) {
		
			double timeDiff = param.pairs_diversity_values[0];
					
			cout << "Printing pairwise diversity to " << outputFile << endl;
			for (int nA = 0; nA < tipNames.size(); nA++) {
				for (int nB = nA + 1; nB < tipNames.size(); nB++) {
				
					string tipA = tipNames[nA];
					string tipB = tipNames[nB];
					
					double timeA = treelist[0].getTime(tipA);
					double timeB = treelist[0].getTime(tipB);
														
					if (abs(timeA - timeB) < timeDiff) {
									
						Series s;
						for (int i = 0; i < treelist.size(); i++) {
							double n = treelist[i].getDiversity(tipA, tipB);
							s.insert(n);
						}
						
						outStream << "diversity" << "\t";
						outStream << tipA << "\t";
						outStream << tipB << "\t";								
						outStream << s.quantile(0.025) << "\t" << s.mean() << "\t" << s.quantile(0.975) << endl;
					
					}
				
				}
			}
		}


	}
	
}

/* go through problist and treelist and return index of highest posterior probability tree */
int IO::getBestTree() {

	int index;
	if (problist.size() == treelist.size()) {
		
		double ll = problist[0];
		index = 0;
		for (int i = 1; i < problist.size(); i++) {
			if (problist[i] > ll) {
				ll = problist[i];
				index = i;
			}
		}
		
	}
	else {
		index = treelist.size() - 1;
	}
	
	return index;

}