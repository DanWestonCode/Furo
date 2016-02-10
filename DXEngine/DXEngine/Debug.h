#ifndef Debug_h__
#define Debug_h__

__declspec(align(16)) class Debug
{
public:
	static Debug *Instance()
	{
		if (!m_Debug)
		{
			m_Debug = new Debug;
		}
		return m_Debug;
	}

	Debug();
	~Debug();
	void* operator new(size_t);
	void operator delete(void*);


	void Log(float);

private:
	static Debug *m_Debug;
};
#endif // Debug_h__
