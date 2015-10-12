#ifndef EXR_HANDLER_H
#define EXR_HANDLER_H

#include <ImfRgba.h>
#include <QByteArray>
#include <QColor>
#include <QImage>
#include <QImageIOHandler>
#include <QIODevice>

class ExrHandler : public QImageIOHandler
{
	public:
		ExrHandler(float kneeLow=0, float kneeHigh=5.f, float gamma=1.f/2.2f);

		bool canRead() const;
		bool read(QImage *image);

		QByteArray name() const;

		static bool canRead(QIODevice *device);

		bool supportsOption(ImageOption option) const;
		QVariant option(ImageOption option) const;
		void setOption(ImageOption option, const QVariant &value);

		QRgb halfRgbaToQRgba(struct Imf::Rgba imagepixel);

	private:
		float f;
		float gamma;
		float kl;
		float s;

		inline float knee(double x, double f);
		float findKneeF(float x, float y);
};

#endif
