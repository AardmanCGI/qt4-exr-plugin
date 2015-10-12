#ifndef EXR_STREAM_H
#define EXR_STREAM_H

#include <ImathInt64.h>
#include <ImfIO.h>
#include <QIODevice>

class Exr_IStream : public Imf::IStream {
	public:
		Exr_IStream(QIODevice *device);

		virtual bool read(char c[], int n);
		virtual Imf::Int64 tellg();
		virtual void seekg(Imf::Int64 pos);
		virtual void clear();

	private:
		QIODevice *device;
};

class Exr_OStream : public Imf::OStream {
	public:
		Exr_OStream(QIODevice *device);

		virtual void write(const char c[], int n);
		virtual Imf::Int64 tellp();
		virtual void seekp(Imf::Int64 pos);

	private:
		QIODevice *device;
};

#endif
