#include <qplugin.h>

#include "exr_handler.h"
#include "exr_plugin.h"

QStringList ExrPlugin::keys() const
{
	return QStringList("exr");
}

QImageIOPlugin::Capabilities ExrPlugin::capabilities(QIODevice *device,
		const QByteArray &format) const
{
	if (format == "exr") {
		return Capabilities(CanRead);
	}

	if (!(format.isEmpty() && device->isOpen())) {
		return 0;
	}

	Capabilities cap;
	if (device->isReadable() && ExrHandler::canRead(device)) {
		cap |= CanRead;
	}

	return cap;
}

QImageIOHandler* ExrPlugin::create(QIODevice *device,
		const QByteArray &format) const
{
	QImageIOHandler* handler = new ExrHandler;
	handler->setDevice(device);
	handler->setFormat(format);
	return handler;
}

Q_EXPORT_STATIC_PLUGIN(ExrPlugin)
Q_EXPORT_PLUGIN2(qexr4, ExrPlugin)
