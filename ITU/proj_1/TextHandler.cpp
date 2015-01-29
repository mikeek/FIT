#include "TextHandler.h"
#include <iostream>
#include <QMessageBox>

#define DICT_PREFIX ":/dicts/dicts/"
#define DICT_SUFFIX ".dic"
#define DICT(_lang_) DICT_PREFIX _lang_ DICT_SUFFIX

/* Button indexes */
enum KB_KEYS {
	KEY_0, KEY_1, KEY_2, KEY_3,
	KEY_4, KEY_5, KEY_6, KEY_7,
	KEY_8, KEY_9,
	KEY_T9, KEY_BACKSPACE,
	KEY_SHIFT, KEY_SWITCH, KEY_ENTER
};

/* Button grids */
enum GRIDS {
	G_LOWER, G_UPPER, G_NUMBERS, G_SYMBOLS, G_SMILES
};

/* Dictionary names */
static const std::string dicts_str[] = {
	DICT("cz"), DICT("sk"), DICT("eng"), DICT("de")
};

/* Constructor */
TextHandler::TextHandler(QObject *parent) :
QObject(parent)
{
	/* Connect signals and slots */
	connect(this, SIGNAL(press(int, QString, QString, QString)), this, SLOT(onPress(int, QString, QString, QString)));
	connect(this, SIGNAL(pressLong(int)), this, SLOT(onPressLong(int)));
	connect(this, SIGNAL(dictChanged()), this, SLOT(onDictChanged()));
	connect(this, SIGNAL(t9Changed()), this, SLOT(onT9Changed()));

	/* Load last selected dictionary */
	loadDict();
}

/* Destructor */
TextHandler::~TextHandler()
{
	/* Save active dictionary */
	saveDict();

	/* Release T9 memory */
	if (T9) {
		delete T9;
	}
}

void TextHandler::choosePrediction(int index)
{
	/* Erase actual word */
	if (_wstart > 0) {
		setText(_text.left(_wstart));
	} else {
		setText("");
	}

	/* Insert new word */
	appendText(T9->getActWords()[index]);
}

QString TextHandler::predictionAt(int index)
{
	/* first is shown in text field */
	return T9->getActWords()[index];
}

/* Process move in T9 */
void TextHandler::processT9change()
{
	if (T9->getActWords().empty()) {
		setPredictCount(0);
		return;
	}

	/* Remove last word */
	if (_wstart > 0) {
		setText(_text.left(_wstart));
	} else {
		setText("");
	}

	/* Insert new word */
	appendText(T9->getActWords()[0]);
	setPredictCount(static_cast<int> (T9->getActWords().size()));
}

/* Process NON-T9 press */
void TextHandler::normalSolve(int index, QString sec)
{
	QChar result = normalInput.resolve(index, sec);
	if (normalInput.lastResult() == CHANGE_LAST) {
		removeLast();
	}
	appendText(QString(result));
}

/* Handle short press of one of base buttons (1 - 9) */
void TextHandler::basePress(int index, QString sec)
{
	if (_t9enabled && index != KEY_1) {
		T9->updatePositionByIndex(index);

		if (T9->getActWords().empty()) {
			/* No words in T9 -> normal solve */
			appendText(QString(sec[0]));
			//            normalSolve(index, sec);
			return;
		}

		/* Process new set of words from T9 */
		processT9change();
	} else {
		normalSolve(index, sec);
		resetT9Position();
		_wstart = _text.length();
	}
}

/* Remove last character from text */
void TextHandler::removeLast()
{
	if (_text.isEmpty()) {
		return;
	}

	_text.remove(_text.length() - 1, 1);

	if (_t9enabled) {
		/* Move up in T9 trie */
		T9->moveUp();

		/* update text and list of predicates */
		processT9change();
	}

	emit textChanged();
}

void TextHandler::onPress(int index, QString prim, QString sec, QString cent)
{
	/* Procces control buttons */
	switch (index) {
	case KEY_0:
		appendText(" ");
		_prevStart = _wstart;
		_wstart = _text.length();
		resetT9Position();
		setGrid(G_LOWER);
		break;
	case KEY_T9:
		switchT9();
		break;
	case KEY_BACKSPACE:
		if (_wstart == _text.length()) {
			_wstart--;
		}
		removeLast();
		break;
	case KEY_ENTER:
		appendText("\n");
		_prevStart = _wstart;
		_wstart = _text.length();
		resetT9Position();
		break;
	case KEY_SHIFT:
		/* Switch lower/upper case */
		switch (_grid) {
		case G_LOWER:
			setGrid(G_UPPER);
			break;
		case G_UPPER:
			setGrid(G_LOWER);
			break;
		default:
			break;
		}
		break;
	case KEY_SWITCH:
		switch (_grid) {
		case G_LOWER:
			/* skip upper - we have shift for this */
			setGrid(G_UPPER + 1);
			break;
		case G_SMILES:
			/* Last grid - go to beginning */
			setGrid(G_LOWER);
			break;
		default:
			setGrid(_grid + 1);
			break;
		}

		break;
	default:
		break;
	}

	/* procces button */
	if (index > KEY_0 && index <= KEY_9) {
		if (_grid > G_UPPER) {
			resetT9Position();
			normalInput.reset();
			appendText(cent);
		} else {
			basePress(index, sec);
		}
	} else {
		normalInput.reset();
	}
}

void TextHandler::onPressLong(int index)
{
	/*
	 * Ignore long press of control buttons and long press on other
	 * than lower/upper case grid
	 */
	if (index > KEY_9 || _grid > G_UPPER) {
		return;
	}

	appendText(QString::fromStdString(std::to_string(index)));
	resetT9Position();
}

void TextHandler::onDictChanged()
{
	if (_t9enabled) {
		reloadT9();
	}
}

void TextHandler::onT9Changed()
{
	if (_t9enabled) {
		reloadT9();
	}
}

void TextHandler::appendText(QString str)
{
	_text += str;
	emit textChanged();
}

void TextHandler::resetT9Position()
{
	if (T9) {
		T9->resetPosition();
		setPredictCount(0);
	}
}

void TextHandler::switchT9()
{
	setT9(!_t9enabled);
}

void TextHandler::loadDict()
{
	/*
	 * Don't enable T9 on startup, set active and selected dicts
	 * different so next time T9 is enabled it will be reloaded
	 */
	_dict = _settings.value("dict", DictClass::CZ).toInt();
	_activeDict = _dict + 1;
}

void TextHandler::saveDict()
{
	_settings.setValue("dict", _dict);
}

void TextHandler::reloadT9()
{
	/* Reload dictionary only when selected is different than loaded one */
	if (_activeDict != _dict) {
		if (T9) {
			delete T9;
		}
		T9 = new Trie();
		try {
			T9->loadFile(dicts_str[_dict]);
			_activeDict = _dict;
		} catch (std::exception &e) {
			/* Error when loading dictionary */
			QMessageBox mBox;
			mBox.setText(QString(e.what()));
			mBox.exec();

			/* Disable T9 */
			setT9(false);
		}
	}
}
