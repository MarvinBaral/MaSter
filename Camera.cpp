#include "Camera.h"
#include "math.h"

Camera::Camera(double f_in_mm, double pixel_size_in_um, double max_mag):
	f (f_in_mm * 1E-3), pixelSize (pixel_size_in_um * 1E-6), maxMag(max_mag) //pixels are square-sized
{

}

//double Camera::getAngle(unsigned int pixelsFromCenter) //do pixels have space inbetween them?
////Assumption: Even number of pixels -> no "Central Pixel"
//{
//	if (pixelsFromCenter > 1) {
//		return atan((pixelsFromCenter * pixelSize - 0.5 * pixelSize) / f);
//	} else {
//		return 0;
//	}
//}
