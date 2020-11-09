#include "MyOctant.h"

using namespace Simplex;

uint MyOctant::m_uOctantCount = 0;
uint MyOctant::m_uMaxLevel = 3;
uint MyOctant::m_uIdealEntityCount = 5;
uint MyOctant::GetOctantCount() 
{
	return m_uOctantCount; 
}

//Creating methods in same order as MyEntity.cpp does
// Init -> Swap -> Release -> Constructors -> copy operator
void  MyOctant::Init(void)
{
	//Initializing everything that is created at the beginning of MyOctant.h according to their values
	m_uChildren = 0;
	m_fSize = 0.0f;
	m_uID = m_uOctantCount;
	m_uLevel = 0;
	m_v3Center = vector3(0.0f);
	m_v3Min = vector3(0.0f);
	m_v3Max = vector3(0.0f);

	//MyRigidBody.cpp does this for the mesh manager, I assume both managers need this??
	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	for (uint i = 0; i < 8; i++)
	{
		m_pChild[i] = nullptr;
	}	
	m_pRoot = nullptr;
	m_pParent = nullptr;
}


void  MyOctant::Swap(MyOctant& other)
{
	//Swapping everything, borrowed from MyRigidBody to get syntax properly
	//std::swap(m_pMeshMngr, other.m_pMeshMngr);
	std::swap(m_uChildren, other.m_uChildren);
	std::swap(m_fSize, other.m_fSize);
	std::swap(m_uID, other.m_uID);	
	

	//std::swap(m_pEntityMngr, other.m_pEntityMngr);

	std::swap(m_v3Center, other.m_v3Center);
	std::swap(m_v3Min, other.m_v3Min);
	std::swap(m_v3Max, other.m_v3Max);

	std::swap(m_pRoot, other.m_pRoot);
	std::swap(m_lChild, other.m_lChild);

	std::swap(m_uLevel, other.m_uLevel);
	
	std::swap(m_pParent, other.m_pParent);	

	for (uint i = 0; i < 8; i++)
	{
		std::swap(m_pChild[i], other.m_pChild[i]);
	}

	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

}

//Clearing data
void  MyOctant::Release(void)
{
	//Kill all the child level nodes and their leaves if its level 0
	if (m_uLevel == 0)
	{
		KillBranches();
	}

	m_uChildren = 0;
	m_fSize = 0.0f;
	m_EntityList.clear();
	m_lChild.clear();	
}

//Constructor
 MyOctant::MyOctant(uint a_nMaxLevel, uint a_nIdealEntityCount)
{
	//Most of our contructors start with Init
	Init();

	m_uOctantCount = 0;
	m_uMaxLevel = a_nMaxLevel;
	m_uIdealEntityCount = a_nIdealEntityCount;
	
	//The id is the same as the current octant count
	m_uID = m_uOctantCount;

	//The root is equal to itself
	m_pRoot = this;

	m_lChild.clear();

	std::vector<vector3> maxMins;

	uint objects = m_pEntityMngr->GetEntityCount();

	for (uint i = 0; i < objects; i++)
	{
		//Creating maxs and mins with the entities  from the manager
		MyEntity* entity = m_pEntityMngr->GetEntity(i);
		MyRigidBody* rigidBody = entity->GetRigidBody();
		maxMins.push_back(rigidBody->GetMinGlobal());
		maxMins.push_back(rigidBody->GetMaxGlobal());
	}

	MyRigidBody* rigidBody = new MyRigidBody(maxMins);
	vector3 halfWidth = rigidBody->GetHalfWidth();

	//Finding max
	float max = halfWidth.x;

	for (int i = 1; i < 3; i++)
	{
		if (max < halfWidth[i])
			max = halfWidth[i];
	}

	vector3 center = rigidBody->GetCenterLocal();
	maxMins.clear();
	SafeDelete(rigidBody);

	m_fSize = max * 2.0f;
	m_v3Center = center;
	m_v3Max = m_v3Center + (vector3(max));
	m_v3Min = m_v3Center - (vector3(max));

	m_uOctantCount++;

	ConstructTree(m_uMaxLevel);
}

//Other contructor using center and size
 MyOctant::MyOctant(vector3 a_v3Center, float a_fSize)
{
	Init();

	m_v3Center = a_v3Center;
	m_fSize = a_fSize;

	m_v3Max = m_v3Center + (vector3(m_fSize) / 2.0f);
	m_v3Min = m_v3Center - (vector3(m_fSize) / 2.0f);

	m_uOctantCount++;
}

 MyOctant::MyOctant(MyOctant const& other)
{
	m_uChildren = other.m_uChildren;
	m_v3Center = other.m_v3Center;
	m_v3Max = other.m_v3Max;
	m_v3Min = other.m_v3Min;
	m_fSize = other.m_fSize;
	m_uID = other.m_uID;
	m_uLevel = other.m_uLevel;
	
	m_pParent = other.m_pParent;
	m_pRoot = other.m_pRoot;
	m_lChild = other.m_lChild;

	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	for (uint i = 0; i < 8; i++)
	{
		m_pChild[i] = other.m_pChild[i];
	}
}

//Swapping everything from new octant to the old one
MyOctant&  MyOctant::operator=(MyOctant const& other)
{
	if (this == &other)
	{
		return *this;
	}
	
	Release();
	Init();
	MyOctant tempOct(other);
	Swap(tempOct);
}

 MyOctant::~MyOctant(void)
{
	Release();
}


void  MyOctant::Subdivide(void)
{
	//Return if it reaches the max
	if (m_uLevel >= m_uMaxLevel)
	{
		return;
	}

	//If its been divided, return
	if (m_uChildren != 0)
	{
		return;
	}

	m_uChildren = 8;

	float size = m_fSize / 4.0f;
	float size2 = size * 2.0f;

	//creating the different octants
	vector3 center = m_v3Center;
	center.x -= size;
	center.y -= size;
	center.z -= size;

	//First 4 are the bottom
	//back left
	m_pChild[0] = new MyOctant(center, size2);
		
	//back right
	center.x += size2;
	m_pChild[1] = new MyOctant(center, size2);

	//front right
	center.z += size2;
	m_pChild[2] = new MyOctant(center, size2);

	//front left
	center.x -= size2;
	m_pChild[3] = new MyOctant(center, size2);

	//Next 4 are the top
	//front left
	center.y += size2;
	m_pChild[4] = new MyOctant(center, size2);

	//back left
	center.z -= size2;
	m_pChild[5] = new MyOctant(center, size2);

	//back right
	center.x += size2;
	m_pChild[6] = new MyOctant(center, size2);

	//front right
	center.z += size2;
	m_pChild[7] = new MyOctant(center, size2);

	for (uint i = 0; i < 8; i++)
	{
		m_pChild[i]->m_pRoot = m_pRoot;
		m_pChild[i]->m_pParent = this;
		m_pChild[i]->m_uLevel = m_uLevel + 1;

		//checking to see if the child has more than the ideal count
		if (m_pChild[i]->ContainsMoreThan(m_uIdealEntityCount))
		{
			//subdivide if so
			m_pChild[i]->Subdivide();
		}
	}
}

void  MyOctant::ConstructList(void)
{
	//Construct lists for children
	for (uint i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->ConstructList();
	}

	if (m_EntityList.size() > 0)
	{
		//adding the octant to the root's child list
		m_pRoot->m_lChild.push_back(this);
	}
}

bool  MyOctant::ContainsMoreThan(uint a_nEntities)
{
	uint count = 0;
	uint numOfEntities = m_pEntityMngr->GetEntityCount();

	for (uint i = 0; i < numOfEntities; i++)
	{
		if (IsColliding(i))
		{
			count++;			
		}
		if (count > a_nEntities)
		{
			return true;
		}
	}
	return false;	
}

void  MyOctant::ConstructTree(uint a_nMaxLevel)
{
	//only allow the root to run this
	if (m_uLevel != 0)
		return;

	m_uMaxLevel = a_nMaxLevel;
	m_uOctantCount = 1;
	m_EntityList.clear();
	KillBranches();
	m_lChild.clear();

	//if its the base, subdivide
	if (ContainsMoreThan(m_uIdealEntityCount))
		Subdivide();

	//Assign ids
	AssignIDtoEntity();

	ConstructList();
}

void  MyOctant::ClearEntityList(void)
{
	//Loop through children and clear their entity lists
	for (uint i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->ClearEntityList();
	}

	m_EntityList.clear();
}

void  MyOctant::AssignIDtoEntity(void)
{
	//Loop until you find a leaf aka no children
	for (uint i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->AssignIDtoEntity();
	}

	//if there are no children
	if (m_uChildren == 0)
	{
		uint entities = m_pEntityMngr->GetEntityCount();

		for (uint i = 0; i < entities; i++)
		{
			if (IsColliding(i))
			{
				m_EntityList.push_back(i);
				m_pEntityMngr->AddDimension(i, m_uID);
			}
		}
	}
}

bool  MyOctant::IsColliding(uint a_uRBIndex)
{
	uint numOfEntities = m_pEntityMngr->GetEntityCount();

	//No collision if index is too high
	if (a_uRBIndex >= numOfEntities)
		return false;

	//grab the entity at the given index
	MyEntity* entity = m_pEntityMngr->GetEntity(a_uRBIndex);
	//Get the rigidbody of the entity
	MyRigidBody* rigidBody = entity->GetRigidBody();

	//Getting max and min of the rigid body to check with AABB
	vector3 maxOther = rigidBody->GetMaxGlobal();
	vector3 minOther = rigidBody->GetMinGlobal();

	//Classic AABB method, if any min is greater than any max, return false, if not, return true
	//Trying to do this the other way around initially slowed the program down a lot, not sure why so i reversed it
	if (maxOther.x < m_v3Min.x)
		return false;

	if (m_v3Max.x < minOther.x)
		return false;

	if (m_v3Max.y < minOther.y)
		return false;

	if (maxOther.y < m_v3Min.y)
		return false;

	if (maxOther.z < m_v3Min.z)
		return false;

	if (m_v3Max.z < minOther.z)
		return false;

	return true;
}

void  MyOctant::Display(uint a_nIndex, vector3 a_v3Color)
{
	//Display the render wire box at the id
	if (m_uID == a_nIndex)
	{
		//Adding the cube to the center and making it the correct size
		m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) * glm::scale(vector3(m_fSize)), a_v3Color, RENDER_WIRE);
		return;
	}

	//If it is not at the proper one, loop through children
	for (uint i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->Display(a_nIndex);
	}
}

void  MyOctant::Display(vector3 a_v3Color)
{
	//Same as other display but this one does not use an index to find a specifc value
	for (uint i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->Display(a_v3Color);
	}

	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) * glm::scale(vector3(m_fSize)), a_v3Color, RENDER_WIRE);
}

void  MyOctant::DisplayLeafs(vector3 a_v3Color)
{
	uint leaves = m_lChild.size();

	for (uint i = 0; i < leaves; i++)
	{
		//Make all the children display leaves, similar to how kill branches work
		m_lChild[i]->DisplayLeafs(a_v3Color);
	}
	//Just like other display methods
	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) * glm::scale(vector3(m_fSize)), a_v3Color, RENDER_WIRE);
}

void  MyOctant::KillBranches(void)
{
	for (uint i = 0; i < m_uChildren; i++)
	{
		//Every child may have children so they need to delete them as well so make another call
		m_pChild[i]->KillBranches();
		//Delete the child and set it to null ptr
		delete m_pChild[i];
		m_pChild[i] = nullptr;
	}

	//Set the children num to 0 because they are all gone now
	m_uChildren = 0;
}