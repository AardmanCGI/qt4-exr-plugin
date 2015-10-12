#ifndef EXR_PLUGIN_H
#define EXR_PLUGIN_H

#include <QByteArray>
#include <QImageIOHandler>
#include <QImageIOPlugin>
#include <QIODevice>

class ExrPlugin : public QImageIOPlugin
{
	public:

		QStringList keys() const;
		Capabilities capabilities(QIODevice *device,
				const QByteArray &format) const;
		QImageIOHandler* create(QIODevice *device,
				const QByteArray &format=QByteArray()) const;
};

#endif
