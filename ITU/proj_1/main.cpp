#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QtQuick/qquickitem.h>
#include <QtQuick/qquickview.h>

#include "KeyTranslator.h"
#include "TextHandler.h"

int main(int argc, char *argv[])
{
	QCoreApplication::setOrganizationName("KoHu");
	QCoreApplication::setApplicationName("ITU");

	QApplication app(argc, argv);

	KeyTranslator translator;
	TextHandler textHandler;

	QQmlApplicationEngine engine;

	/* Dictionary enum */
	qmlRegisterType<DictClass>("ITU", 1, 0, "Dict");

	/* Key translator and textHandler (aka "everything solver") */
	engine.rootContext()->setContextProperty("trans", &translator);
	engine.rootContext()->setContextProperty("textHandler", &textHandler);

	/* Let the hunt begin! */
	engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

	return app.exec();
}
