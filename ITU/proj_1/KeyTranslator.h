#ifndef KEYTRANSLATOR_H
#define KEYTRANSLATOR_H

#include <QObject>
#include <QSettings>
#include <QString>
#include <QKeySequence>
#include <vector>
#include "Trie.h"

class KeyTranslator : public QObject
{
    Q_OBJECT
public:
    explicit KeyTranslator(QObject *parent = 0);
    ~KeyTranslator();

    /** Translate key to string */
    Q_INVOKABLE QString key2str(int key);

    /** Translate string to key code */
    Q_INVOKABLE int str2key(QString str) { return QKeySequence(str)[0]; }

    /** Get key for given index */
    Q_INVOKABLE int getKey(int index) { return _keys[index]; }

    /** Save key for given index */
    Q_INVOKABLE void saveKey(int index, int key) { _keys[index] = key; }

    /** Total number of keys */
    Q_INVOKABLE int keyCount() { return static_cast<int>(_keys.size()); }

private:

    /**
     * @brief Load keys from settings
     */
    void loadKeys();

    /**
     * @brief Save key bindings into settings
     */
    void saveKeys();

    QSettings _settings;    /**< Program settings */

    std::vector<int> _keys{{   /**< Default key bindings */
        Qt::Key_0, /* space */
        Qt::Key_7, Qt::Key_8, Qt::Key_9,
        Qt::Key_4, Qt::Key_5, Qt::Key_6,
        Qt::Key_1, Qt::Key_2, Qt::Key_3,
        Qt::Key_Minus,     /* T9 button */
        Qt::Key_Backspace, /* backspace */
        Qt::Key_Shift,     /* shift */
        Qt::Key_Right,     /* switcher */
        Qt::Key_Enter      /* new line */
    }};
};

#endif // KEYTRANSLATOR_H
