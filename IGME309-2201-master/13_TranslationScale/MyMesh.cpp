#include "MyMesh.h"
void MyMesh::Init(void)
{
	m_bBinded = false;
	m_uVertexCount = 0;

	m_VAO = 0;
	m_VBO = 0;

	m_pShaderMngr = ShaderManager::GetInstance();
}
void MyMesh::Release(void)
{
	m_pShaderMngr = nullptr;

	if (m_VBO > 0)
		glDeleteBuffers(1, &m_VBO);

	if (m_VAO > 0)
		glDeleteVertexArrays(1, &m_VAO);

	m_lVertex.clear();
	m_lVertexPos.clear();
	m_lVertexCol.clear();
}
MyMesh::MyMesh()
{
	Init();
}
MyMesh::~MyMesh() { Release(); }
MyMesh::MyMesh(MyMesh& other)
{
	m_bBinded = other.m_bBinded;

	m_pShaderMngr = other.m_pShaderMngr;

	m_uVertexCount = other.m_uVertexCount;

	m_VAO = other.m_VAO;
	m_VBO = other.m_VBO;
}
MyMesh& MyMesh::operator=(MyMesh& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyMesh temp(other);
		Swap(temp);
	}
	return *this;
}
void MyMesh::Swap(MyMesh& other)
{
	std::swap(m_bBinded, other.m_bBinded);
	std::swap(m_uVertexCount, other.m_uVertexCount);

	std::swap(m_VAO, other.m_VAO);
	std::swap(m_VBO, other.m_VBO);

	std::swap(m_lVertex, other.m_lVertex);
	std::swap(m_lVertexPos, other.m_lVertexPos);
	std::swap(m_lVertexCol, other.m_lVertexCol);

	std::swap(m_pShaderMngr, other.m_pShaderMngr);
}
void MyMesh::CompleteMesh(vector3 a_v3Color)
{
	uint uColorCount = m_lVertexCol.size();
	for (uint i = uColorCount; i < m_uVertexCount; ++i)
	{
		m_lVertexCol.push_back(a_v3Color);
	}
}
void MyMesh::AddVertexPosition(vector3 a_v3Input)
{
	m_lVertexPos.push_back(a_v3Input);
	m_uVertexCount = m_lVertexPos.size();
}
void MyMesh::AddVertexColor(vector3 a_v3Input)
{
	m_lVertexCol.push_back(a_v3Input);
}
void MyMesh::CompileOpenGL3X(void)
{
	if (m_bBinded)
		return;

	if (m_uVertexCount == 0)
		return;

	CompleteMesh();

	for (uint i = 0; i < m_uVertexCount; i++)
	{
		//Position
		m_lVertex.push_back(m_lVertexPos[i]);
		//Color
		m_lVertex.push_back(m_lVertexCol[i]);
	}
	glGenVertexArrays(1, &m_VAO);//Generate vertex array object
	glGenBuffers(1, &m_VBO);//Generate Vertex Buffered Object

	glBindVertexArray(m_VAO);//Bind the VAO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);//Bind the VBO
	glBufferData(GL_ARRAY_BUFFER, m_uVertexCount * 2 * sizeof(vector3), &m_lVertex[0], GL_STATIC_DRAW);//Generate space for the VBO

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)0);

	// Color attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)(1 * sizeof(vector3)));

	m_bBinded = true;

	glBindVertexArray(0); // Unbind VAO
}
void MyMesh::Render(matrix4 a_mProjection, matrix4 a_mView, matrix4 a_mModel)
{
	// Use the buffer and shader
	GLuint nShader = m_pShaderMngr->GetShaderID("Basic");
	glUseProgram(nShader); 

	//Bind the VAO of this object
	glBindVertexArray(m_VAO);

	// Get the GPU variables by their name and hook them to CPU variables
	GLuint MVP = glGetUniformLocation(nShader, "MVP");
	GLuint wire = glGetUniformLocation(nShader, "wire");

	//Final Projection of the Camera
	matrix4 m4MVP = a_mProjection * a_mView * a_mModel;
	glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(m4MVP));
	
	//Solid
	glUniform3f(wire, -1.0f, -1.0f, -1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);  

	//Wire
	glUniform3f(wire, 1.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.f, -1.f);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);
	glDisable(GL_POLYGON_OFFSET_LINE);

	glBindVertexArray(0);// Unbind VAO so it does not get in the way of other objects
}
void MyMesh::AddTri(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft)
{
	//C
	//| \
	//A--B
	//This will make the triangle A->B->C 
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);
}
void MyMesh::AddQuad(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft, vector3 a_vTopRight)
{
	//C--D
	//|  |
	//A--B
	//This will make the triangle A->B->C and then the triangle C->B->D
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);

	AddVertexPosition(a_vTopLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopRight);
}
void MyMesh::GenerateCube(float a_fSize, vector3 a_v3Color)
{
	if (a_fSize < 0.01f)
		a_fSize = 0.01f;

	Release();
	Init();

	float fValue = a_fSize * 0.5f;
	//3--2
	//|  |
	//0--1

	vector3 point0(-fValue,-fValue, fValue); //0
	vector3 point1( fValue,-fValue, fValue); //1
	vector3 point2( fValue, fValue, fValue); //2
	vector3 point3(-fValue, fValue, fValue); //3

	vector3 point4(-fValue,-fValue,-fValue); //4
	vector3 point5( fValue,-fValue,-fValue); //5
	vector3 point6( fValue, fValue,-fValue); //6
	vector3 point7(-fValue, fValue,-fValue); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCuboid(vector3 a_v3Dimensions, vector3 a_v3Color)
{
	Release();
	Init();

	vector3 v3Value = a_v3Dimensions * 0.5f;
	//3--2
	//|  |
	//0--1
	vector3 point0(-v3Value.x, -v3Value.y, v3Value.z); //0
	vector3 point1(v3Value.x, -v3Value.y, v3Value.z); //1
	vector3 point2(v3Value.x, v3Value.y, v3Value.z); //2
	vector3 point3(-v3Value.x, v3Value.y, v3Value.z); //3

	vector3 point4(-v3Value.x, -v3Value.y, -v3Value.z); //4
	vector3 point5(v3Value.x, -v3Value.y, -v3Value.z); //5
	vector3 point6(v3Value.x, v3Value.y, -v3Value.z); //6
	vector3 point7(-v3Value.x, v3Value.y, -v3Value.z); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCone(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	std::vector<vector3 > points;

	//Create the circular base for the cone
	GLfloat angle1 = 0.0f;
	GLfloat angle2 = static_cast<GLfloat>(2 * PI) / a_nSubdivisions;

	for (int i = 0; i < a_nSubdivisions; i++)
	{
		vector3 temp = vector3(cos(angle1) * a_fRadius, sin(angle1) * a_fRadius, 0.0f);
		angle1 += angle2;
		points.push_back(temp);
	}

	//Add the triangles to physically make the base
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		AddTri(vector3(0.0f, 0.0f, 0.0f), points[i], points[(i + 1) % a_nSubdivisions]);
	}

	//Add triangles going to a point in the center of the circle at the specified height
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		AddTri(points[i], vector3(0.0f, 0.0f, a_fHeight), points[(i + 1) % a_nSubdivisions]);
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCylinder(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	std::vector<vector3 > points;

	//Create the circle for the first end of the cylinder
	GLfloat angle1 = 0.0f;
	GLfloat angle2 =static_cast<GLfloat>(2 * PI) / a_nSubdivisions;

	for (int i = 0; i < a_nSubdivisions; i++)
	{
		vector3 temp = vector3(cos(angle1) * a_fRadius, sin(angle1) * a_fRadius, 0.0f);
		angle1 += angle2;
		points.push_back(temp);
	}

	//Add the triangles to physically create the cylinder end
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		AddTri(vector3(0.0f, 0.0f, 0.0f), points[(i + 1) % a_nSubdivisions], points[i]);
	}

	//Store the current points into a new vector to use for later
	std::vector<vector3 > sidePoints;

	for (int i = 0; i < a_nSubdivisions; i++)
	{
		sidePoints.push_back(points[i]);
	}

	//Raise the z axis of all the points of the first end of the cylinder for the next end
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		points[i].z += a_fHeight / 2;
		points[(i + 1) % a_nSubdivisions].z += a_fHeight / 2;
	}

	//Add the triangles to create the next end
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		AddTri(vector3(0.0f, 0.0f, a_fHeight), points[i], points[(i + 1) % a_nSubdivisions]);
	}

	//Create quads to make the sides of the cylinder, connecting both ends of the cylinder
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		AddQuad(sidePoints[i], sidePoints[(i + 1) % a_nSubdivisions], points[i], points[(i + 1) % a_nSubdivisions]);
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTube(float a_fOuterRadius, float a_fInnerRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	// Inner radius circle ------------------------

	//Create the circle for the inner radius, this will give the points to where the outer circle chould end
	std::vector<vector3 > points2;
	GLfloat angle3 = 0.0f;
	GLfloat angle4 = static_cast<GLfloat>(2 * PI) / a_nSubdivisions;

	for (int i = 0; i < a_nSubdivisions; i++)
	{
		vector3 temp = vector3(cos(angle3) * a_fInnerRadius, sin(angle3) * a_fInnerRadius, 0.0f);
		angle3 += angle4;
		//points 2 is storing the points where the outer circle ends
		points2.push_back(temp);
	}

	//Outer radius circle ----------------
	
	//Create the circle for the outer edge as normal
	std::vector<vector3 > points;
	
	GLfloat angle1 = 0.0f;
	GLfloat angle2 = static_cast<GLfloat>(2 * PI) / a_nSubdivisions;

	for (int i = 0; i < a_nSubdivisions; i++)
	{
		vector3 temp = vector3(cos(angle1) * a_fOuterRadius, sin(angle1) * a_fOuterRadius, 0.0f);
		angle1 += angle2;
		points.push_back(temp);
	}

	//Create quads for both sides of the circle instead of triangles
	//This makes it a circle with a hollowed  center by using the points2 vector containin the inner circle info
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		AddQuad(points[(i + 1) % a_nSubdivisions], points[i], points2[(i + 1) % a_nSubdivisions], points2[i]);
		AddQuad(points2[(i + 1) % a_nSubdivisions], points2[i], points[(i + 1) % a_nSubdivisions], points[i]);
	}
	
	
	//Store the points for the first end of the tube into two different vectors before raising the end to the other side of the tube
	std::vector<vector3 > sidePoints;

	for (int i = 0; i < a_nSubdivisions; i++)
	{
		sidePoints.push_back(points[i]);
	}

	std::vector<vector3 > innerSidePoints;

	for (int i = 0; i < a_nSubdivisions; i++)
	{
		innerSidePoints.push_back(points2[i]);
	}
	
	//Raise the z coordinate of everything that was done so it will mirror the end of the tube on the opposite side
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		points[i].z += a_fHeight;
		points2[i].z += a_fHeight;
		points[(i + 1) % a_nSubdivisions].z += a_fHeight;
		points2[(i + 1) % a_nSubdivisions].z += a_fHeight;
	}

	//Create the quads for the opposite side of the tube to recreate what was done for the first end of the tube
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		AddQuad(points[(i + 1) % a_nSubdivisions], points[i], points2[(i + 1) % a_nSubdivisions], points2[i]);
		AddQuad(points2[(i + 1) % a_nSubdivisions], points2[i], points[(i + 1) % a_nSubdivisions], points[i]);
	}

	//Create the sides of the tube using the previously stored points to connect them to the current points
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		AddQuad(sidePoints[i], sidePoints[(i + 1) % a_nSubdivisions], points[i], points[(i + 1) % a_nSubdivisions]);
		AddQuad(points[i], points[(i + 1) % a_nSubdivisions], innerSidePoints[i], innerSidePoints[(i + 1) % a_nSubdivisions]);
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTorus(float a_fOuterRadius, float a_fInnerRadius, int a_nSubdivisionsA, int a_nSubdivisionsB, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_nSubdivisionsA < 3)
		a_nSubdivisionsA = 3;
	if (a_nSubdivisionsA > 360)
		a_nSubdivisionsA = 360;

	if (a_nSubdivisionsB < 3)
		a_nSubdivisionsB = 3;
	if (a_nSubdivisionsB > 360)
		a_nSubdivisionsB = 360;

	Release();
	Init();

	// Replace this with your code
	
	//Create the circle for the first end of the sphere
	std::vector<vector3> points;
	GLfloat angle1 = 0.0f;
	GLfloat angle2 = static_cast<GLfloat>(2 * PI) / a_nSubdivisionsA;

	for (int i = 0; i < a_nSubdivisionsA; i++)
	{
		vector3 temp = vector3(cos(angle1) * a_fInnerRadius, sin(angle1) * a_fInnerRadius, 0.0f);
		angle1 += angle2;
		points.push_back(temp);
	}

	std::vector<vector3> points2;
	GLfloat angle3 = 0.0f;
	GLfloat angle4 = static_cast<GLfloat>(2 * PI) / a_nSubdivisionsA;

	for (int i = 0; i < a_nSubdivisionsA; i++)
	{
		vector3 temp = vector3(cos(angle3) * a_fOuterRadius, sin(angle3) * a_fOuterRadius, 0.0f);
		angle3 += angle4;
		points2.push_back(temp);
	}

	//Add the triangles to physically create the sphere end
	//for (int i = 0; i < a_nSubdivisionsA; i++)
	//{
		//AddTri(vector3(0.0f, 0.0f, 0.0f), points[i], points[(i + 1) % a_nSubdivisionsA]);
	//}

	for (int i = 0; i < a_nSubdivisionsA; i++)
	{
		AddQuad(points[i], points[(i + 1) % a_nSubdivisionsA], points2[i], points2[(i + 1) % a_nSubdivisionsA]);
	}

	//Copying the points to a new vector
	std::vector<vector3> sidePoints;
	std::vector<vector3> sidePoints2;

	for (int i = 0; i < a_nSubdivisionsA; i++)
	{
		sidePoints.push_back(points[i]);
		sidePoints2.push_back(points2[i]);
	}

	//Sending the points up to make the second layer
	for (int i = 0; i < a_nSubdivisionsA; i++)
	{
		points[i].z += a_fInnerRadius;
		points2[i].z += a_fInnerRadius;
	}

	for (int i = 0; i < a_nSubdivisionsA; i++)
	{
		AddQuad(points2[i], points2[(i + 1) % a_nSubdivisionsA], points[i], points[(i + 1) % a_nSubdivisionsA]);
	}

	for (int i = 0; i < a_nSubdivisionsA; i++)
	{
		AddQuad(points2[i], points2[(i + 1) % a_nSubdivisionsA], sidePoints2[i], sidePoints2[(i + 1) % a_nSubdivisionsA]);
		AddQuad(sidePoints2[i], sidePoints2[(i + 1) % a_nSubdivisionsA], points2[i], points2[(i + 1) % a_nSubdivisionsA]);
	}

	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateSphere(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	//Sets minimum and maximum of subdivisions
	if (a_nSubdivisions < 1)
	{
		GenerateCube(a_fRadius * 2.0f, a_v3Color);
		return;
	}
	if (a_nSubdivisions > 6)
		a_nSubdivisions = 6;

	Release();
	Init();

	// Replace this with your code
	
	std::vector<vector3 > points;

	//Create the circlee for the first end of the sphere
	GLfloat angle1 = 0.0f;
	GLfloat angle2 = static_cast<GLfloat>(2 * PI) / a_nSubdivisions;

	for (int i = 0; i < a_nSubdivisions; i++)
	{
		vector3 temp = vector3(cos(angle2) * sin(angle1) * a_fRadius, sin(angle2) * sin(angle1) * a_fRadius, cos(angle1) * a_fRadius);
		angle1 += angle2;
		points.push_back(temp);
	}

	//Add the triangles to physically create the sphere end
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		AddTri(vector3(0.0f, 0.0f, 0.0f), points[(i + 1) % a_nSubdivisions], points[i]);		
	}

	std::vector<vector3> sidePoints;

	for (int i = 0; i < a_nSubdivisions; i++)
	{
		points[i] += a_fRadius;
		sidePoints.push_back(points[i]);
	}

	for (int i = 0; i < a_nSubdivisions; i++)
	{
		AddQuad(sidePoints[(i + 1) % a_nSubdivisions], sidePoints[i], points[(i + 1) % a_nSubdivisions], points[i]);
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}