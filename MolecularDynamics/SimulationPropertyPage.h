#pragma once
#include "afxwin.h"

#include "NumberEdit.h"

// SimulationPropertyPage dialog

class SimulationPropertyPage : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(SimulationPropertyPage)

public:
	SimulationPropertyPage();
	virtual ~SimulationPropertyPage();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SIMULATIONPROPERTYPAGE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	CNumberEdit interiorRadiusControl;
	CNumberEdit initialSpeedControl;
	CNumberEdit interiorSpheresMassControl;
	CNumberEdit interiorSpheresRadiusControl;
	CNumberEdit exteriorSpheresMassControl;
	CNumberEdit exteriorSpheresRadiusControl;

	unsigned int nrSpheres;
	double interiorRadius;
	double initialSpeed;
	double interiorSpheresRadius;
	double interiorSpheresMass;
	double exteriorSpheresRadius;
	double exteriorSpheresMass;

	int rightSide;

	virtual BOOL OnApply();
	virtual BOOL OnInitDialog();
	void ApplyValues();

	afx_msg void OnEnChangeEdit();
public:
	afx_msg void OnBnClickedCheck3();
};
