#pragma once
#include <QQmlEngine>

class DelegateEnum : public QObject {
	Q_OBJECT
	QML_SINGLETON
	QML_ELEMENT
public:
	enum Delegate {
		Wordclass, Category, Definition, Phrase
	};
	Q_ENUM(Delegate)
};

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

	QCborValue gen_cbor() const;
	explicit Sentence(const QCborValue&);
};

class Phrase {
	Q_GADGET
	Q_PROPERTY(QString phrase MEMBER phrase CONSTANT)
	Q_PROPERTY(QString def MEMBER def CONSTANT)
	Q_PROPERTY(QString chi_def MEMBER chi_def CONSTANT)
	Q_PROPERTY(QVector<Sentence> sentences MEMBER sentences CONSTANT)
	Q_PROPERTY(DelegateEnum::Delegate repr READ repr CONSTANT)
	QML_VALUE_TYPE(phrase_t)
public:
	static DelegateEnum::Delegate repr();

	QString phrase;
	QString def;
	QString chi_def;
	Phrase() = default;
	QVector<Sentence> sentences;
	bool operator==(const Phrase& other) const;
	bool operator!=(const Phrase& other) const;

	QCborValue gen_cbor() const;
	explicit Phrase(const QCborValue&);
};

class Definition {
	Q_GADGET
	Q_PROPERTY(QString definition MEMBER definition CONSTANT)
	Q_PROPERTY(QVector<Sentence> sentences MEMBER sentences CONSTANT)
	Q_PROPERTY(QVector<Phrase> phrases MEMBER phrases CONSTANT)
	Q_PROPERTY(DelegateEnum::Delegate repr READ repr CONSTANT)
	QML_VALUE_TYPE(definition_t)
public:
	static DelegateEnum::Delegate repr();

	QString definition;
	QVector<Sentence> sentences;
	QVector<Phrase> phrases;
	Definition() = default;
	explicit Definition(const QString& def);
	bool operator==(const Definition& other) const;
	bool operator!=(const Definition& other) const;

	QCborValue gen_cbor() const;
	explicit Definition(const QCborValue&);
};

class Category {
	Q_GADGET
	Q_PROPERTY(QString category MEMBER category CONSTANT)
	Q_PROPERTY(QVector<Definition> definitions MEMBER definitions CONSTANT)
	Q_PROPERTY(DelegateEnum::Delegate repr READ repr CONSTANT)
	QML_VALUE_TYPE(category_t)
public:
	static DelegateEnum::Delegate repr();
	Q_INVOKABLE DelegateEnum::Delegate determine_delegate() const;
	Q_INVOKABLE QVector<Phrase> shortcut_phrases() const;

	QString category;
	QVector<Definition> definitions;
	Category() = default;
	explicit Category(const QString& spec);
	bool operator==(const Category& other) const;
	bool operator!=(const Category& other) const;

	QCborValue gen_cbor() const;
	explicit Category(const QCborValue&);
};

class WordClass {
	Q_GADGET
	Q_PROPERTY(QString attribute MEMBER attribute CONSTANT)
	Q_PROPERTY(QVector<Category> categories MEMBER categories CONSTANT)
	QML_VALUE_TYPE(wordclass_t)
public:
	Q_INVOKABLE DelegateEnum::Delegate determine_delegate() const;
	Q_INVOKABLE QVector<Definition> shortcut_definitions() const;
	Q_INVOKABLE QVector<Phrase> shortcut_phrases() const;

	QString attribute;
	QVector<Category> categories;
	WordClass() = default;
	explicit WordClass(const QString& attr);
	bool operator==(const WordClass& other) const;
	bool operator!=(const WordClass& other) const;

	QCborValue gen_cbor() const;
	explicit WordClass(const QCborValue&);
};

class Vocab {
	Q_GADGET
	Q_PROPERTY(QVector<WordClass> classes MEMBER classes CONSTANT)
	QML_VALUE_TYPE(vocab_t)
public:
	Vocab() = default;
	QString vocab;
	QVector<WordClass> classes;
	bool operator==(const Vocab& other) const;
	bool operator!=(const Vocab& other) const;

	QString top_definition() const;
	QCborValue gen_cbor() const;
	explicit Vocab(const QCborValue&);
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
