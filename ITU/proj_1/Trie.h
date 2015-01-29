#ifndef TRIE_H
#define TRIE_H

#include "TrieNode.h"
#include <QString>

class Trie
{
public:
    /**
     * @brief Constructor
     */
    Trie();

    /**
     * @brief Destructor
     */
    ~Trie();

    /**
     * @brief Get root node
     *
     * @return root node
     */
    TrieNode *getRoot()    { return _root; }

    /**
     * @brief Get words from actual trie node
     *
     * @return string vector
     */
    stringVector getActWords() { return (_depth > 0) ? _emptyVec : _actNode->getWords(); }

    /**
     * @brief Reset trie position
     */
    void resetPosition() { if (_actNode != _root) { _lastValid = _actNode; } _actNode = _root; }

    /**
     * @brief Update position when character pressed
     *
     * @param c pressed character
     */
    void updatePosition(char c) { updatePositionByIndex(decodeChar(c)); }

    /**
     * @brief Update position with index of child
     *
     * @param index child index
     */
    void updatePositionByIndex(int index);

    /**
     * @brief Load file with words
     *
     * @param file path to file
     */
    void loadFile(std::string file);

    /**
     * @brief Move back
     */
    void moveUp();

    /**
      * @brie Restore last node before reset
      */
    void restoreLast();

private:

    /**
     * @brief Decode character into number
     *
     * @param c character
     * @return decoded number
     */
    uint16_t decodeChar(char c);

    /**
     * @brief Move down in trie
     *
     * @param index index of child
     * @param create true == create child if it doesn't exist
     */
    void moveDown(uint16_t index, bool create = false);

    /**
     * @brief Add word into trie
     *
     * @param word
     */
    void addWord(QString& word);

    /**
     * @brief Create string without unicode characters
     *
     * @param str string
     * @return ascii string
     */
    QString fromUnicode(QString& str);
    int _depth{0};
    stringVector _emptyVec{};
    TrieNode *_root, *_actNode, *_lastValid;  /**< trie nodes */
};

#endif // TRIE_H
