#pragma once
#include <QQmlEngine>

class Sentence {
	Q_GADGET
	Q_PROPERTY(QString eng MEMBER eng CONSTANT)
	Q_PROPERTY(QString chi MEMBER chi CONSTANT)
	QML_VALUE_TYPE(sentence_t)
public:
	QString eng;
	QString chi;
	Sentence() = default;
	Sentence(const QString& eng);
	Sentence(const QString& eng, const QString& chi);
	bool operator==(const Sentence& other) const;
	bool operator!=(const Sentence& other) const;
};

class Phrase {
	Q_GADGET
	Q_PROPERTY(QString phrase MEMBER phrase CONSTANT)
	Q_PROPERTY(QString def MEMBER def CONSTANT)
	Q_PROPERTY(QVector<Sentence> sentences MEMBER sentences CONSTANT)
	QML_VALUE_TYPE(phrase_t)
  public:
	QString phrase;
	QString def;
	Phrase() = default;
	QVector<Sentence> sentences;
	bool operator==(const Phrase& other) const;
	bool operator!=(const Phrase& other) const;
};

class Definition {
	Q_GADGET
	Q_PROPERTY(QString definition MEMBER definition CONSTANT)
	Q_PROPERTY(QVector<Sentence> sentences MEMBER sentences CONSTANT)
	Q_PROPERTY(QVector<Phrase> phrases MEMBER phrases CONSTANT)
	QML_VALUE_TYPE(definition_t)
public:
	QString definition;
	QVector<Sentence> sentences;
	QVector<Phrase> phrases;
	Definition() = default;
	Definition(const QString& def);
	bool operator==(const Definition& other) const;
	bool operator!=(const Definition& other) const;
};

class Category {
	Q_GADGET
	Q_PROPERTY(QString category MEMBER category CONSTANT)
	Q_PROPERTY(QVector<Definition> definitions MEMBER definitions CONSTANT)
	QML_VALUE_TYPE(category_t)
public:
	QString category;
	QVector<Definition> definitions;
	Category() = default;
	Category(const QString& spec);
	bool operator==(const Category& other) const;
	bool operator!=(const Category& other) const;
};

class WordClass {
	Q_GADGET
	Q_PROPERTY(QString attribute MEMBER attribute)
	QML_VALUE_TYPE(wordclass_t)
public:
	QString attribute;
	std::variant<QVector<Category>, QVector<Definition>> sub;

	Q_INVOKABLE bool has_categories() const;
	QVector<Category>& get_category_ref();
	QVector<Definition>& get_definition_ref();
	Q_INVOKABLE QVector<Category> get_category_value();
	Q_INVOKABLE QVector<Definition> get_definition_value();
	bool sub_initialized() const;

	void init_categorys();
	void init_definitions();

	WordClass() = default;
	WordClass(const QString& attr);
	bool operator==(const WordClass& other) const;
	bool operator!=(const WordClass& other) const;
private:
	std::optional<bool> has_categories_v;
};

class Vocab {
	Q_GADGET
	Q_PROPERTY(QVector<WordClass> classes MEMBER classes CONSTANT)
	QML_VALUE_TYPE(vocab_t)
public:
	Vocab() = default;
	QVector<WordClass> classes;
	bool operator==(const Vocab& other) const;
	bool operator!=(const Vocab& other) const;
};

class Wrapper : public QObject {
	Q_OBJECT
	QML_ELEMENT
public:
	Wrapper(const Vocab& other) : voc{other} {}
	Q_INVOKABLE Vocab get() {
		return this->voc;
	}
private:
	Vocab voc;
};
