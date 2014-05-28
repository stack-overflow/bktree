// SBW
// compilation: g++ -std=c++11 -o fuzzy_matcher fuzzy_matcher.cpp

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <vector>
#include <string>
#include <memory>
#include <deque>
#include <functional>
#include <cctype>
#include <cstring>

class BKTree
{
public:
	static const size_t MAX_WORD_SIZE = 1024;
	static int size;
	typedef std::deque<std::pair<int, std::string>> QueryResult;

	int v0[MAX_WORD_SIZE];
	int v1[MAX_WORD_SIZE];

public:
	int distParent;
	std::string word;
	std::vector<std::unique_ptr<BKTree> > subnodes;

	BKTree() :
		distParent(0)
	{
		++size;
	}

	BKTree(const std::string& in, int inDist) :
		word(in),
		distParent(inDist)
	{
		++size;
	}

	BKTree(std::string&& in, int inDist) :
		word(std::move(in)),
		distParent(inDist)
	{
		std::cout << "DSADSAD" << std::endl;
		++size;
	}

	~BKTree()
	{
	}

	void insert(const std::string& in)
	{
		if (word.empty())
		{
			word = in;
			distParent = 0;
			return;
		}
		else
		{
			int inDst = levensthein_distance(word.c_str(), in.c_str());

			if (subnodes.size() > 0)
			{
				for (const std::unique_ptr<BKTree>& sub : subnodes)
				{
					if (sub->distParent == inDst)
					{
						sub->insert(in);
						return;
					}
				}
			}

			/*subnodes.push_back(new BKTree(in, inDst));*/
			subnodes.push_back(
				std::unique_ptr<BKTree>(new BKTree(in, inDst))
				);
			return;
		}
	}

	void query(const std::string& in, int tolerance, QueryResult& out)
	{
		int inDst = levensthein_distance(word.c_str(), in.c_str());

		if (inDst <= tolerance)
		{
			out.push_back(std::make_pair(inDst, word));
		}

		for (const auto& sub : subnodes)
		{
			if ((sub->distParent <= (inDst + tolerance)) &&
				(sub->distParent >= (inDst - tolerance)))
			{
				sub->query(in, tolerance, out);
			}
		}

		return;
	}

	void print()
	{
		std::cout << word << " " << distParent << " " << subnodes.size() << std::endl;

		for (const std::unique_ptr<BKTree>& sub : subnodes)
		{
			sub->print();
		}
	}
private:
	int levensthein_distance(const char* s, const char* t)
	{
		if (s == t) return 0;

		const size_t sLen = strlen(s);
		const size_t tLen = strlen(t);

		if (sLen == 0) return tLen;
		if (tLen == 0) return sLen;

		const size_t vecLen = tLen + 1;

		for (size_t i = 0; i < vecLen; ++i)
		{
			v0[i] = i;
		}

		for (size_t i = 0; i < sLen; ++i)
		{
			v1[0] = i + 1;

			for (size_t j = 0; j < tLen; ++j)
			{
				int cost = (s[i] == t[j]) ? 0 : 1;
				v1[j + 1] = std::min(v1[j] + 1, v0[j + 1] + 1);
				v1[j + 1] = std::min(v1[j + 1], v0[j] + cost);
			}

			for (size_t j = 0; j < vecLen; ++j)
			{
				v0[j] = v1[j];
			}
		}

		return v1[tLen];
	}
};

int BKTree::size = 0;

int main()
{
	std::ios::ios_base::sync_with_stdio(false);

	std::unique_ptr<BKTree> bk_tree(new BKTree()); // Tree containing known words.
	std::vector<std::string> input_words; // Collection of input words.
	std::vector<std::string> matched_words; // Collection of matched words.

	std::ifstream fwords("dict.txt");
	std::string line;
	int numWords = 0;

	// Load known words from dictionary
	while (!fwords.eof())
	{
		std::getline(fwords, line);
		if (!line.empty())
		{
			bk_tree->insert(line);
			++numWords;
		}
	}
	fwords.close();

	// Load input words from file.
	std::ifstream input("in.txt");
	std::string in_word;
	while (!input.eof())
	{
		std::getline(input, line);
		if (!line.empty()) input_words.push_back(line);
	}
	input.close();

	matched_words.reserve(input_words.size());

	for (auto& in_word : input_words)
	{
		BKTree::QueryResult result;

		int max_distance = 4;

		char save = in_word[0];

		if (in_word.size() < 5 &&
			in_word.size() > 1 &&
			isdigit(in_word[1]))
		{
			in_word[0] = 'E';
			max_distance = 0;
		}

		// Query tree for input word
		bk_tree->query(in_word, max_distance, result);

		in_word[0] = save;

		if (!result.empty())
		{
			std::sort(std::begin(result), std::end(result));
			matched_words.push_back(result.front().second);
			std::cout << in_word << ":" << result.front().second << ":" << result.front().first << "\n";
		}
	}

	std::ofstream output("out.txt");
	for (auto& matched_word : matched_words)
	{
		output << matched_word << std::endl;
	}
	output.close();

	getchar();

	return 0;
}
