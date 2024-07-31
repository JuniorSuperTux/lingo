#include <dict_types.hpp>

bool Vocab::operator==(const Vocab& other) const {
	return this->classes == other.classes;
}

bool Vocab::operator!=(const Vocab& other) const {
	return !(*this == other);
}

WordClass::WordClass(const QString& attr) {
	this->attribute = attr;
}

bool WordClass::has_categories() const {
	Q_ASSERT(this->sub_initialized());
	return this->has_categories_v.has_value();
}

bool WordClass::sub_initialized() const {
	return this->has_categories_v.has_value();
}

void WordClass::init_categorys() {
	Q_ASSERT(!this->sub_initialized());
	this->sub = QVector<Category>{};
	this->has_categories_v = true;
}

void WordClass::init_definitions() {
	Q_ASSERT(!this->sub_initialized());
	this->sub = QVector<Definition>{};
	this->has_categories_v = false;
}

QVector<Category>& WordClass::get_category_ref() {
	qDebug() << "EXEC";
	return std::get<QVector<Category>>(this->sub);
}

QVector<Definition>& WordClass::get_definition_ref() {
	return std::get<QVector<Definition>>(this->sub);
}

QVector<Category> WordClass::get_category_value() {
	return std::get<QVector<Category>>(this->sub);
}

QVector<Definition> WordClass::get_definition_value() {
	return std::get<QVector<Definition>>(this->sub);
}

bool WordClass::operator==(const WordClass& other) const {
	return this->sub == other.sub &&
		   this->attribute == other.attribute;
}

bool WordClass::operator!=(const WordClass& other) const {
	return !(*this == other);
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

Definition::Definition(const QString& def) {
	this->definition = def;
}

bool Definition::operator==(const Definition& other) const {
	return this->definition == other.definition && this->sentences == other.sentences && this->phrases == other.phrases;
}

bool Definition::operator!=(const Definition& other) const {
	return !(*this == other);
}

bool Phrase::operator==(const Phrase& other) const {
	return this->phrase == other.phrase &&
		   this->def == other.def &&
		   this->sentences == other.sentences;
}

bool Phrase::operator!=(const Phrase& other) const {
	return !(*this == other);
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
