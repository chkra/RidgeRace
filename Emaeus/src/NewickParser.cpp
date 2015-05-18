/*
 * Parser.cpp
 *
 *  Created on: Nov 8, 2012
 *      Author: ctusche
 */

#include <stdlib.h>
#include <iostream>
#include <sstream>

#include "Exception.h"
#include "Parser.h"
#include "Node.h"

std::string getSuffix(std::string old, std::string cur) {

	int now = old.size() - cur.size();
	std::string suff = old.substr(0, now);

	if (suff.size() > 10) {
		suff = suff.substr(suff.size() - 10, 10);
	}

	return suff;
}


void throwMissingChar(std::string line, std::string oldLine, char missing) {

	std::string tmp = getSuffix(oldLine, line);

	std::cerr << tmp << line.substr(0, 10) << std::endl;
	for (size_t i = 0; i < tmp.size(); i++)
		std::cerr << ' ';
	std::cerr << "^" << std::endl;

	std::ostringstream s;
	s << "ERROR parsing line! Expected ";
	s << missing;
	s << '\n';

	throw Exception(s.str());
}

void throwMissingChar(std::string line, std::string oldLine,
		std::string missing) {

	std::string tmp = getSuffix(oldLine, line);

	std::cerr << tmp << line.substr(0, 10) << std::endl;
	for (size_t i = 0; i < tmp.size(); i++)
		std::cerr << ' ';
	std::cerr << "^" << std::endl;

	std::ostringstream s;
	s << "ERROR parsing line! Expected ";
	for (size_t i = 0; i < missing.size(); i++) {
		s << missing[i] << "  ";
	}
	s << '\n';

	throw Exception(s.str());
}



void NewickParser::clip(std::string &line, size_t length) {
	if (line.size() < length) {
		throw "ERROR in clip: line to short!\n";
	}
	line = line.substr(length, line.size() - length);
}

void NewickParser::removeLeadingWhite(std::string &line) {
	size_t p = 0;
	while (p < line.size())
		if (line[p] == ' ' || line[p] == '\t')
			p++;
		else
			break;

	line = line.substr(p, line.size() - p);
}

bool NewickParser::isNum(char c) {
	return (c == 'E') || (c == 'e') || (c == '-') || (c == '.') || ((c > 47)
			& (c < 58));
}

std::string mySubstring(std::string in, size_t beg, size_t length) {
	std::string out = "";
	for (size_t i = beg; i < beg + length; i++) {
		out.push_back(in[i]);
	}
	return out;
}

float NewickParser::getLeadingNumber(std::string &line) {

	float f;

	size_t p = 0;
	while (p < line.size()) {
		if (isNum(line[p])) {
			p++;
		} else {
			break;
		}
	}

	f = atof(line.substr(0, p).c_str());
	line = line.substr(p, line.size() - p);
	return f;
}

bool NewickParser::isName(char c) {
	return c != '(' && c != ')' && c != ',' && c != ':' && c != ';' && c != '['
			&& c != ']';
}

std::string NewickParser::getLeadingName(std::string &line) {

	size_t p = 0; // for leading '
	while (p < line.size() && isName(line[p]))
		p++;

	std::string name;

	if (line[0] == '\'') {
		name = line.substr(1, p - 2).c_str();
	} else {
		name = line.substr(0, p).c_str();
	}
	line = line.substr(p, line.size() - p);
	return name;
}


Node* NewickParser::parse(std::string line) {
	//std::cout << "now parsing:\n" << line << std::endl;
	return iNode(line);
}

// tree = iNode
// iNode = (iNode, iNode):bLength | (iNode, iNode)name:bLength | leaf
// leaf = name:blength
Node* NewickParser::iNode(std::string &line) {

	std::string oldLine = line;

	removeLeadingWhite(line);

	Node* n = new Node();

	if (line[0] == '(') {

		//std::cout << "(";
		clip(line, 1);

		Node* leftChild;
		Node* rightChild;

		leftChild = iNode(line);
		n->children.push_back(leftChild);
		leftChild->father = n;
		//std::cout << ".";
		removeLeadingWhite(line);

		while (line[0] == ',') { // yes, this is a 'while' (>2 children!)
			clip(line, 1);
			rightChild = iNode(line);
			n->children.push_back(rightChild);
			rightChild->father = n;
			removeLeadingWhite(line);
		}

		if (line[0] != ')') {
			throwMissingChar(line, oldLine, ')');
		}
		clip(line, 1);

		// in this case there is a name after the internal node:
		// (a,b)c:0.2[&bla="foo"]
		if ((line[0] != ':') & (line[0] != '[') & (line[0] != ',')) {
			n->info = getLeadingName(line);
		}

	} else {
		//n->s = new Sample();
		n->info = getLeadingName(line);
		//std::cout << n->s->info << std::endl;
	}

	// this could be the end of the tree
	if (line[0] == ';') {
		clip(line, 1);
		n->dist = 0;
		return n;
	}

	// we could have a distance option
	if (line[0] == ':') {
		clip(line, 1);
		n->dist = getLeadingNumber(line);
	}

	// we could have additional features
	if (line[0] == '[') {
		// do nothing, simply ignorne it
		size_t p = line.find(']') + 1;
		clip(line, p);
	}

	// give every node a name if it does not have one
	// already
	ostringstream os;
	if (n->info == "") {
		os << 'i' << n->id;
		n->info = os.str();
	}

	return n;
}

NewickParser::NewickParser(string path) {

	std::ifstream is(path.c_str());

	if (!is.is_open()) {
		Exception e;
		e << "ERROR in NewickParser: file does not exist\n";
		e << "   path was ";
		e << path;
		e << '\n';
		throw e;
	}

	std::string line, tmp;
	size_t pos = 0;

	// -------- create tree ----------------
	while (!is.eof()) {
		getline(is, line);
		if (line.size() == 0)
			continue;
		if (line[0] == '>')
			continue;
		pos = line.find("(");
		if (pos != std::string::npos) {
			tree = parse(line.substr(pos, line.size() - pos));
			break;
		}
	}
}

Node* NewickParser::getTree() {
	return tree;
}
