#ifndef CAMERA_H
#define CAMERA_H

class Camera
{
	public:
		const double f; //m
		const double pixelSize; //m
		const double maxMag;
		Camera(double f_in_mm, double pixel_size_in_um, double max_mag);
		double getAngle(unsigned int pixelsFromCenter);
};

#endif // CAMERA_H
