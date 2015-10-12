#include <IexBaseExc.h>
#include <ImfArray.h>
#include <ImathFun.h>
#include <ImfRgbaFile.h>
#include <ImfVersion.h>
#include <QVariant>

#include "exr_handler.h"
#include "exr_io.h"

// Default kneeLow = 0 and kneeHigh = 5 as per http://www.openexr.com/using.html
ExrHandler::ExrHandler(float kneeLow, float kneeHigh, float gamma) :
	QImageIOHandler(),
	f(findKneeF(Imath::Math<float>::pow(2.f, kneeHigh) - kl,
				Imath::Math<float>::pow(2.f, 3.5f) - kl)),
	gamma(gamma),
	kl(Imath::Math<float>::pow(2.f, kneeLow)),
	s(255.f * Imath::Math<float>::pow(2.f, -3.5f * gamma))
{
}

bool ExrHandler::canRead() const
{
	if (canRead(device())) {
		setFormat("exr");
		return true;
	}

	return false;
}

bool ExrHandler::read(QImage *outImage)
{
	if (!canRead(device())) {
		return false;
	}

	try {
		Exr_IStream stream(device());
		Imf::RgbaInputFile file(stream);
		Imath::Box2i dw = file.dataWindow();

		unsigned int width = dw.max.x - dw.min.x + 1;
		unsigned int height = dw.max.y - dw.min.y + 1;

		Imf::Array2D<Imf::Rgba> pixels;
		pixels.resizeErase(height, width);
		file.setFrameBuffer(&pixels[0][0] - dw.min.y * width, 1, width);
		file.readPixels (dw.min.y, dw.max.y);

		QImage image(width, height, QImage::Format_RGB32);
		if (image.isNull()) {
			return false;
		}

		for (unsigned int y = 0; y < height; y++) {
			for (unsigned int x = 0; x < width; x++) {
				image.setPixel(x, y, halfRgbaToQRgba(pixels[y][x+dw.min.x]));
			}
		}

		*outImage = image;
		return true;
	}
	catch (Iex::BaseExc &e) {
		qDebug(e.what());
	}

	return false;
}

QByteArray ExrHandler::name() const
{
	return "exr";
}

bool ExrHandler::canRead(QIODevice *device)
{
	if (!device) {
		qWarning("ExrHandler::canRead was called without a device!");
		return false;
	}

	char bytes[4];
	qint64 result = device->peek(bytes, 4);
	return result > 0 && Imf::isImfMagic(bytes);
}

bool ExrHandler::supportsOption(ImageOption option) const
{
	return (option == Size || option == Gamma);
}

QVariant ExrHandler::option(ImageOption option) const
{
	if (option == Size) {
		if (canRead(device())) {
			try {
				Exr_IStream stream(device());
				Imf::RgbaInputFile file(stream);
				Imath::Box2i dw = file.dataWindow();
				device()->reset();

				unsigned int width = dw.max.x - dw.min.x + 1;
				unsigned int height = dw.max.y - dw.min.y + 1;
				return QSize(width, height);
			}
			catch (Iex::BaseExc &e) {
				qDebug(e.what());
			}
		}
	}
	else if (option == Gamma) {
		return gamma;
	}

	return QVariant();
}

void ExrHandler::setOption(ImageOption option, const QVariant &value)
{
	if (option == Gamma) {
		bool ok = true;
		float newGamma = value.toFloat(&ok);
		if (ok) {
			gamma = newGamma;
		}
	}
}

inline float ExrHandler::knee(double x, double f)
{
	return float (Imath::Math<double>::log(x * f + 1.f) / f);
}

float ExrHandler::findKneeF(float x, float y)
{
	float f0 = 0;
	float f1 = 1.f;

	while (knee (x, f1) > y) {
		f0 = f1;
		f1 = f1 * 2.f;
	}

	for (int i = 0; i < 30; ++i) {
		const float f2 = (f0 + f1) / 2.f;
		const float y2 = knee (x, f2);

		if (y2 < y) {
			f1 = f2;
		}
		else {
			f0 = f2;
		}
	}

	return (f0 + f1) / 2.f;
}

// Adapted from OpenEXR_Viewers/exrdisplay/ImageView.cpp
QRgb ExrHandler::halfRgbaToQRgba(struct Imf::Rgba pixel)
{
	float r, g, b, a;
	//  1) Compensate for fogging by subtracting defog
	//     from the raw pixel values.
	// We assume a defog of 0

	//  2) Multiply the defogged pixel values by
	//     2^(exposure + 2.47393).
	const float exposeScale = Imath::Math<float>::pow(2, 2.47393f);
	r = pixel.r * exposeScale;
	g = pixel.g * exposeScale;
	b = pixel.b * exposeScale;
	a = pixel.a * exposeScale;

	//  3) Values that are now 1.0 are called "middle gray".
	//     If defog and exposure are both set to 0.0, then
	//     middle gray corresponds to a raw pixel value of 0.18.
	//     In step 6, middle gray values will be mapped to an
	//     intensity 3.5 f-stops below the display's maximum
	//     intensity.

	//  4) Apply a knee function.  The knee function has two
	//     parameters, kneeLow and kneeHigh.  Pixel values
	//     below 2^kneeLow are not changed by the knee
	//     function.  Pixel values above kneeLow are lowered
	//     according to a logarithmic curve, such that the
	//     value 2^kneeHigh is mapped to 2^3.5.  (In step 6,
	//     this value will be mapped to the the display's
	//     maximum intensity.)
	if (r > kl) {
		r = kl + knee(r - kl, f);
	}
	if (g > kl) {
		g = kl + knee(g - kl, f);
	}
	if (b > kl) {
		b = kl + knee(b - kl, f);
	}
	if (a > kl) {
		a = kl + knee(a - kl, f);
	}

	//  5) Gamma-correct the pixel values, according to the
	//     screen's gamma.  (We assume that the gamma curve
	//     is a simple power function.)
	r = Imath::Math<float>::pow (r, gamma);
	g = Imath::Math<float>::pow (g, gamma);
	b = Imath::Math<float>::pow (b, gamma);
	a = Imath::Math<float>::pow (a, gamma);

	//  6) Scale the values such that middle gray pixels are
	//     mapped to a frame buffer value that is 3.5 f-stops
	//     below the display's maximum intensity. (84.65 if
	//     the screen's gamma is 2.2)
	r *= s;
	g *= s;
	b *= s;
	a *= s;

	//  7) Clamp the values to [0, 255].
	r = Imath::clamp<float>(r, 0.f, 255.f);
	g = Imath::clamp<float>(g, 0.f, 255.f);
	b = Imath::clamp<float>(b, 0.f, 255.f);
	a = Imath::clamp<float>(a, 0.f, 255.f);

	return qRgba(
			(unsigned char) r,
			(unsigned char) g,
			(unsigned char) b,
			(unsigned char) a
			);
}
