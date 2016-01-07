#include "Cube.h"

#include "Cube.h"
#include "FluidHelper.h"

Cube::Cube()
{
	m_VertexBuffer = nullptr;
	m_IndexBuffer = nullptr;
	m_ColorShader = nullptr;

	vel = 5.0f;
}

Cube::Cube(const Cube& other)
{
}

Cube::~Cube()
{

}

HRESULT Cube::Initialize(ID3D11Device* device, HWND hwnd)
{
	HRESULT result;
	unsigned long* indices;

	//Set up shader
	//Create the texture shader object.
	m_ColorShader = new ColorShader;
	// Initialize the texture shader object.
	result = m_ColorShader->Initialize(device, hwnd);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return result;
	}

	m_VertexCount = 6 * 6;
	// Set the number of indices in the index array.
	m_IndexCount = m_VertexCount;

	// Create the vertex array.
	m_VerticesColorVL = new ColorVL[m_VertexCount];

	// Create the index array.
	indices = new unsigned long[m_IndexCount];
	if (!indices)
	{
		return false;
	}

	//set up indices
	for (int i = 0; i < m_VertexCount; i++)
	{
		indices[i] = i;
	}

	float m_floatSize = 20;

	int vert = 0;
	Color colour = Color(1.0f,0.0f,0.0f,1.0f);
	Color colour2 = Color(0.0f, 1.0f, 0.0f, 1.0f);
	Color colour3 = Color(0.0f, 0.0f, 1.0f, 1.0f);
	Color colour4 = Color(1.0f, 1.0f, 0.0f, 1.0f);
	Color colour5 = Color(0.0f, 1.0f, 1.0f, 1.0f);
	Color colour6 = Color(1.0f, 0.0f, 1.0f, 1.0f);

	//top
	m_VerticesColorVL[vert].color = colour;
	m_VerticesColorVL[vert++].position = Vector3(m_pos.x, m_pos.y + m_floatSize, m_pos.z + m_floatSize);
	m_VerticesColorVL[vert].color = colour;
	m_VerticesColorVL[vert++].position = Vector3(m_pos.x + m_floatSize, m_pos.y + m_floatSize, m_pos.z + m_floatSize);
	m_VerticesColorVL[vert].color = colour;
	m_VerticesColorVL[vert++].position = Vector3(m_pos.x + m_floatSize, m_pos.y + m_floatSize, m_pos.z);

	m_VerticesColorVL[vert].color = colour;
	m_VerticesColorVL[vert++].position = Vector3(m_pos.x + m_floatSize, m_pos.y + m_floatSize, m_pos.z);
	m_VerticesColorVL[vert].color = colour;
	m_VerticesColorVL[vert++].position = Vector3(m_pos.x, m_pos.y + m_floatSize, m_pos.z);
	m_VerticesColorVL[vert].color = colour;
	m_VerticesColorVL[vert++].position = Vector3(m_pos.x, m_pos.y + m_floatSize, m_pos.z + m_floatSize);

	//front
	m_VerticesColorVL[vert].color = colour2;
	m_VerticesColorVL[vert++].position = Vector3(m_pos.x, m_pos.y, m_pos.z + m_floatSize);
	m_VerticesColorVL[vert].color = colour2;
	m_VerticesColorVL[vert++].position = Vector3(m_pos.x + m_floatSize, m_pos.y, m_pos.z + m_floatSize);
	m_VerticesColorVL[vert].color = colour2;
	m_VerticesColorVL[vert++].position = Vector3(m_pos.x, m_pos.y + m_floatSize, m_pos.z + m_floatSize);

	m_VerticesColorVL[vert].color = colour2;
	m_VerticesColorVL[vert++].position = Vector3(m_pos.x + m_floatSize, m_pos.y, m_pos.z + m_floatSize);
	m_VerticesColorVL[vert].color = colour2;
	m_VerticesColorVL[vert++].position = Vector3(m_pos.x + m_floatSize, m_pos.y + m_floatSize, m_pos.z + m_floatSize);
	m_VerticesColorVL[vert].color = colour2;
	m_VerticesColorVL[vert++].position = Vector3(m_pos.x, m_pos.y + m_floatSize, m_pos.z + m_floatSize);

	//right
	m_VerticesColorVL[vert].color = colour3;
	m_VerticesColorVL[vert++].position = Vector3(m_pos.x + m_floatSize, m_pos.y, m_pos.z);
	m_VerticesColorVL[vert].color = colour3;
	m_VerticesColorVL[vert++].position = Vector3(m_pos.x + m_floatSize, m_pos.y + m_floatSize, m_pos.z);
	m_VerticesColorVL[vert].color = colour3;
	m_VerticesColorVL[vert++].position = Vector3(m_pos.x + m_floatSize, m_pos.y + m_floatSize, m_pos.z + m_floatSize);

	m_VerticesColorVL[vert].color = colour3;
	m_VerticesColorVL[vert++].position = Vector3(m_pos.x + m_floatSize, m_pos.y, m_pos.z);
	m_VerticesColorVL[vert].color = colour3;
	m_VerticesColorVL[vert++].position = Vector3(m_pos.x + m_floatSize, m_pos.y + m_floatSize, m_pos.z + m_floatSize);
	m_VerticesColorVL[vert].color = colour3;
	m_VerticesColorVL[vert++].position = Vector3(m_pos.x + m_floatSize, m_pos.y, m_pos.z + m_floatSize);

	//down
	m_VerticesColorVL[vert].color = colour4;
	m_VerticesColorVL[vert++].position = Vector3(m_pos.x + m_floatSize, m_pos.y, m_pos.z);
	m_VerticesColorVL[vert].color = colour4;
	m_VerticesColorVL[vert++].position = Vector3(m_pos.x, m_pos.y, m_pos.z + m_floatSize);
	m_VerticesColorVL[vert].color = colour4;
	m_VerticesColorVL[vert++].position = Vector3(m_pos.x + m_floatSize, m_pos.y, m_pos.z + m_floatSize);

	m_VerticesColorVL[vert].color = colour4;
	m_VerticesColorVL[vert++].position = Vector3(m_pos.x + m_floatSize, m_pos.y, m_pos.z);
	m_VerticesColorVL[vert].color = colour4;
	m_VerticesColorVL[vert++].position = Vector3(m_pos.x, m_pos.y, m_pos.z);
	m_VerticesColorVL[vert].color = colour4;
	m_VerticesColorVL[vert++].position = Vector3(m_pos.x, m_pos.y, m_pos.z + m_floatSize);

	//back
	m_VerticesColorVL[vert].color = colour5;
	m_VerticesColorVL[vert++].position = Vector3(m_pos.x, m_pos.y, m_pos.z);
	m_VerticesColorVL[vert].color = colour5;
	m_VerticesColorVL[vert++].position = Vector3(m_pos.x, m_pos.y + m_floatSize, m_pos.z);
	m_VerticesColorVL[vert].color = colour5;
	m_VerticesColorVL[vert++].position = Vector3(m_pos.x + m_floatSize, m_pos.y, m_pos.z);

	m_VerticesColorVL[vert].color = colour5;
	m_VerticesColorVL[vert++].position = Vector3(m_pos.x, m_pos.y + m_floatSize, m_pos.z);
	m_VerticesColorVL[vert].color = colour5;
	m_VerticesColorVL[vert++].position = Vector3(m_pos.x + m_floatSize, m_pos.y + m_floatSize, m_pos.z);
	m_VerticesColorVL[vert].color = colour5;
	m_VerticesColorVL[vert++].position = Vector3(m_pos.x + m_floatSize, m_pos.y, m_pos.z);

	//left
	m_VerticesColorVL[vert].color = colour6;
	m_VerticesColorVL[vert++].position = Vector3(m_pos.x, m_pos.y + m_floatSize, m_pos.z + m_floatSize);
	m_VerticesColorVL[vert].color = colour6;
	m_VerticesColorVL[vert++].position = Vector3(m_pos.x, m_pos.y + m_floatSize, m_pos.z);
	m_VerticesColorVL[vert].color = colour6;
	m_VerticesColorVL[vert++].position = Vector3(m_pos.x, m_pos.y, m_pos.z);

	m_VerticesColorVL[vert].color = colour6;
	m_VerticesColorVL[vert++].position = Vector3(m_pos.x, m_pos.y, m_pos.z);
	m_VerticesColorVL[vert].color = colour6;
	m_VerticesColorVL[vert++].position = Vector3(m_pos.x, m_pos.y, m_pos.z + m_floatSize);
	m_VerticesColorVL[vert].color = colour6;
	m_VerticesColorVL[vert++].position = Vector3(m_pos.x, m_pos.y + m_floatSize, m_pos.z + m_floatSize);

	BuildImmuatbleVB(device, m_VertexCount, m_VerticesColorVL);
	BuildImmuatbleIB(device, indices);

	/*BuildStaticVB(device, m_VertexCount, m_VerticesColorVL);
	BuildIndexBuffer(device, indices);*/

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	/*delete[] m_VerticesColorVLColorVLTextureVL;
	m_VerticesColorVLColorVLTextureVL = 0;*/

	delete[] indices;
	indices = 0;
}

void Cube::Render(ID3D11DeviceContext* deviceContext, XMMATRIX* worldMatrix, XMMATRIX* viewMatrix, XMMATRIX* projectionMatrix)
{
	//VertexModel::Render(deviceContext);

	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(ColorVL);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Render the model using the texture shader.
	m_ColorShader->Render(deviceContext, 36, m_worldMatrix, *viewMatrix, *projectionMatrix);
}

void Cube::Shutdown()
{
	ReleaseTexture();
	// Release the texture shader object.
	if (m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
	}
	VertexModel::Shutdown();
}

void Cube::Update(float dt)
{
	if (InputManager::Instance()->IsKeyDown(DIK_Z))
		m_rot.x += vel*dt;
	if (InputManager::Instance()->IsKeyDown(DIK_X))
		m_rot.y += vel*dt;
	if (InputManager::Instance()->IsKeyDown(DIK_C))
		m_rot.z += vel*dt;

	VertexModel::Update(dt);
}

