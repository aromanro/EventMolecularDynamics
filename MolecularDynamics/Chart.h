#pragma once

#include "ChartAxis.h"
#include "DataSets.h"

class Chart
{
	friend class Axis;
public:
	Chart();

	DataSets dataSets;

	void Draw(const CDC* pDC, CRect& rect);

	void SetNumTicksX(int ticks) { X.SetNumTicks(ticks); }
	void SetNumTicksY(int ticks) { Y.SetNumTicks(ticks); }
	void SetNumBigTicksX(int ticks) { X.SetNumBigTicks(ticks); }
	void SetNumBigTicksY(int ticks) { Y.SetNumBigTicks(ticks); }
	void SetXLabels(const std::list<CString>& l) { X.SetLabels(l); }
	void SetYLabels(const std::list<CString>& l) { Y.SetLabels(l); }

	void AddDataSet(const double *dataX, const double *dataY, unsigned int len, float lineWidth = 0, COLORREF color = RGB(0, 0, 0));
	void AddDataSet(const std::vector<std::pair<double, double>> *data, float lineWidth = 0, COLORREF color = RGB(0, 0, 0));
	void AddDataSet(double start, double step, const double *dataY, unsigned int len, float lineWidth = 0, COLORREF color = RGB(0, 0, 0));
	void AddDataSlice(double X, const double *slice, unsigned int len);
	void clear();

	CString title;
	CString XAxisLabel;
	CString YAxisLabel;
	bool XAxisGrid = true;
	bool YAxisGrid = true;

	bool useSpline = true;
	bool antialias = false;

	double XAxisMin = DBL_MIN;
	double XAxisMax = DBL_MAX;
	double YAxisMin = DBL_MIN;
	double YAxisMax = DBL_MAX;

	bool drawStartTickX = true;
	bool drawStartTickY = true;

	int maxTitleHeight = 36;
	int maxAxisLabelHeight = 18;
	int maxLabelHeight = 28;

private:
	Axis X;
	Axis Y;

	CRect chartRect;

	int GetNumTicks() const;
	int GetNumBigTicks() const;

	std::list<CString> GetXLabels() const;
	std::list<CString> GetYLabels() const;

	float GetLabelHeight(bool XAxis = true) const;
	float GetLabelWidth(bool XAxis = true) const;

	int GetNeededFontSize(CString& str, const Gdiplus::Graphics& g, const Gdiplus::RectF& boundRect) const;

	void DrawText(CString& str, Gdiplus::Graphics& g, const Gdiplus::RectF& boundRect, Gdiplus::StringAlignment align = Gdiplus::StringAlignmentCenter, float fontSize = 0);

	void DrawXLabel(Gdiplus::Graphics& g, const CRect& rect);
	void DrawYLabel(Gdiplus::Graphics& g, const CRect& rect, int leftSide);
	void DrawData(Gdiplus::Graphics& g, const CRect& rect);
	void DrawAxis(Gdiplus::Graphics& g, CRect& rect, int titleHeight);
	float GetLabelFontSize(Gdiplus::Graphics& g);
};

