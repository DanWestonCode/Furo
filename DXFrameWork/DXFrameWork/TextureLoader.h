#ifndef TextureLoader_h__
#define TextureLoader_h__

#include <DDSTextureLoader.h>
#include <d3d11.h>
class TextureLoader
{
public:
	TextureLoader();
	~TextureLoader();
	static TextureLoader *Instance()
	{
		if (!m_TextureLoader)
		{
			m_TextureLoader = new TextureLoader;
		}
		return m_TextureLoader;
	}

	ID3D11ShaderResourceView* LoadTexture(ID3D11Device*, WCHAR*);
	void DeleteInstance();	
private:
	static TextureLoader *m_TextureLoader;
};
#endif // TextureLoader_h__
