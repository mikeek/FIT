#ifndef TRIENODE_H
#define TRIENODE_H

#include <vector>
#include <string>
#include <QString>

#include <inttypes.h>

class TrieNode;

using stringVector = std::vector<QString>;
using nodeVector = std::vector<TrieNode *>;

class TrieNode
{
public:
    /**
     * @brief Constructor
     */
    TrieNode(TrieNode *parent = nullptr);

    /**
     * @brief Destructor
     */
    ~TrieNode();

    /**
     * @brief Get parent node
     *
     * @return parent node
     */
    TrieNode *getParent()    { return _parent; }

    /**
     * @brief Get words associated with this node
     *
     * @return words
     */
    stringVector& getWords() { return _words;  }

    /**
     * @brief Add word to this node
     *
     * @param word
     */
    void addWord(QString word) { _words.push_back(word); }

    /**
     * @brief Get child on given index
     *
     * @param index
     * @return child
     */
    TrieNode *getChild(uint16_t index) { return _children[index - 2]; }

    /**
     * @brief Create child on given index
     *
     * @param index
     * @return created child
     */
    TrieNode *createChild(uint16_t index);

private:

    TrieNode *_parent;    /**< Parent node */
    nodeVector _children; /**< Children vector */
    stringVector _words;  /**< associated words */
};

#endif // TRIENODE_H
