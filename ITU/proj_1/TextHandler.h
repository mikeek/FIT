#ifndef TEXTHANDLER_H
#define TEXTHANDLER_H

#include <iostream>

#include <QObject>
#include <QSettings>
#include <QString>
#include "Trie.h"
#include "NormalInput.h"

/* Dictionary enum accesible from QML */
class DictClass : public QObject
{
    Q_OBJECT
public:
    enum enumDict {
        CZ, SK, EN, DE
    };

    Q_ENUMS(enumDict)
};

/* Everything handler */
class TextHandler : public QObject
{
    Q_OBJECT

public:
    explicit TextHandler(QObject *parent = 0);
    ~TextHandler();

    /* QML mapping */
    Q_PROPERTY(int dict MEMBER _dict READ getDict WRITE setDict NOTIFY dictChanged)
    Q_PROPERTY(int grid MEMBER _grid READ getGrid WRITE setGrid NOTIFY gridChanged)
    Q_PROPERTY(QString text MEMBER _text READ getText WRITE setText NOTIFY textChanged)
    Q_PROPERTY(bool t9enabled MEMBER _t9enabled READ getT9 WRITE setT9 NOTIFY t9Changed)
    Q_PROPERTY(int predictCount MEMBER _t9predictions READ getPredictCount WRITE setPredictCount NOTIFY predictCountChanged)

    /* Dictionary get/set */
    int getDict()  { return _dict; }
    void setDict(int nDict) { _dict = nDict; emit dictChanged(); }

    /* T9 get/set */
    bool getT9() { return _t9enabled; }
    void setT9(bool nT9 = true) { _t9enabled = nT9; emit t9Changed(); }

    /* Text get/set */
    QString getText() { return _text; }
    void setText(QString nText) { _text = nText; emit textChanged(); }

    /* Grid get/set */
    int getGrid() { return _grid; }
    void setGrid(int nGrid) { _grid = nGrid; emit gridChanged(); }

    /* Predictions get/set */
    int getPredictCount() { return _t9predictions; }
    void setPredictCount(int nCount) { _t9predictions = nCount; emit predictCountChanged(); }
    Q_INVOKABLE QString predictionAt(int index);
    Q_INVOKABLE void choosePrediction(int index);

signals:
    void dictChanged();
    void t9Changed();
    void press(int index, QString prim = "", QString sec = "", QString cent = "");
    void pressLong(int index);
    void textChanged();
    void gridChanged();
    void predictCountChanged();

private slots:
    void onDictChanged();
    void onT9Changed();
    void onPress(int index, QString prim, QString sec, QString cent);
    void onPressLong(int index);

private:

    void loadDict();
    void saveDict();
    void reloadT9();
    void switchT9();
    void resetT9Position();
    void appendText(QString str);
    void basePress(int index, QString sec);
    void removeLast();
    void processT9change();
    void normalSolve(int index, QString sec);

    int _t9predictions{};   /**< number of predicted words */
    int _wstart{}, _prevStart{}; /**< starting position of last word */
    int _grid{};            /**< active button grid */
    int _dict{};            /**< selected dictionary */
    int _activeDict{};      /**< active dictionary */
    bool _t9enabled{false}; /**< t9 flag */
    QString _text;          /**< Displayed text */
    Trie *T9{};             /**< T9 structure */
    QSettings _settings;    /**< Program settings */
    NormalInput normalInput;/**< normal input resolver */
};

#endif // TEXTHANDLER_H
