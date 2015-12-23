#include "TextureLoader.h"

TextureLoader *TextureLoader::m_TextureLoader = nullptr;

TextureLoader::TextureLoader()
{
	
}
TextureLoader::~TextureLoader()
{

}

ID3D11ShaderResourceView* TextureLoader::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	ID3D11ShaderResourceView* m_texture = nullptr;
	DirectX::CreateDDSTextureFromFile(device, filename, nullptr, &m_texture);
	return m_texture;
}
void TextureLoader::DeleteInstance()
{
	delete m_TextureLoader;
	m_TextureLoader = nullptr;
}


