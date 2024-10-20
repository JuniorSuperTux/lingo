#include <dict_types.hpp>
#include <QCborValue>
#include <QCborMap>
#include <QCborArray>

template <typename T> static QCborValue vec_to_cbor(const QVector<T>& vec) {
	QCborArray ret;
	for (const T& entry : vec)
		ret.append(entry.gen_cbor());
	return ret;
}

bool Vocab::operator==(const Vocab& other) const {
	return this->classes == other.classes;
}

bool Vocab::operator!=(const Vocab& other) const {
	return !(*this == other);
}

QString Vocab::top_definition() const {
	if (!this->classes.size() ||
		!this->classes.at(0).categories.size() ||
		!this->classes.at(0).categories.at(0).definitions.size())
		return QString();
	return this->classes.at(0).categories.at(0).definitions.at(0).definition;
}

QCborValue Vocab::gen_cbor() const {
	return QCborMap{
		{"vocab", vocab},
		{"classes", vec_to_cbor(classes)}
	};
}

Vocab::Vocab(const QCborValue& target) {
	const QCborMap& map = target.toMap();
	const QCborArray& arr = map.value("classes").toArray();
	this->vocab = map.value("vocab").toString();
	this->classes.reserve(arr.size());
	for (const auto& item : arr)
		this->classes.emplace_back(item);
}

WordClass::WordClass(const QString& attr) {
	this->attribute = attr;
}

bool WordClass::operator==(const WordClass& other) const {
	return this->attribute == other.attribute &&
		   this->categories == other.categories;
}

bool WordClass::operator!=(const WordClass& other) const {
	return !(*this == other);
}

DelegateEnum::Delegate WordClass::determine_delegate() const {
	Q_ASSERT(this->categories.size());
	if (this->categories.size() == 1 &&
		this->categories.at(0).category.isEmpty()) {
		return this->categories.at(0).determine_delegate();
	}
	return DelegateEnum::Category;
}

QVector<Definition> WordClass::shortcut_definitions() const {
	Q_ASSERT(this->determine_delegate() == DelegateEnum::Definition);
	return this->categories.at(0).definitions;
}

QVector<Phrase> WordClass::shortcut_phrases() const {
	Q_ASSERT(this->determine_delegate() == DelegateEnum::Phrase);
	return this->categories.at(0).definitions.at(0).phrases;
}

QCborValue WordClass::gen_cbor() const {
	return QCborMap{
		{"attribute", this->attribute},
		{"categories", vec_to_cbor(this->categories)}
	};
}

WordClass::WordClass(const QCborValue& target) {
	const QCborMap& map = target.toMap();
	const QCborArray& arr = map.value("categories").toArray();
	this->attribute = map.value("attribute").toString();
	this->categories.reserve(arr.size());
	for (const auto& item : arr)
		this->categories.emplace_back(item);
}

Category::Category(const QString& spec) {
	this->category = spec;
}

bool Category::operator==(const Category& other) const {
	return this->category == other.category &&
		   this->definitions == other.definitions;
}

bool Category::operator!=(const Category& other) const {
	return !(*this == other);
}

DelegateEnum::Delegate Category::determine_delegate() const {
	Q_ASSERT(this->definitions.size());
	if (this->definitions.size() == 1 &&
		this->definitions.at(0).definition.isEmpty())
		return DelegateEnum::Phrase;
	return DelegateEnum::Definition;
}

DelegateEnum::Delegate Category::repr() {
	return DelegateEnum::Category;
}

QVector<Phrase> Category::shortcut_phrases() const {
	Q_ASSERT(this->determine_delegate() == DelegateEnum::Phrase);
	return this->definitions.at(0).phrases;
}

QCborValue Category::gen_cbor() const {
	return QCborMap{
		{"category", this->category},
		{"definitions", vec_to_cbor(this->definitions)}
	};
}

Category::Category(const QCborValue& target) {
	const QCborMap& map = target.toMap();
	const QCborArray& arr = map.value("definitions").toArray();
	this->category = map.value("category").toString();
	this->definitions.reserve(arr.size());
	for (const auto& item : arr)
		this->definitions.emplace_back(item);
}

Definition::Definition(const QString& def) {
	this->definition = def;
}

bool Definition::operator==(const Definition& other) const {
	return this->definition == other.definition && this->sentences == other.sentences && this->phrases == other.phrases;
}

bool Definition::operator!=(const Definition& other) const {
	return !(*this == other);
}

DelegateEnum::Delegate Definition::repr() {
	return DelegateEnum::Definition;
}

QCborValue Definition::gen_cbor() const {
	return QCborMap{
		{"definition", this->definition},
		{"sentences", vec_to_cbor(this->sentences)},
		{"phrases", vec_to_cbor(this->phrases)},
	};
}

Definition::Definition(const QCborValue& target) {
	const QCborMap& map = target.toMap();
	const QCborArray& sentences_arr = map.value("sentences").toArray();
	const QCborArray& phrases_arr = map.value("phrases").toArray();
	this->definition = map.value("definition").toString();
	this->sentences.reserve(sentences_arr.size());
	this->phrases.reserve(phrases_arr.size());
	for (const auto& item : sentences_arr)
		this->sentences.emplace_back(item);
	for (const auto& item : phrases_arr)
		this->phrases.emplace_back(item);
}

bool Phrase::operator==(const Phrase& other) const {
	return this->phrase == other.phrase &&
		   this->def == other.def &&
		   this->sentences == other.sentences;
}

bool Phrase::operator!=(const Phrase& other) const {
	return !(*this == other);
}

DelegateEnum::Delegate Phrase::repr() {
	return DelegateEnum::Phrase;
}

QCborValue Phrase::gen_cbor() const {
	return QCborMap{
		{"phrase", this->phrase},
		{"def", this->def},
		{"chi_def", this->chi_def},
		{"sentences", vec_to_cbor(this->sentences)}
	};
}

Phrase::Phrase(const QCborValue& target) {
	const QCborMap& map = target.toMap();
	const QCborArray& arr = map.value("sentences").toArray();
	this->phrase = map.value("phrase").toString();
	this->def = map.value("def").toString();
	this->chi_def = map.value("chi_def").toString();
	this->sentences.reserve(arr.size());
	for (const auto& item : arr)
		this->sentences.emplace_back(item);
}

Sentence::Sentence(const QString& eng, const QString& chi) {
	this->eng = eng;
	this->chi = chi;
}

Sentence::Sentence(const QString& eng) {
	this->eng = eng;
}

bool Sentence::operator==(const Sentence& other) const {
	return this->eng == other.eng && this->chi == other.chi;
}

bool Sentence::operator!=(const Sentence& other) const {
	return !(*this == other);
}

QCborValue Sentence::gen_cbor() const {
	return QCborMap{
		{"chi", this->chi},
		{"eng", this->eng}
	};
}

Sentence::Sentence(const QCborValue& target) {
	const QCborMap& map = target.toMap();
	this->chi = map.value("chi").toString();
	this->eng = map.value("eng").toString();
}
