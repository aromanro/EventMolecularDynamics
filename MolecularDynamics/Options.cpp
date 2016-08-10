#include "stdafx.h"
#include "Options.h"

#include "MolecularDynamics.h"

Options::Options()
	:
	translationSpeed(100), rotationSpeed(100), scrollSpeed(100),
	gammaCorrection(false),
	nrSpheres(3000),
	interiorSpheresRadius(3),
	exteriorSpheresRadius(1),
	interiorSpheresMass(4),
	exteriorSpheresMass(1),
	initialSpeed(1),
	interiorRadius(30),
	smallSphereColor(RGB(0, 255, 0)),
	bigSphereColor(RGB(255, 0, 0))
{
}


Options::~Options()
{
}


void Options::Load()
{
	int res = (int)theApp.GetProfileInt(L"options", L"gamma", 0);
	gammaCorrection = (res != 0 ? true : false);

	translationSpeed = theApp.GetProfileInt(L"options", L"translationSpeed", 100);
	rotationSpeed = theApp.GetProfileInt(L"options", L"rotationSpeed", 100);
	scrollSpeed = theApp.GetProfileInt(L"options", L"scrollSpeed", 100);

	nrSpheres = theApp.GetProfileInt(L"options", L"nrSpheres", 3000);

	interiorSpheresRadius = GetDouble(L"interiorSphereRadius", 2);
	exteriorSpheresRadius = GetDouble(L"exteriorSphereRadius", 1);
	interiorSpheresMass = GetDouble(L"interiorSphereMass", 4.);
	exteriorSpheresMass = GetDouble(L"exteriorSphereMass", 1.);
	initialSpeed = GetDouble(L"initialSpeed", 1.);
	interiorRadius = GetDouble(L"interiorRadius", 30);

	smallSphereColor = GetColor(L"smallSphereColor", RGB(0, 255, 0));
	bigSphereColor = GetColor(L"bigSphereColor", RGB(255, 0, 0));
}


void Options::Save()
{
	theApp.WriteProfileInt(L"options", L"gamma", gammaCorrection ? 1 : 0);
	theApp.WriteProfileInt(L"options", L"translationSpeed", (int)translationSpeed);
	theApp.WriteProfileInt(L"options", L"rotationSpeed", (int)rotationSpeed);
	theApp.WriteProfileInt(L"options", L"scrollSpeed", (int)scrollSpeed);

	theApp.WriteProfileInt(L"options", L"nrSpheres", (int)nrSpheres);

	theApp.WriteProfileBinary(L"options", L"interiorSphereRadius", (LPBYTE)&interiorSpheresRadius, sizeof(double));
	theApp.WriteProfileBinary(L"options", L"exteriorSphereRadius", (LPBYTE)&exteriorSpheresRadius, sizeof(double));
	theApp.WriteProfileBinary(L"options", L"interiorSphereMass", (LPBYTE)&interiorSpheresMass, sizeof(double));
	theApp.WriteProfileBinary(L"options", L"exteriorSphereMass", (LPBYTE)&exteriorSpheresMass, sizeof(double));

	theApp.WriteProfileBinary(L"options", L"initialSpeed", (LPBYTE)&initialSpeed, sizeof(double));
	theApp.WriteProfileBinary(L"options", L"interiorRadius", (LPBYTE)&interiorRadius, sizeof(double));

	theApp.WriteProfileBinary(L"options", L"smallSphereColor", (LPBYTE)&smallSphereColor, sizeof(COLORREF));
	theApp.WriteProfileBinary(L"options", L"bigSphereColor", (LPBYTE)&bigSphereColor, sizeof(COLORREF));
}

COLORREF Options::GetColor(LPCTSTR param, COLORREF defval)
{
	UINT sz = 0;
	LPBYTE buf = NULL;

	if (theApp.GetProfileBinary(L"options", param, &buf, &sz))
	{
		if (sizeof(COLORREF) == sz)	return *((COLORREF*)buf);
		delete[] buf;
	}

	return defval;
}


double Options::GetDouble(LPCTSTR param, double defval)
{
	double val = defval;

	UINT sz = 0;
	LPBYTE buf = NULL;

	if (theApp.GetProfileBinary(L"options", param, &buf, &sz))
	{
		if (sizeof(double) == sz) val = *((double*)buf);
		delete[] buf;
	}

	return val;
}