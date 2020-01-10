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
		bool contains = 0;			//if child node contains separator symbol
		int start = -1;				// starting index in string
		int end = -1;				// ending index in string
		vector<Node*> child;		// node childs
	};
private:
	Node* root;
	string s; 
	int separator_pos;				// position of separator symbol
	vector<std::pair<int, int>> cs; // list of all common substrings
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

	// finds all common substrings in a single node
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
			cs.push_back(std::make_pair(start, end));
		}
	}

	// returns array of pairs of int, where first ellment is starting index of common substring
	// and second is end index
	// returns only sequences longer than min_len, all by default
	// -1 as an argument to return only longest common substring
	vector<std::pair<int, int>> calculateCS(int min_len = 0) {
		for (int i = 0; i < root->child.size(); i++) {
			inspectNode(root->child[i]);
		}
		for (int i = 0; i < root->child.size(); i++) {
			if (root->child[i]->contains) findCommon(root->child[i]);
		}
		if (min_len == -1) {
			int max = cs[0].second - cs[0].first + 1;
			int max_el = 0;
			for (int i = 1; i < cs.size(); i++) {
				if (cs[i].second - cs[i].first + 1 > max) {
					max = cs[i].second - cs[i].first + 1;
					max_el = i;
				}
			}
			cs.erase(cs.begin(), cs.begin() + max_el);
			cs.erase(cs.begin() + 1, cs.end());
			return cs;
		}

		for (int i = 0; i < cs.size();) {
			if (cs[i].second - cs[i].first + 1 < min_len) {
				cs.erase(cs.begin() + i);
			}
			else i++;
		}
		return cs;
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

// constructs string for input for building a tree
std::pair<string, int> constructString(string s_first, string s_second) {
	int pos = s_first.length();
	string res = s_first + "#" + s_second + "$";
	return std::make_pair(res, pos);
}

// prints returned array
void print(vector<std::pair<int, int>> lcs, string s) {
	for (int i = 0; i < lcs.size(); i++) {
		for (int p = lcs[i].first; p <= lcs[i].second; p++) {
			std::cout << s[p];
		}
		std::cout << std::endl;
	}
}

int main() {
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	string s_first = "1211121112111211212111211121121112112111212221222121222122212212221221222222322322232232232223223223222322232232212122121212121212121212121221212112121212121212121221101101011010110110101101101110110110101101011112112112112121111121121121112212222122221222212222122122122221222212222122111111111111111111111111110111111110010000100001000010000100001000010010010001001011010110101101101011011001111212121211121121121211121121211211212112110110010101010101010101011001010101010101010101100030030030000300300300300300030030030030000030010010010011122221122212221222212221222121211111111111111111111111111111111100100010000100010001000010001000101001001110111011101110110111101101121121121121121112112112111121121112121110101010010101010101010010101010101010101001000003003003003030000300300030300300000001001001001001000100010000011011101110110111101101110111011100100000000000000000000000001000000000330333033001111112112121211112121121121211010101010101001101001010100101000303003003003003003003030030030001001000010010300100011011111011101111011011111000000300000000000030000003303330333033303303033303333030003000300030030030030000100101000100101000011000303030303303030303030303303030303033233232332332332332333233233233303033033033033303303003000300100300030003000030003033333333330333333333333330332232223222322322223223222233323323323323323323323330330333030330333303303303333232323232332323232332323232323232332122121222212212221212222122333333333333333333333333333333333333232322322232222322232322322232223223223323332332333332333232332333303303330330330333303303033330330333332323232323232323232323232323232330003000030033332333333333332333333333323333332232222322223222232222222232222322223222232223323233233233233233232332232333303033303030333303033303030333303033332232323232323223232323223223232323232232323223223222222122122122122222122122122122212232232223223223223222322322322232232232223223223223222322322322232232232223223223222322322322322232232232223";
	string s_second = "12111211121121121211121112112112121121112122212221212221222121222212212212232232232222322322322223222232212121212121212121211212112212121121212121211212112211011010110111011010110110111011011010111011111211112112121111121211111221221221222212222122212212212222122221222212211111211111111111111110111101111111100100001000010000100100100001000010010010001001010101101010110101001100111212112112111121211121121211211212112110110011010101010111010110011010101010110101100030003000030030030030000030030030030000030010010010011112222112221222122221222122212121111211111111111111111111111111111001000100001000100010000100010001010010011101110111011210110111101101121121121121121112111121111112111211101010100101001010101001100101010101010010000030030000303000030000030300000000101000100100100010001000001010110111011101101011011011100100000003000000010000000000010000000030001112111212112121211001010101001010010010010003030030030030003003000303003000001000000100000101111011111101111110000030000030000030000003330330333033303303033030333030003000300030030030300010001001000100010003033030330303033030303030303303323323233233333232333233233333030303303303030330003000300100300000030000300303303333333303333303333333303322322322322322232323222333233232332332332332333330333033303333033330333232323233232332323323323233232323321222122222122212222122233033333333333333333333333333333333333232322322323222322232323222322322323323333323333323303232332323330330333033330333033303333033333332323232323232323232323223232323300000030333323333323333233333233332333332232222232222322223222222223222232222322223222323233233232323233232332232333303330330333303330330333033333223232323232223232323222322323232322232323223223222222122122221222221222212212221223223222322322322322232232232223223223222322322322322232232232223223223222322322322322232232232223";
	auto p = constructString(s_first, s_second);
	SuffixTree st(p.first, p.second);
	//st.calculateCS();
	print(st.calculateCS(-1), p.first);
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms" << std::endl;
}
