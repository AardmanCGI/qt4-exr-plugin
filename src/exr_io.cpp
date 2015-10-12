#include <exception>

#include <IexThrowErrnoExc.h>

#include "exr_io.h"

Exr_IStream::Exr_IStream(QIODevice *device) : Imf::IStream(""), device(device)
{
	if (!device) {
		throw std::runtime_error("Cannot create stream without a valid device");
	}
}

bool Exr_IStream::read(char c[], int n)
{
	qint64 result = device->read(c, n);

	// Read past the end of the stream
	if (result < 0) {
		Iex::throwErrnoExc();
	}

	// If there are more bytes available
	if (result > 0) {
		return true;
	}

	// There are no more bytes available
	return false;
}

Imf::Int64 Exr_IStream::tellg()
{
	return device->pos();
}

void Exr_IStream::seekg(Imf::Int64 pos)
{
	device->seek(pos);
}

void Exr_IStream::clear()
{
}

Exr_OStream::Exr_OStream(QIODevice *device) : Imf::OStream(""), device(device)
{
	if (!device) {
		throw std::runtime_error("Cannot create stream without a valid device");
	}
}

void Exr_OStream::write(const char c[], int n)
{
	if (device->write(c, n) < 0) {
		Iex::throwErrnoExc();
	}
}

Imf::Int64 Exr_OStream::tellp()
{
	return device->pos();
}

void Exr_OStream::seekp(Imf::Int64 pos)
{
	device->seek(pos);
}
