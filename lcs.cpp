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
		bool contains = 0; //if child node contains separator symbol
		int start = -1; // starting index in string
		int end = -1;  // ending index in string
		vector<Node*> child; // node childs
	};
private:
	Node* root;
	string s; 
	int separator_pos; // position of separator symbol
	vector<std::pair<int, int>> lcs; // list of all common sequences
public:
	SuffixTree(string init, int sep_init) {
		s = init;
		separator_pos = sep_init;
		root = new Node;
		BuildTree();
	}

	~SuffixTree() {
		delete root;
	}

	// add child from position %n% to the end of string to %node%
	void addSuffix(int n, Node* node) {
		Node* add = new Node;
		add->start = n;
		add->end = s.size() - 1;
		node->child.push_back(add);
	}

	// check if symbol of string at position %child[0]% equal to symbol at %s[n]%
	bool checkFirst(int n, Node* child) {
		return s[n] == s[child->start];
	}

	void splitEdge(int i, int len, Node* node) {
		Node* add = new Node;
		add->start = node->start + len;
		add->child = node->child;
		add->end = node->end;
		node->child.clear();
		node->child.push_back(add);
		node->end = len + node->start - 1;
		addSuffix(i, node);
	}


	void traverseNode(int n, Node* node) {
		int len = 0;
		for (int i = n; i < s.size(); i++) {
			if (len <= node->end - node->start) {
				if (s[i] == s[node->start + len]) ++len;
				else {
					splitEdge(i, len, node);
					return;
				}
			}
			else {
				for (int p = 0; p < node->child.size(); p++) {
					if (checkFirst(i, node->child[p])) {
						traverseNode(i, node->child[p]);
						return;
					}
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
			for (int i = 0; i < node->child.size(); i++) {
				if (inspectNode(node->child[i])) node->contains = 1;
			} 
			if (node->contains == 1) return 1;
				else return 0;
		}
	}

	void findCommon(Node* node, int start = 0, int end = -1) {
		start = node->start - (end - start + 1);
		end = node->end;
		bool fin = 1;
		for (int i = 0; i < node->child.size(); i++) {
			if (node->child[i]->contains) {
				fin = 0;
				findCommon(node->child[i], start, end);
			}
		}
		if (fin) {
			lcs.push_back(std::make_pair(start, end));
		}
	}

	void calculateLCS(int min_len = 0) {
		for (int i = 0; i < root->child.size(); i++) {
			inspectNode(root->child[i]);
		}
		for (int i = 0; i < root->child.size(); i++) {
			if (root->child[i]->contains) findCommon(root->child[i]);
		}
		// /*
		for (int i = 0; i < lcs.size(); i++) {
			for (int p = lcs[i].first; p <= lcs[i].second; p++) std::cout << s[p];
			std::cout << std::endl;
		}
		// */
	}

	void BuildTree() {
		for (int i = 0; i < s.size(); i++) {
			if (root->child.size() == 0) {
				addSuffix(i, root);
			}
			else {
				bool match = 0;
				for (int p = 0; p < root->child.size(); p++) {
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
};

std::pair<string, int> constructString(string s_first, string s_second) {
	int pos = s_first.length();
	string res = s_first + "#" + s_second + "$";
	return std::make_pair(res, pos);
}

int main() {
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	string s_first = "string";
	string s_second = "ringing";
	auto p = constructString(s_first, s_second);
	SuffixTree st(p.first, p.second);
	st.calculateLCS();
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms" << std::endl;
}
