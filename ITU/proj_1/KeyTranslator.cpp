#include "KeyTranslator.h"
#include <QSettings>

#include <qkeysequence.h>
#include <iostream>
#include <sstream>

/**
 * @brief Constructor
 */
KeyTranslator::KeyTranslator(QObject *parent) :
QObject(parent)
{
	/* Load saved values */
	loadKeys();
}

/**
 * @brief Destructor
 */
KeyTranslator::~KeyTranslator()
{
	saveKeys();
}

/**
 * @brief Load key bindings
 */
void KeyTranslator::loadKeys()
{
	for (uint16_t i = 0; i < _keys.size(); ++i) {
		int defaultValue = _keys[i];
		std::stringstream ss;
		ss << i;

		_keys[i] = _settings.value(QString(ss.str().c_str()), defaultValue).toInt();
	}
}

/**
 * @brief Save key bindings
 */
void KeyTranslator::saveKeys()
{
	for (uint16_t i = 0; i < _keys.size(); ++i) {
		std::stringstream ss;
		ss << i;

		_settings.setValue(QString(ss.str().c_str()), _keys[i]);
	}
}

/**
 * @brief Translate key to string
 */
QString KeyTranslator::key2str(int key)
{
	switch (key) {
	case Qt::Key_Shift:
		return "Shift";
	case Qt::Key_AltGr:
		return "AltGr";
	case Qt::Key_Alt:
		return "Alt";
	case Qt::Key_Control:
		return "Control";
	case Qt::Key_Meta:
		return "Meta";
	default:
		break;
	}

	return QKeySequence(key).toString();
}
