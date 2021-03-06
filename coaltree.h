/* coaltree.h
Copyright 2009-2012 Trevor Bedford <t.bedford@ed.ac.uk>
CoalescentTree class definition
This object stores and manipulates coalescent trees, rooted bifurcating trees with nodes mapped to time points
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

#ifndef CTREE_H
#define CTREE_H

#include <set>
using std::set;

#include <string>
using std::string;

#include <vector>
using std::vector;

#include "tree.hh"
#include "node.h"
#include "rng.h"

class CoalescentTree {

public:
	CoalescentTree(string);					// constructor, takes a parentheses string as input
											// starts with most recent sample set at time = 0
											// sharing a most recent sample time ensures skyline calculations 
											// will work properly

	// TREE MANIPULATION
	void pushTimesBack(double);				// push dates to agree with a most recent sample date at t
	void pushTimesBack(double,double);		// oldest sample and most recent sample	
	void reduceTips(double);				// reduces tree to ancestors of a subset of tips
	void renewTrunk(double);				// renews definition of trunk, working back from all recent tips
	void renewTrunkRandom(double);			// renews definition of trunk, working back from a random tip
	void pruneToTrunk();					// reduces tree to trunk
	void pruneToLabel(string);				// reduces tree to only include tips with specified label
	void pruneToTips(vector<string>);		// reduces tree to specified set of tips	
	void removeTips(vector<string>);		// removes specified set of tips from tree		
	void pruneToName(string);				// reduces CoalescentTree object to only include a particular tip	
	void pruneToTime(double,double);		// reduces CoalescentTree object to only include tips in a certain time frame
	void padMigrationEvents();				// pads ancestral state tree with migration events
	void collapseLabels();					// sets all labels in CoalescentTree to 1
	void trimEnds(double,double);			// reduces CoalescentTree object to only those nodes between
											// time start and time stop	
	void sectionTree(double,double,double);	// break tree up into sections
	void timeSlice(double);					// reduces CoalescentTree to all the ancestors of time slice
	void trunkSlice(double);				// removes from CoalescentTree all descendents of trunk at slice of time
	void leafSlice(double, double);			// reduces CoalescentTree to ancestors of leafs in a window of time
	void padTree();							// TODO: fix this
											// pads CoalescentTre with additional nodes at each coalescent event
											// included mainly for compatibility with TreePlot	
	void rotateLoc(double);					// rotate X&Y locations around origin
	void accumulateLoc();					// accumulate X&Y traits across tree (converts branch displacements to node traits)	
	void addTail(double);					// pads the tree with a node before the root, set backing by a specified amount of time	
	void setCoords(vector<string>);			// sets coords based on supplied vector of tip names	

	// TREE STRUCTURE
	void printTree();						// print indented tree with coalescent times			
	void printRuleList(string,bool);		// print to file name in Mathematica rule list format
											// used with Graphics primitives, specify whether linear or circular
	void printRuleListWithOrdering(string,vector<string>);	// print to file name in Mathematica rule list format
											// supply the tip ordering with vector of tip names											
	void printParen();						// TODO: migration events
											// print parentheses tree										

	// BASIC STATISTICS
	double getPresentTime();				// returns most recent time in tree
	double getRootTime();					// returns most ancient time in tree
	double getTMRCA();						// span of time in tree
//	int getMaxLabel();						// returns the highest label present
	int getLeafCount();						// returns the count of leaf nodes in tree
	int getNodeCount();						// returns the total number of nodes in tree

	// LABEL STATISTICS		
	double getLength();						// return total tree length
	double getLength(string);				// return length with this label
	double getLabelPro(string);				// return proportion of tree with label
	double getRootLabelPro(string);			// return proportion of root (0 or 1) with label	
	double getTrunkPro();					// proportion of tree that can trace its history from present day samples
	set<string> getLabelSet();				// return labelset
	double getLabelProFromTips(string,double);		// return proportion of tree with label	at time back from tips
	double getLabelProFromTips(string,double,string); // return proportion of tree with label conditioned on tip label
	
	// COALESCENT STATISTICS
	// problem with weight calculation for sectioned data
	int getCoalCount();						// total count of coalescent events on tree
	int getCoalCount(string);				// count of coalescent events involving label on tree	
	double getCoalWeight();					// total opportunity for coalescence on tree
	double getCoalWeight(string);			// total opportunity for coalescence on tree
	double getCoalRate();
	double getCoalRate(string);
	int getCoalCountTrunk();				// count of coalescent events involving one trunk and one side branch lineage
	double getCoalWeightTrunk();			// opportunity for coalescence on tree, scaling by n, rather than by n*(n-1)/2
	
	// MIGRATION STATISTICS
	int getMigCount();
	int getMigCount(string,string);	
	double getMigRate();			
	double getMigRate(string,string);
	double getPersistence();				// return average time from a tip to a node with different label
	double getPersistenceQuantile(double);	// return quantile across tips of persistence time
	double getPersistence(string);			// return average time from a tip with particular label to a node with different label
	double getPersistenceQuantile(double, string);	// return quantile across tips of persistence time
	
	// DIVERSITY STATISTICS	
	double getDiversity();					// return mean of (2 * time to common ancestor) for every pair of leaf nodes
	double getDiversity(string);			// diversity only involving a particular label
	double getDiversity(string, string);	// diversity for a particular pair of tips
	double getDiversityWithin();			// diversity where both samples have the same label
	double getDiversityBetween();			// diversity where both samples have different labels
	double getFst();						// Fst = (divBetween - divWithin) / divBetween
	double getTajimaD();					// return D = pi - S/a1, where pi is diversity, S is the total tree length, 
											// and a1 is a normalization factor

	// LOCATION STATISTICS
	double getMeanX();						// return mean X location across all tips in the tree
	double getMeanY();						// return mean Y location across all tips in the tree
	vector<double> getTipsX();				// returns a vector of double for X position of every tip in tree
	vector<double> getTipsY();				// returns a vector of double for Y position of every tip in tree	
	void assignLocation();
	double getDiffusionCoefficient();		// returns the coefficient of diffusion across the tree
	double getDiffusionCoefficientTrunk();
	double getDiffusionCoefficientSideBranches();		
	double getDiffusionCoefficientInternalBranches();	
	double getDriftRate();					// returns the rate of drift of location X across the tree
	double getDriftRateTrunk();
	double getDriftRateSideBranches();
	double getDriftRateInternalBranches();

	tree<Node>::iterator getNodeBackFromTip(tree<Node>::iterator, double);													
	double getXBackFromTip(tree<Node>::iterator, double);	
	double getYBackFromTip(tree<Node>::iterator, double);	
	double get1DRateFromTips(double, double);	// returns the average rate of 1D drift at double distance 
													// from tips in a window of size double	
	double get2DRateFromTips(double, double);	// returns the average rate of Euclidean drift at double distance 
													// from tips in a window of size double
														
	// RATE STATISTICS
	double getMeanRate();					// return mean rate across all tips in the tree	

	// TIP STATISTICS
	vector<string> getTipNames();			// returns vector of tip names
	double getTime(string);
	string getLabel(string);
	double timeToTrunk(string);				// time it takes for a named tip to coalesce with the trunk	
									
private:
	RNG rgen;								// random number generator
	tree<Node> nodetree;					// linked tree containing Node objects	
	set<string> labelset;					// set of all label names
										
	// HELPER FUNCTIONS
	string initialDigits(string);			// return initial digits in a string, 34ATZ -> 34, 3454 -> 0
	void reduce();							// goes through tree and removes inconsequential nodes	
	void peelBack();						// removes excess root from tree
	void adjustCoords();					// sets coords in Nodes to allow tree drawing	
	void adjustCircularCoords();			// sets coords in Nodes to allow tree unrooted drawing	
	int countDescendants(tree<Node>::iterator);		// counts the number of tips descended from a node
	int getMaxNumber();						// return largest number in tree
	int renumber(int);						// renumbers tree in preorder traversal starting from int 
											// returning 1 greater than the max in the tree
	tree<Node>::iterator findNode(int);		// return iterator to a Node in nodetree based upon matching number
	tree<Node>::iterator findNode(string);	// return iterator to a Node in nodetree based upon matching name	
											// if not found, returns iterator to end of tree
												
	tree<Node>::iterator commonAncestor(tree<Node>::iterator,tree<Node>::iterator);
	
};

#endif