# lsc-suffix_tree
>If this code helped you somehow please support me
><https://buymeacoff.ee/fWqd6hO>

C++ Implementation of Longest Common Substring problem using Suffix Tree built with naive algorithm

- [Implementation](#implementation)
- [Usage](#usage)
- [Issues](#issues)
- [Further Works](#further-work)
- [References](#references)

## Implementation
[Suffix Tree](https://en.wikipedia.org/wiki/Suffix_tree) is implemented with naive algorithm, where on each step new suffix of a string added to the tree.

Common Substring check is implemented by checking if child node contains separator symbol.

## Usage
To use this implementation you will need to add whole SuffixTree class to your project; Also you will need constructString method.

As this code works for two strings only, it can be upgraded to infinite number of strings.

Firstly, generate \<string, int\> pair with constructString() method, where \<string\> is first and second string divided by separator, and \<int\> is a separator position in a string;

Then create new instance of suffix tree by calling class constructor with parameters obtained in previous step;
Suffix Tree is being built on constructor call, so no need to call it again.

Then call calculateCS() method of SuffixTree class.

It will return array of pairs of integers, where first element is starting index of common substring and second element is ending index;

calculateCS() can take a parameter; If none set, it returns all common substrings of two strings; If -1 set, it returns Longest Common Substring; Otherwise it returns all substrings longer than parameter.

## Issues
1. As each suffix of a substring is also a substring, all of them will appear in final result
2. By now function returns only indexes of start and end of a common substring in a first string;
So if position of a common substring in second string required, you'll need to perform additional calculations.
3. Naive tree building takes more than 1 second if sum of string lengths more than 5k symbols.

## Further works
Add function to find position of common substring in second string also.

## References
<https://marknelson.us/posts/1996/08/01/suffix-trees.html>

<https://www.geeksforgeeks.org/suffix-tree-application-5-longest-common-substring-2/>

<http://yury.name/internet/01ianote.pdf>
