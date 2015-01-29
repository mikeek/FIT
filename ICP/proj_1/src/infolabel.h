/** \file infolabel.h
 *  \author Lukáš Huták, xhutak01
 *	\author Michal Kozubík, xkozub03
 */


#ifndef INFOLABEL_H
#define INFOLABEL_H

#include <QLabel>

/**
 * \class infoLabel
 * \brief Třída pro lepší zobrazení statistik hráčů po hře - pro budoucí vývoj
 */
class infoLabel : public QLabel {
	//    Q_OBJECT
public:
	explicit infoLabel(QWidget *parent = 0);

signals:

	public slots :

};

#endif // INFOLABEL_H
