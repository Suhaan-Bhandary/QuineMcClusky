#include <iostream>
#include <vector>
#include <set>
#include <map>
using namespace std;

// Only For Developer: Donot Uncomment for regural use
// #define SuhaanTesting

struct GroupElement
{
	vector<int> minterms;
	vector<int> value;
};

pair<vector<int>, int> getMintermAndCount(const int minTerm, const int literalCount);
map<int, vector<GroupElement>> mergeGroupElement(map<int, vector<GroupElement>> numberedGroups, int literals, vector<GroupElement> &notUpdated);
void displayFormatedAnswer(set<vector<int>> answer);

// All Print Functions
void printMinterm(const vector<int> v);
void printState(const vector<int> v);
void printMintermWithDontCare(const vector<int> v, const set<int> dontCareSet);
void printGroups(const map<int, vector<GroupElement>> allGroups, const set<int>dontCareSet);
void printNotUpdated(vector<GroupElement> notUpdated);

int main() {
#ifdef SuhaanTesting
	freopen("input.txt", "r", stdin);
	freopen("output.txt", "w", stdout);
#endif

	// Header
	cout << "\nQuine McCluskey Minimization Technique\n";

	// SetUp
	cout << "\n--------------------------------------------------\n";

	int literalCount;
	cout << "Enter the number of Literals(greather than Zero): ";
	cin >> literalCount;

	int minTermsCount;
	cout << "Number of Minterms: ";
	cin >> minTermsCount;
	set<int> minTermsSet;
	cout << "Enter Minterms with space betweeen(eg. 1 2 3): " << endl;
	for (int i = 0; i < minTermsCount; ++i)
	{
		int minterm;
		cin >> minterm;
		minTermsSet.insert(minterm);
	}

	int dontCareTermsCount;
	cout << "Number of DontCares: ";
	cin >> dontCareTermsCount;
	set<int> dontCareTermsSet;
	cout << "Enter Dont Care Terms with space betweeen(eg. 1 2 3): " << endl;
	for (int i = 0; i < dontCareTermsCount; ++i)
	{
		int dontCareTerm;
		cin >> dontCareTerm;
		dontCareTermsSet.insert(dontCareTerm);
	}

	cout << "\n--------------------------------------------------\n";

	// Making Groups
	map<int, vector<GroupElement>> numberedGroups;

	// Adding minterms to the numberedGroups
	set<int> totalTerms;
	totalTerms.insert(minTermsSet.begin(), minTermsSet.end());
	totalTerms.insert(dontCareTermsSet.begin(), dontCareTermsSet.end());
	for (auto term : totalTerms)
	{
		pair<vector<int>, int> result = getMintermAndCount(term, literalCount);

		// Creating a group element
		GroupElement groupEle;
		groupEle.minterms.push_back(term);
		groupEle.value = result.first;

		// Adding group element to the respective group
		numberedGroups[result.second].push_back(groupEle);

		// Printing the minterm with its value
		cout << term << ": ";
		printMinterm(result.first);
		cout << endl;
	}

	// Now Printing the All the Groups
	cout << "\n--------------------------------------------------\n";
	cout << "Grouped Minterms And DontCares: \n" << endl;
	printGroups(numberedGroups, dontCareTermsSet);

	// Merging the Groups and printing them
	// and not updated groups are those which were not considred during merge
	vector<GroupElement> notUpdated;
	while (true) {
		map<int, vector<GroupElement>> newNumberedGroups = mergeGroupElement(numberedGroups, literalCount, notUpdated);

		if (newNumberedGroups.size() == 0 ) break;
		numberedGroups = newNumberedGroups;

		// Now Printing the All the Groups
		cout << "\n--------------------------------------------------\n";
		cout << "Grouped Minterms And DontCares: \n" << endl;
		printGroups(numberedGroups, dontCareTermsSet);
	}

	// Printing the terms which were left out
	cout << "\n--------------------------------------------------\n";
	cout << "Terms which were left when merging the groups" << endl;
	printNotUpdated(notUpdated);

	// Step to get the minterm and the table of them

	// going through the table and taking the minterms which appear only once in the table
	// and adding it to the answer
	map<int, int> countInTable;
	set<vector<int>> seenMinterms; // Used for identifying the seen minterms

	for (auto& group : numberedGroups) {
		for (auto& groupElement : group.second) {

			// Checking if the minterm is seen or not
			if (seenMinterms.count(groupElement.value)) continue;
			seenMinterms.insert(groupElement.value);

			// Adding minterms to the table
			for (auto& minterm : groupElement.minterms) {
				if (!dontCareTermsSet.count(minterm)) {
					countInTable[minterm]++;
				}
			}
		}
	}

	// Adding other prime implicants which were lost during merging
	for (auto& groupElement : notUpdated) {
		if (seenMinterms.count(groupElement.value)) continue;
		seenMinterms.insert(groupElement.value);
		// printState(groupElement.value);
		// cout << endl;
		for (auto& minterm : groupElement.minterms) {
			if (!dontCareTermsSet.count(minterm)) {
				countInTable[minterm]++;
			}
		}
	}

	// Now we will extract the answer
	set<vector<int>> answer;

	// it stroes the individual terms used so tha we can check which terms are remaining
	set<int> termsTaken;
	for (auto& cell : countInTable) {
		if (cell.second == 1)
		{
			for (auto& gp : numberedGroups) {
				for (auto& ele : gp.second) {
					int count = 0;
					for (auto& num : ele.minterms) {
						if (cell.first == num)count++;
					}
					if (count != 0) {
						answer.insert(ele.value);
						for (auto& num : ele.minterms) {
							termsTaken.insert(num);
						}
					}
				}
			}
		}
	}

	// Printing the table
	cout << "\n--------------------------------------------------\n";
	cout << "Terms with the occurnce of them: " << endl;
	for (auto& i : countInTable) {
		cout << i.first << ": " << i.second << endl;
	}

	// Finding the elements left in countable which are not included
	// The logic suggest that if the term is in the count table then it is in any of the elements

	for (auto& iterator : countInTable) {
		if (!termsTaken.count(iterator.first))
		{
			bool flag = false;
			// cout << iterator.first << endl;
			for (auto& gp : numberedGroups) {
				for (auto& ele : gp.second) {
					for (auto& j : ele.minterms) {
						if (iterator.first == j) {
							answer.insert(ele.value);
							flag = true;
							break;
						}
					}
					if (flag) break;
				}
				if (flag) break;
			}

			// Checking if it is present
			for (auto& j : notUpdated) {
				for (auto& k : j.minterms) {
					if (iterator.first == k) {
						answer.insert(j.value);
						flag = true;
						break;
					}
				}
				if (flag) break;
			}
		}
	}

	// This is for the remaining onesfor which is not included
	for (auto& minterm : minTermsSet) {
		if (!countInTable.count(minterm)) {
			answer.insert(getMintermAndCount(minterm, literalCount).first);
		}
	}

	// showing the answer
	cout << "\n--------------------------------------------------\n";
	cout << "The Minimized Equation is: ";
	displayFormatedAnswer(answer);

	return 0;
}

pair<vector<int>, int> getMintermAndCount(const int minTerm, const int literalCount) {
	vector<int> mintermVector;
	int onesCount = 0;

	for (int i = 0; i < literalCount; ++i)
	{
		if (minTerm & 1 << (literalCount - i - 1)) {
			mintermVector.push_back(1);
			onesCount++;
		}
		else
		{
			mintermVector.push_back(0);
		}
	}

	pair<vector<int>, int> mintermAndCount;
	mintermAndCount.first = mintermVector;
	mintermAndCount.second = onesCount;

	return mintermAndCount;
}

// merging function
map<int, vector<GroupElement>> mergeGroupElement(map<int, vector<GroupElement>> numberedGroups, int literals, vector<GroupElement> &notUpdated) {
	// Merged groups will be added here
	map<int, vector<GroupElement>> newNumberedGroups;
	int updateCount = 0;

	set<vector<int>> termsUsed;

	for (auto& group : numberedGroups) {
		if (numberedGroups.count(group.first + 1))
		{
			int wholeGroupNotUpdated = 0;
			for (auto& groupElement : group.second) {
				// Checking if next group is present or not
				// Comparing the elements with the next group elements
				for (auto& nextGroupElement : numberedGroups[group.first + 1]) {
					int diffIndex = -1;
					int diffCount = 0;

					for (int i = 0; i < literals; ++i)
					{
						if (groupElement.value[i] != nextGroupElement.value[i]) {
							diffIndex = i;
							diffCount++;
						}
					}

					if (diffCount == 1) {
						updateCount++;
						wholeGroupNotUpdated++;

						GroupElement newGroupElement;

						newGroupElement.value = groupElement.value;
						newGroupElement.value[diffIndex] = -1;

						// Adding new minters

						newGroupElement.minterms = groupElement.minterms;
						for (auto& m : nextGroupElement.minterms) {
							newGroupElement.minterms.push_back(m);
						}

						termsUsed.insert(groupElement.minterms);
						termsUsed.insert(nextGroupElement.minterms);

						// Adding new numbered group
						newNumberedGroups[group.first].push_back(newGroupElement);
					}
				}
			}
		}
	}

	if (updateCount == 0) return {};

	for (auto& group : numberedGroups) {
		for (auto& groupElement : group.second) {
			if (!termsUsed.count(groupElement.minterms))
			{
				notUpdated.push_back(groupElement);
			}
		}
	}

	return newNumberedGroups;
}

void displayFormatedAnswer(set<vector<int>> answer) {
	for (auto& i : answer) {
		char letter = 'A';
		for (auto& j : i) {
			if (j != -1) {
				if (j == 1)
				{
					cout << letter;
				} else {
					cout << "~" << letter;
				}
			}
			letter++;
		}
		cout << " + ";
	}
	cout << "\b\b" << "  " << endl;
}

// All Print functions
void printMinterm(const vector<int> v) {
	for (int i = 0; i < v.size(); ++i)	{
		cout << v[i] << " ";
	}
}

void printState(const vector<int> v) {
	for (int i = 0; i < v.size(); ++i)	{
		if (v[i] == -1)
		{
			cout << "_" << " ";
		} else {
			cout << v[i] << " ";
		}
	}
}

void printMintermWithDontCare(const vector<int> v, const set<int> dontCareSet) {
	for (int i = 0; i < v.size(); ++i)	{
		if (dontCareSet.count(v[i])) cout << v[i] << "* ";
		else cout << v[i] << "  ";
	}
}

// For printing the Group
void printGroups(const map<int, vector<GroupElement>> allGroups, const set<int>dontCareSet) {
	for (auto& group : allGroups) {
		cout << "Group " << group.first << endl;
		for (auto& groupElement : group.second) {
			printMintermWithDontCare(groupElement.minterms, dontCareSet);
			cout << ": ";
			printState(groupElement.value);
			cout << endl;
		}
	}
}

void printNotUpdated(vector<GroupElement> notUpdated) {
	cout << endl;
	for (auto& groupElement : notUpdated) {
		printMinterm(groupElement.minterms);
		cout << ": ";
		printState(groupElement.value);
		cout << endl;
	}
	cout << endl;
}