#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "MolecularDynamics.h"
#endif

#include <gl/glew.h>
#include <gl/GL.h>      // OpenGL Libraries
#include <gl/GLU.h>     // GLU OpenGL Libraries

#include <glm.hpp>


#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>


#include "Camera.h"

#include "Program.h"
#include "Sphere.h"

#include "MolecularDynamicsDoc.h"
#include "MolecularDynamicsView.h"

#include "MainFrm.h"
#include "MFCToolBarSlider.h"

#include "Constants.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



void CMolecularDynamicsView::RenderScene()
{
	if (nullptr == program) return;

	const CMolecularDynamicsDoc* doc = GetDocument();
	if (!doc) return;

	glm::mat4 mat = perspectiveMatrix * (glm::mat4)camera;

	program->Use();

	glUniform3f(program->viewPosLocation, static_cast<float>(camera.GetEyePos().X), static_cast<float>(camera.GetEyePos().Y), static_cast<float>(camera.GetEyePos().Z));
	glUniformMatrix4fv(program->matLocation, 1, GL_FALSE, value_ptr(mat));

	for (const auto& light : program->lights)
	{
		auto lightDir = light.lightPos;
		lightDir = glm::normalize(lightDir);
		glUniform3f(light.lightDirPos, lightDir.x, lightDir.y, lightDir.z);
		glUniform1f(light.attenPos, light.atten);
	}

	// now particles!!!

	int index = 0;
	for (const auto& particle : doc->curResult.particles)
	{
		const auto particlePos = particle.GetPosition(doc->simulationTime/*, spaceSize*/);

		position[3ULL * index] = static_cast<float>(particlePos.X);
		position[3ULL * index + 1ULL] = static_cast<float>(particlePos.Y);
		position[3ULL * index + 2ULL] = static_cast<float>(particlePos.Z);

		++index;
	}

	glEnableVertexAttribArray(2);
	posBuffer->setData(nullptr, doc->nrParticles * 3 * sizeof(GLfloat), GL_STREAM_DRAW); // orphaning
	posBuffer->setSubData(position.data(), doc->nrParticles * 3 * sizeof(GLfloat));

	sphere->DrawInstanced(doc->nrParticles);

	program->UnUse();

	if (theApp.GetOptions().showBillboard)
		DisplayBilboard(mat);
}


void CMolecularDynamicsView::DisplayBilboard(glm::mat4& mat)
{
	if (nullptr == billboardProgram) return;

	const CMolecularDynamicsDoc* doc = GetDocument();
	if (!doc) return;

	billboardProgram->Use();

	glUniform1f(billboardProgram->alphaLoc, static_cast<float>(doc->options.alpha) / 100.f);
	glUniform1i(billboardProgram->textureLoc, 0);
	glUniformMatrix4fv(billboardProgram->matLocation, 1, GL_FALSE, value_ptr(mat));

	glm::dmat4 precisionMat(camera.getMatrixDouble());
	// cancel out the translation of the camera, but preserve rotation:
	precisionMat[3][0] = 0;
	precisionMat[3][1] = 0;
	precisionMat[3][2] = 0;

	// put the billboard in front of the camera
	const auto forward = camera.getNormalizedForward();
	glm::dvec3 pos(forward.X, forward.Y, forward.Z);
	pos *= 0.101;

	// but shifted downwards a little
	const auto up = camera.getNormalizedUp();
	pos -= 0.025 * glm::dvec3(up.X, up.Y, up.Z);

	const auto cameraVector = glm::dvec3(camera.GetEyePos().X, camera.GetEyePos().Y, camera.GetEyePos().Z);
	const auto billboardPos = cameraVector + pos;

	const double bscale = 0.03f;

	const glm::dmat4 modelMatHP = glm::scale(glm::translate(glm::dmat4(1.), billboardPos), glm::dvec3(bscale, bscale, bscale)) * glm::transpose(precisionMat);
	const glm::mat4 modelMat(modelMatHP);

	glUniformMatrix4fv(billboardProgram->modelMatLocation, 1, GL_FALSE, value_ptr(modelMat));

	//billboardTexture->Bind();

	DisableAntialias(); // otherwise a diagonal line is shown over the rectangle sometimes, with alpha blending on

	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);

	billboardRectangle->Draw();

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	glDisable(GL_BLEND);

	EnableAntialias();

	billboardProgram->UnUse();
}

