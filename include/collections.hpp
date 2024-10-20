#pragma once

#include <QCborArray>
#include <dict_types.hpp>
#include <QAbstractListModel>
#include <QMutex>
#include <QWaitCondition>

class CollectionManager : public QAbstractListModel {
	Q_OBJECT
	QML_ELEMENT
	friend class Collections;
public:
	CollectionManager(const QString& name);

	enum VocabRoles {
		VocabRole = Qt::DisplayRole + 1,
		DefinitionRole
	};
	int rowCount(const QModelIndex& parent) const override;
	QVariant data(const QModelIndex& index, int role) const override;
	QHash<int, QByteArray> roleNames() const override;

	Q_INVOKABLE void listview_disown();
	const QString name;
	CollectionManager() = delete;

	QWaitCondition cv;
	QMutex mutex;
	int task_cnt = 0;

private:
	QCborArray collection;
	QVector<Vocab> vocabs;
};

extern QVector<CollectionManager*> active_managers;
extern QMutex managers_guard;

class Collections : public QObject {
	Q_OBJECT
	QML_ELEMENT
	QML_SINGLETON
	Q_PROPERTY(QStringList collections MEMBER collections NOTIFY collectionsChanged)
public:
	Collections();
	Q_INVOKABLE void update_collections();
	QStringList collections;
	Q_INVOKABLE void create_collection(const QString&);
	Q_INVOKABLE void delete_collection(const QString&);
	Q_INVOKABLE CollectionManager* load_collection(const QString&);
signals:
	void collectionsChanged();
};
