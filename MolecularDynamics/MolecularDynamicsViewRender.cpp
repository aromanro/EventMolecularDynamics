#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "MolecularDynamics.h"
#endif

#include <GL\glew.h>
#include <gl\gl.h>      // OpenGL Libraries
#include <gl\glu.h>     // GLU OpenGL Libraries

#include <glm.hpp>


#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>


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
	if (NULL == program) return;

	CMolecularDynamicsDoc* doc = GetDocument();
	if (!doc) return;

	glm::mat4 mat = perspectiveMatrix * (glm::mat4)camera;

	program->Use();

	glUniform3f(program->viewPosLocation, static_cast<float>(camera.eyePos.X), static_cast<float>(camera.eyePos.Y), static_cast<float>(camera.eyePos.Z));
	glUniformMatrix4fv(program->matLocation, 1, GL_FALSE, value_ptr(mat));

	for (unsigned int i = 0; i < program->lights.size(); ++i)
	{
		glm::vec3 lightDir = program->lights[i].lightPos;
		lightDir = glm::normalize(lightDir);
		glUniform3f(program->lights[i].lightDirPos, lightDir.x, lightDir.y, lightDir.z);
		glUniform1f(program->lights[i].attenPos, program->lights[i].atten);
	}

	// now particles!!!

	int index = 0;
	for (const auto& particle : doc->curResult.particles)
	{
		Vector3D<double> particlePos = particle.GetPosition(doc->simulationTime/*, spaceSize*/);
		glm::vec3 pos = glm::vec3(particlePos.X, particlePos.Y, particlePos.Z);
		//glm::vec3 pos = glm::vec3(particle.position.X, particle.position.Y, particle.position.Z);

		position[3ULL * index] = pos.x;
		position[3ULL * index + 1ULL] = pos.y;
		position[3ULL * index + 2ULL] = pos.z;

		++index;
	}

	glEnableVertexAttribArray(2);
	posBuffer->setData(NULL, doc->nrParticles * 3 * sizeof(GLfloat), GL_STREAM_DRAW); // orphaning
	posBuffer->setSubData(position.data(), doc->nrParticles * 3 * sizeof(GLfloat));

	sphere->DrawInstanced(doc->nrParticles);

	program->UnUse();

	if (theApp.options.showBillboard)
		DisplayBilboard(mat);
}


void CMolecularDynamicsView::DisplayBilboard(glm::mat4& mat)
{
	if (!billboardProgram) return;

	billboardProgram->Use();

	glUniform1i(billboardProgram->textureLoc, 0);

	glUniformMatrix4fv(billboardProgram->matLocation, 1, GL_FALSE, value_ptr(mat));

	glm::dmat4 precisionMat(camera.getMatrixDouble());
	// cancel out the translation of the camera, but preserve rotation:
	precisionMat[3][0] = 0;
	precisionMat[3][1] = 0;
	precisionMat[3][2] = 0;

	// put the billboard in front of the camera
	const Vector3D<double> forward = camera.getNormalizedForward();
	glm::dvec3 pos(forward.X, forward.Y, forward.Z);
	pos *= 0.101;

	// but shifted downwards a little
	const Vector3D<double> up = camera.getNormalizedUp();
	pos -= 0.038 * glm::dvec3(up.X, up.Y, up.Z);

	const glm::dvec3 cameraVector = glm::dvec3(camera.eyePos.X, camera.eyePos.Y, camera.eyePos.Z);
	const glm::dvec3 billboardPos = cameraVector + pos;

	const double bscale = 0.0025f;

	const glm::dmat4 modelMatHP = glm::scale(glm::translate(glm::dmat4(1.), billboardPos), glm::dvec3(bscale, bscale, bscale)) * glm::transpose(precisionMat);
	const glm::mat4 modelMat(modelMatHP);

	glUniformMatrix4fv(billboardProgram->modelMatLocation, 1, GL_FALSE, value_ptr(modelMat));

	billboardTexture->Bind();

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

