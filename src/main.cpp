#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <parser.hpp>
#include <dict_types.hpp>
#include <fstream>
#include <sstream>
#include <QQmlContext>
#include <xhtml_converter.hpp>

template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
	std::ifstream test("test");
	std::stringstream buffer;
	buffer << test.rdbuf();
	std::string tmp;
	QString all;
	all = QString::fromStdString(buffer.str());
	qDebug() << "PROGRAM STARTO";
	Vocab res = parse(html_to_xhtml(all));
	for (const auto& wordClass : res.classes) {
		qDebug() << "Attribute:" << wordClass.attribute;
		for (const auto& category : wordClass.categories) {
					   qDebug() << "  Category:" << category.category;
			for (const auto& definition : category.definitions) {
				qDebug() << "    Definition:" << definition.definition;
				for (const auto& sentence : definition.sentences) {
					qDebug() << "      Sentence (ENG):" << sentence.eng;
					qDebug() << "      Sentence (CHI):" << sentence.chi;
				}
				for (const auto& phrase : definition.phrases) {
					qDebug() << "      PHRASE  " << phrase.phrase;
					qDebug() << "      PHRASE MEANING " << phrase.def;
					for (const auto& sentence : phrase.sentences) {
						qDebug() << "        Sentence (ENG):" << sentence.eng;
						qDebug() << "        Sentence (CHI):" << sentence.chi;
					}
				}
			}
		}
	}
    QQmlApplicationEngine engine;
	const QUrl url(u"qrc:/lingo/qml/Main.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
	Wrapper wrapper(res);
	engine.rootContext()->setContextProperty("gae", &wrapper);
    engine.load(url);

    return app.exec();
}
