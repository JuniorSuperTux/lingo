#include <QXmlStreamReader>
#include <QStringView>
#include <parser.hpp>
#include <dict_types.hpp>
#include <utils.hpp>

// Reading order:
// pos dpos -> WordClass
// Next line of "guideword dsense_gw" -> Categorization
// Warning: Category might not exist for words
// trans dtrans dtrans-se  break-cj -> Definition
// phrase-title dphrase-title -> Phrase
// eg deg -> English sentence
// trans dtrans dtrans-se hdb break-cj -> Chinese sentence

inline static bool is_vocab(QXmlStreamReader& xml) {
	return xml.name() == u"b" &&
		   xml.attributes().hasAttribute("class") &&
		   xml.attributes().value("class") == u"tb ttn";
}

inline static QString get_vocab(QXmlStreamReader& xml) {
	return xml.readElementText();
}

inline static bool is_wordclass(QXmlStreamReader& xml) {
	return xml.name() == u"span" &&
		   xml.attributes().hasAttribute("class") &&
		   xml.attributes().value("class") == u"pos dpos";
}

inline static QString get_wordclass(QXmlStreamReader& xml) {
	return xml.readElementText();
}

inline static bool is_category(QXmlStreamReader& xml) {
	return xml.name() == u"span" &&
		   xml.attributes().hasAttribute("class") &&
		   xml.attributes().value("class") == u"guideword dsense_gw";
}

inline static QString get_category(QXmlStreamReader& xml) {
	QString category = xml.readElementText(QXmlStreamReader::IncludeChildElements);
	// Remove the parathenses
	category.removeFirst().removeLast();
	return category;
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
			 xml.attributes().value("class") == u"def ddef_d db")) {
		xml.readNextStartElement();
	}
	return xml.readElementText(QXmlStreamReader::IncludeChildElements);
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

static void recur_create_append(Vocab& target, const Definition& item) {
	Q_ASSERT(target.classes.size());

	WordClass& wordclass = target.classes.back();
	if (!(wordclass.categories.size()))
		wordclass.categories.emplace_back();

	Category& category = wordclass.categories.back();
	category.definitions.push_back(item);
}

static void recur_create_append(Vocab& target, const Phrase& item) {
	Q_ASSERT(target.classes.size());

	WordClass& wordclass = target.classes.back();
	if (!(wordclass.categories.size()))
		wordclass.categories.emplace_back();
	Category& category = wordclass.categories.back();
	if (!(category.definitions.size()))
		category.definitions.emplace_back();

	Definition& definition = category.definitions.back();
	definition.phrases.push_back(item);
}


static Definition& ref_definition(Vocab& target) {
	WordClass& wordclass = target.classes.back();
	Category& category = wordclass.categories.back();
	return category.definitions.back();
}

static Phrase& ref_phrase(Vocab& target) {
	return ref_definition(target).phrases.back();
}

Vocab parse(const QString& data) {
	QXmlStreamReader xml(data);
	// xml.setEntityResolver(&entity_resolver);
	Vocab ret;
	while(!xml.atEnd() && !xml.hasError()) {
		xml.readNextStartElement();
		if (is_vocab(xml)) {
			qDebug() << "vocab";
			ret.vocab = get_vocab(xml);
			break;
		}
	}

	bool write_phrase = false;
	while (!xml.atEnd() && !xml.hasError()) {
		xml.readNextStartElement();
		if (is_wordclass(xml)) {
			qDebug() << "wordclass";
			write_phrase = false;
			ret.classes.emplace_back(get_wordclass(xml));
		}
		else if (is_category(xml)) {
			qDebug() << "category";
			write_phrase = false;
			WordClass& target = ret.classes.back();
			target.categories.emplace_back(get_category(xml));
		}
		else if (is_definition(xml)) {
			qDebug() << "definition";
			if (!write_phrase)
				recur_create_append(ret, Definition(get_definition(xml)));
			else
				ref_phrase(ret).chi_def = get_definition(xml);
		}
		else if (is_phrase(xml)) {
			qDebug() << "phrase";
			write_phrase = true;
			Phrase tmp;
			tmp.phrase = get_phrase_phrase(xml);
			tmp.def = get_phrase_def(xml);
			recur_create_append(ret, tmp);
		}
		else if (is_eng_sentence(xml)) {
			qDebug() << "eng_sentence";
			// Phrases usually appears after all the other definitions
			// English sentences are garunteed while Chinese sentences are not
			if (!write_phrase)
				ref_definition(ret).sentences.emplace_back(get_eng_sentence(xml));
			else
				ref_phrase(ret).sentences.emplace_back(get_eng_sentence(xml));
		}
		else if (is_chi_sentence(xml)) {
			qDebug() << "chi_sentence";
			// Phrases usually appears after all the other definitions
			// English sentences are garunteed while Chinese sentences are not
			if (!write_phrase)
				ref_definition(ret).sentences.back().chi = get_chi_sentence(xml);
			else
				ref_phrase(ret).sentences.back().chi = get_chi_sentence(xml);
		}
	}
	if (xml.hasError()) {
		qDebug() << "XML ERROR";
		qDebug() << "Line Number:" << xml.lineNumber();
		qDebug() << "Column Number:" << xml.columnNumber();
		qDebug() << "Char. Offset:" << xml.characterOffset();
		qFatal() << xml.errorString();
		THROW_ERR("Error while parsing vocabulary XML: ", xml.errorString());
	}

	return ret;
}
