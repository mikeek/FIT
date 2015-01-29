#include "Trie.h"

#include <cctype>
#include <iostream>
#include <stdexcept>

#include <QFile>
#include <QTextStream>

Trie::Trie()
{
	_root = new TrieNode();
	resetPosition();
	_depth = 0;
}

Trie::~Trie()
{
	delete _root;
}

QString Trie::fromUnicode(QString& str)
{
	/* Normalize string */
	QString result;
	QString tmp = str.normalized(QString::NormalizationForm_D);

	for (auto c : tmp) {
		/* Get only valid ascii characters */
		if (c.category() != QChar::Mark_Enclosing
				&& c.category() != QChar::Mark_NonSpacing
				&& c.category() != QChar::Mark_SpacingCombining) {

			result.append(c);
		}
	}


	return result;
}

void Trie::loadFile(std::string file)
{
	/* Open file */
	QFile input(file.c_str());

	if (!input.open(QIODevice::ReadOnly | QIODevice::Text)) {
		throw std::invalid_argument("Cannot open file " + file);
	}

	/* Initialize UTF stream */
	QTextStream in(&input);
	//    in.setCodec("UTF-8");

	while (true) {
		/* Read word */
		QString word = in.readLine();

		/* Check EOF */
		if (word.isEmpty()) {
			break;
		}

		addWord(word);
	}

	resetPosition();
}

void Trie::addWord(QString& word)
{
	resetPosition();

	std::string asciiWord = fromUnicode(word).toStdString();
	for (auto c : asciiWord) {
		uint16_t decoded = decodeChar(c);
		if (decoded < 2) {
			continue;
		}

		moveDown(decoded, true);
	}

	_actNode->addWord(word);
}

void Trie::restoreLast()
{
	if (_lastValid) {
		_actNode = _lastValid;
	}
}

void Trie::updatePositionByIndex(int index)
{
	if (index < 2) {
		resetPosition();
	} else {
		moveDown(index);
	}
}

void Trie::moveDown(uint16_t index, bool create)
{
	TrieNode *child = _actNode->getChild(index);

	if (child) {
		_actNode = child;
	} else if (create) {
		_actNode = _actNode->createChild(index);
	} else {
		_depth++;
	}
}

void Trie::moveUp()
{
	if (_depth > 0) {
		_depth--;
		return;
	}

	if (_actNode->getParent()) {
		_actNode = _actNode->getParent();
		_lastValid = _actNode;
	}
}

uint16_t Trie::decodeChar(char c)
{
	switch (std::tolower(c)) {
	case ' ':
		return 0;
	case 'a': case 'b': case 'c':
		return 2;
	case 'd': case 'e': case 'f':
		return 3;
	case 'g': case 'h': case 'i':
		return 4;
	case 'j': case 'k': case 'l':
		return 5;
	case 'm': case 'n': case 'o':
		return 6;
	case 'p': case 'q': case 'r': case 's':
		return 7;
	case 't': case 'u': case 'v':
		return 8;
	case 'w': case 'x': case 'y': case 'z':
		return 9;
	default:
		return 1;
	}
}
