#pragma once
#include "afxwin.h"

#include "NumberEdit.h"

// SimulationPropertyPage dialog

class SimulationPropertyPage : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(SimulationPropertyPage)

public:
	SimulationPropertyPage();
	~SimulationPropertyPage() override;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SIMULATIONPROPERTYPAGE };
#endif

private:
	void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

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

	BOOL OnApply() override;
	BOOL OnInitDialog() override;
	void ApplyValues();

	afx_msg void OnEnChangeEdit();
	afx_msg void OnBnClickedCheck3();
};
