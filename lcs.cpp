#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <utility>

using std::vector;
using std::string;

class SuffixTree {
	struct Node {
		bool contains = 0;					// if child node contains separator symbol
		int start = -1;						// starting index in string
		int end = -1;						// ending index in string
		vector<Node*> child;				// node childs
	};
private:
	Node* root;
	string s;								// string which will be added to the tree
	unsigned int separator_pos;						// position of separator symbol
	vector<std::pair<unsigned int, unsigned int>> cs;			// list of all common substrings

protected:
	// destructor additional method
	void deleteNode(Node* node) {
		for (unsigned int i = 0; i < node->child.size(); i++) {
			deleteNode(node->child[i]);
		}
		delete node;
	}

	// add child to %node% with start in %n% to the end of the string 
	void addSuffix(unsigned int n, Node* node) {
		Node* add = new Node;
		add->start = n;
		add->end = s.size() - 1;
		node->child.push_back(add);
	}

	// check if symbol of string at position %child[0]% equal to symbol at position %n%
	bool checkFirst(unsigned int n, Node* child) {
		return s[n] == s[child->start];
	}


	void splitEdge(unsigned int i, unsigned int len, Node* node) {
		Node* add = new Node;
		add->start = node->start + len;
		add->child = node->child;
		add->end = node->end;
		node->child.clear();
		node->child.push_back(add);
		node->end = len + node->start - 1;
		addSuffix(i, node);
	}


	void traverseNode(unsigned int n, Node* node) {
		unsigned int len = 0;
		for (unsigned int i = n; i < s.size(); i++) {
			if (len <= node->end - node->start) {
				if (s[i] == s[node->start + len]) ++len;
				else {
					splitEdge(i, len, node);
					return;
				}
			}
			else {
				bool common_in_child = 0;
				for (unsigned int p = 0; p < node->child.size(); p++) {
					if (checkFirst(i, node->child[p])) {
						common_in_child = 1;
						traverseNode(i, node->child[p]);
						return;
					}
				}
				if (!common_in_child) {
					addSuffix(i, node);
					return;
				}
			}
		}
	}

	// checks if node contains separator symbol in one of childs
	bool inspectNode(Node* node) {
		if (node->child.size() == 0) {
			if (separator_pos >= node->start && separator_pos <= node->end) {
				return 1;
			}
			else return 0;
		}
		else {
			for (unsigned int i = 0; i < node->child.size(); i++) {
				if (inspectNode(node->child[i])) node->contains = 1;
			}
			if (node->contains == 1) return 1;
			else return 0;
		}
	}

	// finds all common substrings in a single node
	void findCommon(Node* node, int start = 0, int end = -1) {
		start = node->start - (end - start + 1);
		end = node->end;
		bool fin = 1;
		for (unsigned int i = 0; i < node->child.size(); i++) {
			if (node->child[i]->contains) {
				fin = 0;
				findCommon(node->child[i], start, end);
			}
		}
		if (fin) {
			cs.push_back(std::make_pair(start, end));
		}
	}

	void BuildTree() {
		for (unsigned int i = 0; i < s.size(); i++) {
			if (root->child.size() == 0) {
				addSuffix(i, root);
			}
			else {
				bool match = 0;
				for (unsigned int p = 0; p < root->child.size(); p++) {
					if (checkFirst(i, root->child[p])) {
						match = 1;
						traverseNode(i, root->child[p]);
						break;
					}
				}
				if (!match) addSuffix(i, root);
			}
		}
	}

public:
	// tree builds when class constructor is called
	SuffixTree(string init, unsigned int sep_init) {
		s = init;
		separator_pos = sep_init;
		root = new Node;
		BuildTree();
	}

	~SuffixTree() {
		for (unsigned int i = 0; i < root->child.size(); i++) deleteNode(root->child[i]);
		delete root;
	}

	// returns array of pairs of int, where first element is starting index of common substring
	// and second is end index
	// returns sequences longer than min_len; all by default
	// -1 as an argument returns only longest common substring
	vector<std::pair<unsigned int, unsigned int>> calculateCS(int min_len = 0) {
		for (unsigned int i = 0; i < root->child.size(); i++) {
			inspectNode(root->child[i]);
		}
		for (unsigned int i = 0; i < root->child.size(); i++) {
			if (root->child[i]->contains) findCommon(root->child[i]);
		}

		if (min_len == -1) {
			if (cs.size() > 1) {
				unsigned int max = cs[0].second - cs[0].first + 1;
				unsigned int max_el = 0;
				for (unsigned int i = 1; i < cs.size(); i++) {
					if (cs[i].second - cs[i].first + 1 > max) {
						max = cs[i].second - cs[i].first + 1;
						max_el = i;
					}
				}
				cs.erase(cs.begin(), cs.begin() + max_el);
				cs.erase(cs.begin() + 1, cs.end());
			}
		}
		else {
			for (unsigned int i = 0; i < cs.size();) {
				if (cs[i].second - cs[i].first + 1 < min_len) {
					cs.erase(cs.begin() + i);
				}
				else i++;
			}
		}
		return cs;
	}
};

// constructs string for input for building a tree
std::pair<string, int> constructString(string s_first, string s_second) {
	unsigned int pos = s_first.length();
	string res = s_first + "#" + s_second + "$";
	return std::make_pair(res, pos);
}

bool cmpSequence(std::pair<unsigned int, unsigned int> fpair, std::pair<unsigned int, unsigned int> spair) {
	return (fpair.second - fpair.first > spair.second - spair.first);
}

// prints returned array
void print(vector<std::pair<unsigned int, unsigned int>> lcs, string s, bool print_len = 0) {
	std::sort(lcs.begin(), lcs.end(), cmpSequence);
	for (unsigned int i = 0; i < lcs.size(); i++) {
		for (unsigned int p = lcs[i].first; p <= lcs[i].second; p++) {
			std::cout << s[p];
		}
		if (print_len) std::cout << " " << lcs[i].second - lcs[i].first + 1;
		std::cout << std::endl;
	}
}



int main() {
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	string s_first = "string";
	string s_second = "ringing";
	auto p = constructString(s_first, s_second);
	std::auto_ptr<SuffixTree> st(new SuffixTree(p.first, p.second));
	print(st->calculateCS(-1), p.first, 1);

	// for futher works
	/*std::swap(s_first, s_second);
	auto p2 = constructString(s_first, s_second);
	std::auto_ptr<SuffixTree> st2(new SuffixTree(p2.first, p2.second));
	print(st2->calculateCS(80), p2.first, 1);*/

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms" << std::endl;
}