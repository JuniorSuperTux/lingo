#include <QXmlStreamReader>
#include <QStringView>
#include <parser.hpp>
#include <dict_types.hpp>

// Reading order:
// pos dpos -> WordClass
// Next line of "guideword dsense_gw" -> Categorization
// Warning: Category might not exist for words
// trans dtrans dtrans-se  break-cj -> Definition
// phrase-title dphrase-title -> Phrase
// eg deg -> English sentence
// trans dtrans dtrans-se hdb break-cj -> Chinese sentence

inline static bool is_wordclass(QXmlStreamReader& xml) {
	return xml.name() == u"span" &&
		xml.attributes().hasAttribute("class") &&
		xml.attributes().value("class") == u"pos dpos";
}

static QString get_wordclass(QXmlStreamReader& xml) {
	return xml.readElementText();
}

inline static bool is_category(QXmlStreamReader& xml) {
	return xml.name() == u"span" &&
		xml.attributes().hasAttribute("class") &&
		xml.attributes().value("class") == u"guideword dsense_gw";
}

static QString get_category(QXmlStreamReader& xml) {
	QString category = xml.readElementText(QXmlStreamReader::IncludeChildElements);
	// Remove the parathenses
	category.removeFirst().removeLast();
	return category;
//	while (!(xml.tokenType() == QXmlStreamReader::StartElement && xml.name() == u"span")) {
//		if (xml.hasError())
//			throw(std::runtime_error(xml.errorString().toStdString()));
//		Q_ASSERT(!xml.atEnd());
//		xml.readNext();
//	}
//	return xml.readElementText();
}

inline static bool is_definition(QXmlStreamReader& xml) {
	return xml.name() == u"span" &&
		xml.attributes().hasAttribute("class") &&
		xml.attributes().value("class") == u"trans dtrans dtrans-se break-cj";
}

static QString get_definition(QXmlStreamReader& xml) {
	return xml.readElementText(QXmlStreamReader::IncludeChildElements);
}

inline static bool is_eng_sentence(QXmlStreamReader& xml) {
	return xml.name() == u"span" &&
		xml.attributes().hasAttribute("class") &&
		xml.attributes().value("class") == u"eg deg";
}

static QString get_eng_sentence(QXmlStreamReader& xml) {
	return xml.readElementText(QXmlStreamReader::IncludeChildElements);
}

inline static bool is_chi_sentence(QXmlStreamReader& xml) {
	return xml.name() == u"span" &&
		xml.attributes().hasAttribute("class") &&
		xml.attributes().value("class") == u"trans dtrans dtrans-se hdb break-cj";
}

static QString get_chi_sentence(QXmlStreamReader& xml) {
	return xml.readElementText();
}

inline static bool is_phrase(QXmlStreamReader& xml) {
	return xml.name() == u"span" &&
		xml.attributes().hasAttribute("class") &&
		xml.attributes().value("class") == u"phrase-title dphrase-title";
}

static QString get_phrase_phrase(QXmlStreamReader& xml) {
	return xml.readElementText(QXmlStreamReader::IncludeChildElements);
}

static QString get_phrase_def(QXmlStreamReader& xml) {
	while (!(xml.hasError() || xml.atEnd()) && !(xml.name() == u"div" &&
			 xml.attributes().hasAttribute("class") &&
			 xml.attributes().value("class") == u"ddef_h")) {
//			 xml.attributes().value("class") == u"def ddef_d db")) {
		xml.readNextStartElement();
	}
	return xml.readElementText(QXmlStreamReader::IncludeChildElements);
}

static QVector<Definition>& skip_to_def(WordClass& target) {
	Q_ASSERT(target.sub_initialized());
	if (target.has_categories())
		return target.get_category_ref().back().definitions;
	return target.get_definition_ref();
}

// class CustomEntityResolver : public QXmlStreamEntityResolver {
// public:
// 	CustomEntityResolver() {
// 		entityMap["nbsp"] = "&#160;";
// 	}
// 	QString resolveUndeclaredEntity(const QString& name) override {
// 		if (entityMap.contains(name)) {
// 			return entityMap[name];
// 		} else {
// 			return QString();
// 		}
// 	}
// private:
// 	QMap<QString, QString> entityMap;
// } entity_resolver;

Vocab parse(const QString& data) {
	QXmlStreamReader xml(data);
	// xml.setEntityResolver(&entity_resolver);
	Vocab ret;

	while (!xml.atEnd() && !xml.hasError()) {
		xml.readNextStartElement();
		if (is_wordclass(xml)) {
			qDebug() << "wordclass";
			ret.classes.emplace_back(get_wordclass(xml));
		}
		else if (is_category(xml)) {
			qDebug() << "category";
			WordClass& target = ret.classes.back();
			if (target.sub_initialized())
				Q_ASSERT(target.has_categories());
			if (!target.sub_initialized())
				target.init_categorys();
			target.get_category_ref().emplace_back(get_category(xml));
		}
		else if (is_definition(xml)) {
			qDebug() << "definition";
			if (!ret.classes.back().sub_initialized())
				ret.classes.back().init_definitions();
			QVector<Definition>& vec = skip_to_def(ret.classes.back());
			vec.emplace_back(get_definition(xml));
		}
		else if (is_phrase(xml)) {
			qDebug() << "phrase";
			QVector<Definition>& vec = skip_to_def(ret.classes.back());
			// Some phrases for some vocabs don't have definition
			if (!vec.size())
				vec.emplace_back("");
			Phrase ret;
			ret.phrase = get_phrase_phrase(xml);
			ret.def = get_phrase_def(xml);
			vec.back().phrases.push_back(ret);
		}
		else if (is_eng_sentence(xml)) {
			qDebug() << "eng_sentence";
			Definition& target = skip_to_def(ret.classes.back()).back();
			// Phrases usually appears after all the other definitions
			// English sentences are garunteed while Chinese sentences are not
			if (!target.phrases.size())
				target.sentences.emplace_back(get_eng_sentence(xml));
			else
				target.phrases.back().sentences.emplace_back(get_eng_sentence(xml));
		}
		else if (is_chi_sentence(xml)) {
			qDebug() << "chi_sentence";
			Definition& target = skip_to_def(ret.classes.back()).back();
			// Phrases usually appears after all the other definitions
			// English sentences are garunteed while Chinese sentences are not
			if (!target.phrases.size())
				target.sentences.back().chi = get_chi_sentence(xml);
			else
				target.phrases.back().sentences.back().chi = get_chi_sentence(xml);

			}
	}
	if (xml.hasError()) {
		qDebug() << "XML ERROR";
		qDebug() << "  Line Number:" << xml.lineNumber();
		qDebug() << "Column Number:" << xml.columnNumber();
		qDebug() << " Char. Offset:" << xml.characterOffset();
		qFatal() << xml.errorString();
	}

	return ret;
}
