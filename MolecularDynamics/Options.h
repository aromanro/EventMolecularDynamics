#pragma once
class Options
{
public:
	unsigned int translationSpeed;
	unsigned int rotationSpeed;
	unsigned int scrollSpeed;

	bool gammaCorrection;

	unsigned int nrSpheres;
	
	double interiorSpheresRadius;
	double exteriorSpheresRadius;
	double interiorSpheresMass;
	double exteriorSpheresMass;
	double initialSpeed;

	double interiorRadius;

	COLORREF smallSphereColor;
	COLORREF bigSphereColor;

	// Chart options:
	bool showBillboard;

	unsigned int nrBins;
	double maxSpeed;

	bool useSpline;
	unsigned int lineThickness;

	Options();
	~Options();

	void Load();
	void Save();

protected:
	static COLORREF GetColor(LPCTSTR param, COLORREF defval = RGB(0, 0, 0));
	static double GetDouble(LPCTSTR param, double defval = 0);
};

