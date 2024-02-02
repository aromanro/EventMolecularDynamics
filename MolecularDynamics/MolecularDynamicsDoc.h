
// MolecularDynamicsDoc.h : interface of the CMolecularDynamicsDoc class
//


#pragma once

#include <queue>

#include "Simulation.h"
#include "StatisticsThread.h"
#include "ComputationResult.h"
#include "Options.h"


class CMolecularDynamicsView;


class CMolecularDynamicsDoc : public CDocument
{
protected: // create from serialization only
	CMolecularDynamicsDoc();
	DECLARE_DYNCREATE(CMolecularDynamicsDoc)

public:
	// Implementation
	~CMolecularDynamicsDoc() override;

	CMolecularDynamicsView* GetMainView();

	void RetrieveStatistics()
	{
		if (theThread)
			theThread->CalculateStatistics = true;
	}

	// Attributes
	std::atomic<int> nrsteps;
	int nrParticles;

	int nrBigSpheres;
	int nrSmallSpheres;

	std::atomic<double> simulationTime;

	MolecularDynamics::ComputationResult curResult;

	std::mutex dataSection;

	std::vector<double> results1;
	std::vector<double> results2;

	std::queue<MolecularDynamics::ComputationResult> resultsQueue;


	Options options; // a copy of the options, they might change during running

private:
	MolecularDynamics::StatisticsThread* theThread;

// Operations
// Overrides
	BOOL OnNewDocument() override;
	void Serialize(CArchive& ar) override;
#ifdef SHARED_HANDLERS
	void InitializeSearchContent() override;
	void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds) override;
#endif // SHARED_HANDLERS


#ifdef _DEBUG
	void AssertValid() const override;
	void Dump(CDumpContext& dc) const override;
#endif



// Generated message map functions
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS

public:
	void Advance();
};
