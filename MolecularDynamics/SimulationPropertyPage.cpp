// SimulationPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "SimulationPropertyPage.h"
#include "afxdialogex.h"

#include "Constants.h"
#include "resource.h"

#include "MolecularDynamics.h"
#include "Options.h"

// SimulationPropertyPage dialog

IMPLEMENT_DYNAMIC(SimulationPropertyPage, CPropertyPage)

SimulationPropertyPage::SimulationPropertyPage()
	: CPropertyPage(IDD_SIMULATIONPROPERTYPAGE)
{
	nrSpheres = theApp.options.nrSpheres;
	interiorRadius = theApp.options.interiorRadius;
	initialSpeed = theApp.options.initialSpeed;
	
	interiorSpheresRadius = theApp.options.interiorSpheresRadius;
	interiorSpheresMass = theApp.options.interiorSpheresMass;
	exteriorSpheresRadius = theApp.options.exteriorSpheresRadius;
	exteriorSpheresMass = theApp.options.exteriorSpheresMass;
}

SimulationPropertyPage::~SimulationPropertyPage()
{
}

void SimulationPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_EDIT2, interiorRadiusControl);
	DDX_Control(pDX, IDC_EDIT3, initialSpeedControl);
	DDX_Control(pDX, IDC_EDIT4, interiorSpheresRadiusControl);
	DDX_Control(pDX, IDC_EDIT5, interiorSpheresMassControl);
	DDX_Control(pDX, IDC_EDIT6, exteriorSpheresRadiusControl);
	DDX_Control(pDX, IDC_EDIT7, exteriorSpheresMassControl);


	DDX_Text(pDX, IDC_EDIT1, nrSpheres);
	DDX_Text(pDX, IDC_EDIT2, interiorRadius);
	DDX_Text(pDX, IDC_EDIT3, initialSpeed);

	DDX_Text(pDX, IDC_EDIT4, interiorSpheresRadius);
	DDX_Text(pDX, IDC_EDIT5, interiorSpheresMass);
	DDX_Text(pDX, IDC_EDIT6, exteriorSpheresRadius);
	DDX_Text(pDX, IDC_EDIT7, exteriorSpheresMass);

	DDV_MinMaxUInt(pDX, nrSpheres, 1, 20000);
	DDV_MinMaxDouble(pDX, interiorRadius, 0, spaceSize / 2.);
	DDV_MinMaxDouble(pDX, initialSpeed, 0, 99999);

	DDV_MinMaxDouble(pDX, interiorSpheresRadius, 0.01, spaceSize / 2.);
	DDV_MinMaxDouble(pDX, exteriorSpheresRadius, 0.01, spaceSize / 2.);

	DDV_MinMaxDouble(pDX, interiorSpheresMass, 0.01, 99999);
	DDV_MinMaxDouble(pDX, exteriorSpheresMass, 0.01, 99999);
}


BEGIN_MESSAGE_MAP(SimulationPropertyPage, CPropertyPage)
	ON_EN_CHANGE(IDC_EDIT1, &SimulationPropertyPage::OnEnChangeEdit)
	ON_EN_CHANGE(IDC_EDIT2, &SimulationPropertyPage::OnEnChangeEdit)
	ON_EN_CHANGE(IDC_EDIT3, &SimulationPropertyPage::OnEnChangeEdit)
	ON_EN_CHANGE(IDC_EDIT4, &SimulationPropertyPage::OnEnChangeEdit)
	ON_EN_CHANGE(IDC_EDIT5, &SimulationPropertyPage::OnEnChangeEdit)
	ON_EN_CHANGE(IDC_EDIT6, &SimulationPropertyPage::OnEnChangeEdit)
	ON_EN_CHANGE(IDC_EDIT7, &SimulationPropertyPage::OnEnChangeEdit)
END_MESSAGE_MAP()


// SimulationPropertyPage message handlers


BOOL SimulationPropertyPage::OnApply()
{
	UpdateData();

	ApplyValues();

	return CPropertyPage::OnApply();
}


BOOL SimulationPropertyPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	interiorRadiusControl.allowNegative = false;
	initialSpeedControl.allowNegative = false;
	interiorSpheresMassControl.allowNegative = false;
	interiorSpheresRadiusControl.allowNegative = false;
	exteriorSpheresMassControl.allowNegative = false;
	exteriorSpheresRadiusControl.allowNegative = false;

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void SimulationPropertyPage::ApplyValues()
{
	theApp.options.nrSpheres = nrSpheres;
	theApp.options.interiorRadius = interiorRadius;
	theApp.options.initialSpeed = initialSpeed;

	theApp.options.interiorSpheresRadius = interiorSpheresRadius;
	theApp.options.interiorSpheresMass = interiorSpheresMass;
	theApp.options.exteriorSpheresRadius = exteriorSpheresRadius;
	theApp.options.exteriorSpheresMass = exteriorSpheresMass;

	theApp.options.Save();
}


void SimulationPropertyPage::OnEnChangeEdit()
{
	SetModified();
}
