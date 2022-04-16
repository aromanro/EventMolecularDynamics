
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

// Attributes
public:
	std::atomic<int> nrsteps;
	int nrParticles;

	std::atomic<double> simulationTime;

	MolecularDynamics::ComputationResult curResult;

	std::mutex dataSection;
	std::queue<MolecularDynamics::ComputationResult> resultsQueue;


	Options options; // a copy of the options, they might change during running

private:
	MolecularDynamics::StatisticsThread* theThread;

// Operations
public:
// Overrides
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CMolecularDynamicsDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	CMolecularDynamicsView* GetMainView();

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS

public:
	void Advance();
};
