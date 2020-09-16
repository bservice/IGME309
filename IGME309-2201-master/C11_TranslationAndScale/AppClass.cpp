#include "AppClass.h"
void Application::InitVariables(void)
{
	//init the mesh
	//m_pMesh = new MyMesh();
	
	for (int i = 0; i < 46; i++)
	{
		MyMesh* temp = new MyMesh();		
		cubes.push_back(temp);
		cubes[i]->GenerateCube(1.0f, C_BLACK);
	}
	left = false;
	right = true;
	//m_pMesh->GenerateCube(1.0f, C_BLACK);
	//m_pMesh->GenerateSphere(1.0f, 5, C_WHITE);
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	matrix4 m4View = m_pCameraMngr->GetViewMatrix();
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();
	
	matrix4 m4Scale = glm::scale(IDENTITY_M4, vector3(1.0f,1.0f,1.0f));
	static float value = 0.0f;
	matrix4 m4Translate = glm::translate(IDENTITY_M4, vector3(value, 0.0f, 0.0f));
	
	//If statement to move it to the right, move back to left when reach limit
	if(right)
	{
		value += 0.01f;

		if (value >= 0.5f)
		{
			right = false;
			left = true;
		}
	}

	//If statement to move it to the left, move back to right when reach limit
	if (left)
	{
		value -= 0.01f;
		if (value <= -0.5f)
		{
			right = true;
			left = false;
		}
	}

	//matrix4 m4Model = m4Translate * m4Scale;
	matrix4 m4Model = m4Scale * m4Translate;

	//Make origin cube at 0.0f, 0.0f, 0.0f
	cubes[0]->Render(m4Projection, m4View, m4Model);

	//Change the translation matrix to make the next cube in the vector to the right place based off the origin cube
	//Example: the next cube is 1 above origin so the translation gets changed to value, 1.0f, 0.0f - value in this case is 0.0f
	m4Translate = glm::translate(IDENTITY_M4, vector3(value, 1.0f, 0.0f));
	m4Model = m4Scale * m4Translate;

	cubes[1]->Render(m4Projection, m4View, m4Model);

	m4Translate = glm::translate(IDENTITY_M4, vector3(value, 2.0f, 0.0f));
	m4Model = m4Scale * m4Translate;

	cubes[2]->Render(m4Projection, m4View, m4Model);

	m4Translate = glm::translate(IDENTITY_M4, vector3(value, -1.0f, 0.0f));
	m4Model = m4Scale * m4Translate;

	cubes[3]->Render(m4Projection, m4View, m4Model);
	
	//Adding to value to move to the right of the origin column, subtracting later on will make the subes in the column to the left of the origin
	m4Translate = glm::translate(IDENTITY_M4, vector3(value + 1.0f, 0.0f, 0.0f));
	m4Model = m4Scale * m4Translate;

	cubes[4]->Render(m4Projection, m4View, m4Model);

	m4Translate = glm::translate(IDENTITY_M4, vector3(value + 1.0f, 1.0f, 0.0f));
	m4Model = m4Scale * m4Translate;

	cubes[5]->Render(m4Projection, m4View, m4Model);

	m4Translate = glm::translate(IDENTITY_M4, vector3(value + 1.0f, 2.0f, 0.0f));
	m4Model = m4Scale * m4Translate;

	cubes[6]->Render(m4Projection, m4View, m4Model);

	m4Translate = glm::translate(IDENTITY_M4, vector3(value + 1.0f, -1.0f, 0.0f));
	m4Model = m4Scale * m4Translate;

	cubes[7]->Render(m4Projection, m4View, m4Model);

	m4Translate = glm::translate(IDENTITY_M4, vector3(value - 1.0f, 0.0f, 0.0f));
	m4Model = m4Scale * m4Translate;

	cubes[8]->Render(m4Projection, m4View, m4Model);

	m4Translate = glm::translate(IDENTITY_M4, vector3(value - 1.0f, 1.0f, 0.0f));
	m4Model = m4Scale * m4Translate;

	cubes[9]->Render(m4Projection, m4View, m4Model);

	m4Translate = glm::translate(IDENTITY_M4, vector3(value - 1.0f, 2.0f, 0.0f));
	m4Model = m4Scale * m4Translate;

	cubes[10]->Render(m4Projection, m4View, m4Model);

	m4Translate = glm::translate(IDENTITY_M4, vector3(value - 1.0f, -1.0f, 0.0f));
	m4Model = m4Scale * m4Translate;

	cubes[11]->Render(m4Projection, m4View, m4Model);

	m4Translate = glm::translate(IDENTITY_M4, vector3(value - 2.0f, 0.0f, 0.0f));
	m4Model = m4Scale * m4Translate;

	cubes[12]->Render(m4Projection, m4View, m4Model);

	m4Translate = glm::translate(IDENTITY_M4, vector3(value - 3.0f, 1.0f, 0.0f));
	m4Model = m4Scale * m4Translate;

	cubes[13]->Render(m4Projection, m4View, m4Model);

	m4Translate = glm::translate(IDENTITY_M4, vector3(value - 2.0f, 2.0f, 0.0f));
	m4Model = m4Scale * m4Translate;

	cubes[14]->Render(m4Projection, m4View, m4Model);

	m4Translate = glm::translate(IDENTITY_M4, vector3(value - 2.0f, -1.0f, 0.0f));
	m4Model = m4Scale * m4Translate;

	cubes[15]->Render(m4Projection, m4View, m4Model);

	m4Translate = glm::translate(IDENTITY_M4, vector3(value + 2.0f, 0.0f, 0.0f));
	m4Model = m4Scale * m4Translate;

	cubes[16]->Render(m4Projection, m4View, m4Model);

	m4Translate = glm::translate(IDENTITY_M4, vector3(value + 3.0f, 1.0f, 0.0f));
	m4Model = m4Scale * m4Translate;

	cubes[17]->Render(m4Projection, m4View, m4Model);

	m4Translate = glm::translate(IDENTITY_M4, vector3(value + 2.0f, 2.0f, 0.0f));
	m4Model = m4Scale * m4Translate;

	cubes[18]->Render(m4Projection, m4View, m4Model);

	m4Translate = glm::translate(IDENTITY_M4, vector3(value + 2.0f, -1.0f, 0.0f));
	m4Model = m4Scale * m4Translate;

	cubes[19]->Render(m4Projection, m4View, m4Model);

	m4Translate = glm::translate(IDENTITY_M4, vector3(value + 3.0f, 2.0f, 0.0f));
	m4Model = m4Scale * m4Translate;

	cubes[20]->Render(m4Projection, m4View, m4Model);

	m4Translate = glm::translate(IDENTITY_M4, vector3(value + 3.0f, 0.0f, 0.0f));
	m4Model = m4Scale * m4Translate;

	cubes[21]->Render(m4Projection, m4View, m4Model);

	m4Translate = glm::translate(IDENTITY_M4, vector3(value + 3.0f, -1.0f, 0.0f));
	m4Model = m4Scale * m4Translate;

	cubes[22]->Render(m4Projection, m4View, m4Model);

	m4Translate = glm::translate(IDENTITY_M4, vector3(value - 3.0f, 2.0f, 0.0f));
	m4Model = m4Scale * m4Translate;

	cubes[23]->Render(m4Projection, m4View, m4Model);

	m4Translate = glm::translate(IDENTITY_M4, vector3(value - 3.0f, 0.0f, 0.0f));
	m4Model = m4Scale * m4Translate;

	cubes[24]->Render(m4Projection, m4View, m4Model);

	m4Translate = glm::translate(IDENTITY_M4, vector3(value - 3.0f, -1.0f, 0.0f));
	m4Model = m4Scale * m4Translate;

	cubes[25]->Render(m4Projection, m4View, m4Model);
	
	m4Translate = glm::translate(IDENTITY_M4, vector3(value - 4.0f, 0.0f, 0.0f));
	m4Model = m4Scale * m4Translate;
	
	cubes[26]->Render(m4Projection, m4View, m4Model);

	m4Translate = glm::translate(IDENTITY_M4, vector3(value - 4.0f, 1.0f, 0.0f));
	m4Model = m4Scale * m4Translate;

	cubes[27]->Render(m4Projection, m4View, m4Model);

	m4Translate = glm::translate(IDENTITY_M4, vector3(value - 5.0f, 0.0f, 0.0f));
	m4Model = m4Scale * m4Translate;

	cubes[28]->Render(m4Projection, m4View, m4Model);

	m4Translate = glm::translate(IDENTITY_M4, vector3(value - 5.0f, -1.0f, 0.0f));
	m4Model = m4Scale * m4Translate;

	cubes[29]->Render(m4Projection, m4View, m4Model);

	m4Translate = glm::translate(IDENTITY_M4, vector3(value - 5.0f, -2.0f, 0.0f));
	m4Model = m4Scale * m4Translate;

	cubes[30]->Render(m4Projection, m4View, m4Model);

	m4Translate = glm::translate(IDENTITY_M4, vector3(value + 4.0f, 0.0f, 0.0f));
	m4Model = m4Scale * m4Translate;

	cubes[31]->Render(m4Projection, m4View, m4Model);

	m4Translate = glm::translate(IDENTITY_M4, vector3(value + 4.0f, 1.0f, 0.0f));
	m4Model = m4Scale * m4Translate;

	cubes[32]->Render(m4Projection, m4View, m4Model);

	m4Translate = glm::translate(IDENTITY_M4, vector3(value + 5.0f, 0.0f, 0.0f));
	m4Model = m4Scale * m4Translate;

	cubes[33]->Render(m4Projection, m4View, m4Model);

	m4Translate = glm::translate(IDENTITY_M4, vector3(value + 5.0f, -1.0f, 0.0f));
	m4Model = m4Scale * m4Translate;

	cubes[34]->Render(m4Projection, m4View, m4Model);

	m4Translate = glm::translate(IDENTITY_M4, vector3(value + 5.0f, -2.0f, 0.0f));
	m4Model = m4Scale * m4Translate;

	cubes[35]->Render(m4Projection, m4View, m4Model);
	
	m4Translate = glm::translate(IDENTITY_M4, vector3(value + 3.0f, -2.0f, 0.0f));
	m4Model = m4Scale * m4Translate;

	cubes[36]->Render(m4Projection, m4View, m4Model);

	m4Translate = glm::translate(IDENTITY_M4, vector3(value + 2.0f, -3.0f, 0.0f));
	m4Model = m4Scale * m4Translate;

	cubes[37]->Render(m4Projection, m4View, m4Model);

	m4Translate = glm::translate(IDENTITY_M4, vector3(value + 1.0f, -3.0f, 0.0f));
	m4Model = m4Scale * m4Translate;

	cubes[38]->Render(m4Projection, m4View, m4Model);

	m4Translate = glm::translate(IDENTITY_M4, vector3(value - 3.0f, -2.0f, 0.0f));
	m4Model = m4Scale * m4Translate;

	cubes[39]->Render(m4Projection, m4View, m4Model);

	m4Translate = glm::translate(IDENTITY_M4, vector3(value - 2.0f, -3.0f, 0.0f));
	m4Model = m4Scale * m4Translate;

	cubes[40]->Render(m4Projection, m4View, m4Model);

	m4Translate = glm::translate(IDENTITY_M4, vector3(value - 1.0f, -3.0f, 0.0f));
	m4Model = m4Scale * m4Translate;

	cubes[41]->Render(m4Projection, m4View, m4Model);

	m4Translate = glm::translate(IDENTITY_M4, vector3(value - 2.0f, 3.0f, 0.0f));
	m4Model = m4Scale * m4Translate;

	cubes[42]->Render(m4Projection, m4View, m4Model);

	m4Translate = glm::translate(IDENTITY_M4, vector3(value - 3.0f, 4.0f, 0.0f));
	m4Model = m4Scale * m4Translate;

	cubes[43]->Render(m4Projection, m4View, m4Model);

	m4Translate = glm::translate(IDENTITY_M4, vector3(value + 2.0f, 3.0f, 0.0f));
	m4Model = m4Scale * m4Translate;

	cubes[44]->Render(m4Projection, m4View, m4Model);

	m4Translate = glm::translate(IDENTITY_M4, vector3(value + 3.0f, 4.0f, 0.0f));
	m4Model = m4Scale * m4Translate;

	cubes[45]->Render(m4Projection, m4View, m4Model);
	
	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	for (int i = 0; i < 46; i++)
	{
		SafeDelete(cubes[i]);
	}

	//SafeDelete(m_pMesh);

	//release GUI
	ShutdownGUI();
}