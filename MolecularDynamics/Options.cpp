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
	rightSideInsteadOfSphere(false),
	smallSphereColor(RGB(0, 255, 0)),
	bigSphereColor(RGB(255, 0, 0)),
	showBillboard(false),
	nrBins(30),
	maxSpeed(15),
	useSpline(true),
	lineThickness(3),
	alpha(60)
{
}


Options::~Options()
{
}


void Options::Load()
{
	int res = static_cast<int>(theApp.GetProfileInt(L"options", L"gamma", 0));
	gammaCorrection = (res != 0);

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

	res = static_cast<int>(theApp.GetProfileInt(L"options", L"rightSide", 0));
	rightSideInsteadOfSphere = res != 0;

	smallSphereColor = GetColor(L"smallSphereColor", RGB(0, 255, 0));
	bigSphereColor = GetColor(L"bigSphereColor", RGB(255, 0, 0));

	res = static_cast<int>(theApp.GetProfileInt(L"options", L"showBillboard", 1));
	showBillboard = res != 0;

	nrBins = theApp.GetProfileInt(L"options", L"nrBins", 30);
	maxSpeed = GetDouble(L"maxSpeed", 15.);

	res = static_cast<int>(theApp.GetProfileInt(L"options", L"useSpline", 1));
	useSpline = res != 0;

	lineThickness = theApp.GetProfileInt(L"options", L"lineThickness", 3);
	alpha = theApp.GetProfileInt(L"options", L"alpha", 60);
}


void Options::Save()
{
	theApp.WriteProfileInt(L"options", L"gamma", gammaCorrection ? 1 : 0);
	theApp.WriteProfileInt(L"options", L"translationSpeed", static_cast<int>(translationSpeed));
	theApp.WriteProfileInt(L"options", L"rotationSpeed", static_cast<int>(rotationSpeed));
	theApp.WriteProfileInt(L"options", L"scrollSpeed", static_cast<int>(scrollSpeed));

	theApp.WriteProfileInt(L"options", L"nrSpheres", static_cast<int>(nrSpheres));

	theApp.WriteProfileBinary(L"options", L"interiorSphereRadius", (LPBYTE)&interiorSpheresRadius, sizeof(double));
	theApp.WriteProfileBinary(L"options", L"exteriorSphereRadius", (LPBYTE)&exteriorSpheresRadius, sizeof(double));
	theApp.WriteProfileBinary(L"options", L"interiorSphereMass", (LPBYTE)&interiorSpheresMass, sizeof(double));
	theApp.WriteProfileBinary(L"options", L"exteriorSphereMass", (LPBYTE)&exteriorSpheresMass, sizeof(double));

	theApp.WriteProfileBinary(L"options", L"initialSpeed", (LPBYTE)&initialSpeed, sizeof(double));
	theApp.WriteProfileBinary(L"options", L"interiorRadius", (LPBYTE)&interiorRadius, sizeof(double));

	theApp.WriteProfileInt(L"options", L"rightSide", rightSideInsteadOfSphere ? 1 : 0);

	theApp.WriteProfileBinary(L"options", L"smallSphereColor", (LPBYTE)&smallSphereColor, sizeof(COLORREF));
	theApp.WriteProfileBinary(L"options", L"bigSphereColor", (LPBYTE)&bigSphereColor, sizeof(COLORREF));

	theApp.WriteProfileInt(L"options", L"showBillboard", showBillboard ? 1 : 0);

	theApp.WriteProfileInt(L"options", L"nrBins", static_cast<int>(nrBins));
	theApp.WriteProfileBinary(L"options", L"maxSpeed", (LPBYTE)&maxSpeed, sizeof(double));
	theApp.WriteProfileInt(L"options", L"useSpline", useSpline ? 1 : 0);
	theApp.WriteProfileInt(L"options", L"lineThickness", static_cast<int>(lineThickness));
	theApp.WriteProfileInt(L"options", L"alpha", static_cast<int>(alpha));
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