/* node.h
Copyright 2009 Trevor Bedford <bedfordt@umich.edu>
Node class definition
This object represents a node in the CoalescentTree object, stores relevant attributes
*/

#ifndef NODE_H
#define NODE_H

#include <string>
using std::string;

class Node {

public:
	Node(int);						// creation requires a name
	
	// GET FUNCTIONS
	int getNumber();
	string getName();
	double getLength();
	double getTime();
	int getLabel();
	bool getLeaf();
	bool getTrunk();
	bool getInclude();

	// SET FUNCTIONS
	void setNumber(int);
	void setName(string);
	void setLength(double);
	void setTime(double);
	void setLabel(int);
	void setLeaf(bool);
	void setTrunk(bool);
	void setInclude(bool);
																			
private:
	int number;						// number of node, must be unique
	string name;					// name of node, doesn't have to exist
	double length;					// length of the branch leading into the node
	double time;					// date of the node	
	int label;						// numeric label associated with node
	bool leaf;						// is this node a leaf?
	bool trunk;						// is this node part of the trunk?
	bool include;					// include this node when performing calculations?


};

#endif