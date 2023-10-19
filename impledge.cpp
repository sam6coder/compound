#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <map>

using namespace std;

class Trie;

class Node
{
public:
    Node() { mLetter = ' '; mTerminator = false; }
    ~Node() {}
    void setLetter(char c) { mLetter = c; }
    char getLetter() { return mLetter; }
    void setTerminator() { mTerminator = true; }
    bool hasTerminator() { return mTerminator; }
    void appendChild(Node* child) { mChildren.push_back(child); }
    Node* findChild(char c);
    void processNode(Trie*);
    void setTriePointer(Trie* trie) { mTrie = trie; }
    void process(Node*, char*, int, int, int[]);
    void storeThisWord(char*, int);
    bool isComposed(string, int, string, string);
    vector<Node*> children() { return mChildren; }

private:
    char mLetter;
    bool mTerminator;
    vector<Node*> mChildren;
    Trie* mTrie;
};

class Trie
{
public:
    Trie();
    void addWord(string s);
    bool searchWord(string s);
    void processTrie();
    vector<string>& getLongestStr() { return m_longestStr; }
    int getSecondLongest() { return m_secondLongest; }
    void setSecondLongest(int n) { m_secondLongest = n; }
    multimap<string, string>& getMap() { return m_Map; }

private:
    Node* m_root;
    int m_secondLongest;
    // key string for map
    vector<string> m_longestStr;
    // map for substring of a compount
    multimap<string, string> m_Map;
};

void Node::storeThisWord(char* w, int sz)
{
    int index = 0;
    vector<string> longStr = mTrie->getLongestStr();
    index = longStr[0].size() < longStr[1].size() ? 0 : 1;

    if (sz > longStr[index].size()) {
        mTrie->getLongestStr()[index] = string(w);
        
        mTrie->setSecondLongest(min(longStr[0].size(), longStr[1].size()));
    }
}

Node* Node::findChild(char c)
{
    for (size_t i = 0; i < mChildren.size(); i++)
    {
        Node* ptr = mChildren.at(i);
        if (ptr->getLetter() == c)
        {
            return ptr;
        }
    }
    return NULL;
}


bool Node::isComposed(string inputStr, int sz, string keyStr, string originalStr)
{
    if (sz <= 0) return false;

    
    if (inputStr == originalStr) {
        if (mTrie->searchWord(inputStr)) {
            mTrie->getMap().insert(make_pair(keyStr, inputStr));
            return true;
        }
    }

    
    for (int i = 1; i < sz; i++) {
        string left = inputStr.substr(0, i);
        string right = inputStr.substr(i, inputStr.size());
        if (mTrie->searchWord(left)) {
            if (mTrie->searchWord(right)) {
                // component of the compound
                mTrie->getMap().insert(make_pair(keyStr, left));
                mTrie->getMap().insert(make_pair(keyStr, right));
                return true;
            }
            else {
                if (isComposed(right, right.size(), keyStr, originalStr)) {
                    // component of the compound
                    mTrie->getMap().insert(make_pair(keyStr, left));
                    return true;
                }
            }
        }
        // repeat: recursive
        if (isComposed(left, left.size(), keyStr, originalStr)
            && isComposed(right, right.size(), keyStr, originalStr))
            return true;
    }
    return false;
}



void Node::process(Node* node, char* word, int count,
    int terminator_count, int substringIndex[])
{
    if (node->mLetter != ' ') {
        word[count++] = node->mLetter;
    }

    if (node->hasTerminator()) {
        substringIndex[terminator_count] = count;
        if (count >= mTrie->getSecondLongest()) {
            /* only the word with more than one terminator can be a candidate for a compound word */
            if ((node->mChildren).size() == 0 && terminator_count >= 1) {
                // check substring of a composite word
                int st = substringIndex[terminator_count - 1];
                int end = substringIndex[terminator_count];
                string str = string(word).substr(st, end - st);
                string keyStr = string(word).substr(0, count);
                mTrie->getMap().insert(make_pair(keyStr, string(word).substr(0, st)));
                string originalStr = str;
                // Check if compound. If it is, give it a try to be the longest compound */
                if (isComposed(str, str.size(), keyStr, originalStr)) {
                    word[count] = '\0';
                    storeThisWord(word, count);
                }
            }
        }
        terminator_count++;
    }

    for (int i = 0; i < (node->mChildren).size(); i++) {
        process((node->mChildren).at(i), word, count, terminator_count, substringIndex);
    }
}

/* process all the branches ('a'-'z') one by one */

void Node::processNode(Trie* trie)
{
    //  Node class have pointer to the trie
    setTriePointer(trie);

    char* word = new char[200];
    int substringIndex[10];
    for (size_t i = 0; i < mChildren.size(); i++)
    {
        char c = 'a' + i;
        Node* ptr = mChildren.at(i);
        
        process(ptr, word, 0, 0, substringIndex);
    }
    
}


void Trie::processTrie()
{
    
    m_root->processNode(this);
}

// trie constructor
Trie::Trie()
{
    m_root = new Node();
    m_secondLongest = 1;
    m_longestStr.push_back(" ");
    m_longestStr.push_back(" ");
}

// add words to the trie
void Trie::addWord(string s)
{
    Node* current = m_root;

    if (s.length() == 0)
    {
        current->setTerminator();
        return;
    }

    for (size_t i = 0; i < s.length(); i++)
    {
        Node* child = current->findChild(s[i]);
        if (child != NULL)
        {
            current = child;
        }
        else
        {
            Node* ptr = new Node();
            ptr->setLetter(s[i]);
            current->appendChild(ptr);
            current = ptr;
        }
        if (i == s.length() - 1)
            current->setTerminator();
    }
}

// search a word from the trie
bool Trie::searchWord(string s)
{
    Node* current = m_root;

    while (current != NULL)
    {
        for (size_t i = 0; i < s.length(); i++)
        {
            Node* ptr = current->findChild(s[i]);
            if (ptr == NULL)
                return false;
            current = ptr;
        }

        if (current->hasTerminator())
            return true;
        else
            return false;
    }
    return false;
}




int main()
{
    Trie* trie = new Trie();


    ifstream file;
    file.open("input2.txt");
    if (!file) {
        cout << "Error in opening input file" << endl;
        return -1;
    }

    int lineCount = 0;
    cout << "Adding words to the Trie..." << endl;

    string line;
    while (!file.eof()) {
        getline(file, line);
        trie->addWord(line);
        lineCount++;

    }

    file.close();



    // Loop through the trie from a-z to find the longest compound
    trie->processTrie();


    cout << "The two longest composite strings are: \n\n";
    vector<string> longStr = trie->getLongestStr();

    multimap<string, string>::iterator pos;
    for (int i = 0; i < 2; i++)
    {
        cout << longStr[i] << "(" << longStr[i].size() << ")" << endl;


    }

    delete trie;

    return 0;
}