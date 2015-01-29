#include "TrieNode.h"

#include <iostream>
#include <iomanip>

#define MAX_CHILDREN 8

TrieNode::TrieNode(TrieNode *parent) : _parent{parent}
{
	_children.resize(MAX_CHILDREN, nullptr);
}

TrieNode::~TrieNode()
{
	for (auto child : _children) {
		if (child) {
			delete child;
		}
	}
}

TrieNode *TrieNode::createChild(uint16_t index)
{
	if (!_children[index - 2]) {
		_children[index - 2] = new TrieNode(this);
	}

	return _children[index - 2];
}
