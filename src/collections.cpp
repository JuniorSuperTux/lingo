#include <collections.hpp>
#include <QFile>
#include <QStandardPaths>
#include <QCborStreamReader>
#include <utils.hpp>
#include <QDirIterator>

QVector<CollectionManager*> active_managers;
QMutex managers_guard;

CollectionManager::CollectionManager(const QString& name)
	: name {name} {

	QJSEngine::setObjectOwnership(this, QJSEngine::CppOwnership);
	qDebug() << "Loading collection" << name;

	const QString path = QStandardPaths::locate(QStandardPaths::AppDataLocation,
												QString("collections/%1.cbor").arg(name));
	if (!path.size())
		THROW_ERR("Cannot find collection \"%1\"", name);
	QFile file(path);
	if (!file.open(QIODevice::ReadOnly))
		THROW_ERR("Failed to open collection %1", name);
	QCborStreamReader reader(&file);
	QCborArray vocab_array = QCborValue::fromCbor(reader).toArray();
	if (!(reader.lastError() == QCborError::NoError))
		THROW_ERR("Failed to parse CBOR of collection %1", name);
	for (const auto& item : vocab_array)
		this->vocabs.emplace_back(item);
}

int CollectionManager::rowCount(const QModelIndex& parent = QModelIndex()) const {
	Q_UNUSED(parent);
	return vocabs.size();
}

QVariant CollectionManager::data(const QModelIndex& index, int role = Qt::DisplayRole) const {
	if (!index.isValid())
		return QVariant();
	if (index.row() <= 0 || index.row() >= vocabs.size())
		return QVariant();
	switch (role) {
	case VocabRole:
		return vocabs.at(index.row()).vocab;
	case DefinitionRole:
		return vocabs.at(index.row()).top_definition();
	default:
		return QVariant();
	}
}

QHash<int, QByteArray> CollectionManager::roleNames() const {
	return QHash<int, QByteArray>{
		{VocabRole, "vocab"},
		{DefinitionRole, "definition"}
	};
}

void CollectionManager::listview_disown() {
	mutex.lock();
	if (task_cnt <= 0) {
		qDebug() << "Deleting collection manager" << this->name;
		managers_guard.lock();
		Q_ASSERT(active_managers.removeOne(this));
		delete this;
		managers_guard.unlock();
		qDebug() << "Deleted";
	}
	else
		mutex.unlock();
	// TODO: NO NEED UNLOCK AGAIN CAUSE DELETED?
	// mutex.unlock();
	// TODO: Also check for cleanup in worker threads
}

Collections::Collections() {
	this->update_collections();
}

void Collections::update_collections() {
	const QString path = QStandardPaths::locate(QStandardPaths::AppDataLocation,
												"collections",
												QStandardPaths::LocateDirectory);
	qDebug() << path;
	QDir dir(path);
	if (!dir.exists())
		THROW_ERR("Collection directory \"%1\" does not exist", path);
	QDirIterator it(QString("%1/").arg(path));
	QStringList found_collections;
	while (it.hasNext()) {
		it.next();
		QString name = it.fileName();
		if (!name.endsWith(".cbor"))
			continue;
		name = name.chopped(5);
		found_collections.emplace_back(name);
	}
	if (collections != found_collections)
		collections = found_collections;
	emit collectionsChanged();
}

void Collections::create_collection(const QString& name) {
	qDebug() << "Creating collection " << name;
	const QString path = QString("%1/collections")
							 .arg(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
	qDebug() << "PATH: " << path;
	QDir dir(path);
	if (!dir.exists()) {
		if (!dir.mkpath(path))
			THROW_ERR("Failed to create directory \"%1\" to store collections", path);
	}
	qDebug() << "Still alive";
	QCborArray empty_arr;
	QByteArray cbor_data = empty_arr.toCborValue().toCbor();
	QFile file(QString("%1/%2.cbor").arg(path, name));
	file.open(QFile::WriteOnly);
	file.write(cbor_data);
	file.close();
	qDebug() << "Signal emitted";
	emit collectionsChanged();
}

void Collections::delete_collection(const QString& name) {
	const QString path = QStandardPaths::locate(QStandardPaths::AppDataLocation,
												QString("collections/%1.cbor").arg(name));
	if(!path.size())
		THROW_ERR("Cannot find collection \"%1\" for deletion", name);
	QFile target_collection(path);
	if (!target_collection.remove())
		THROW_ERR("Failed to delete collection \"%1\"", name);
	emit collectionsChanged();
}

CollectionManager* Collections::load_collection(const QString& name) {
	managers_guard.lock();
	CollectionManager* man = nullptr;
	for (auto it = active_managers.begin(); it != active_managers.end(); ++it) {
		if ((*it)->name == name)
			man = *it;
	}
	man = man ? man : new CollectionManager(name);
	active_managers.push_back(man);
	managers_guard.unlock();
	return man;
}
