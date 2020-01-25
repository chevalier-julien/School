#pragma once

#include <vector>
#include <algorithm>

struct GLFWwindow;

class IKeyCallbackHandler
{
public:
	virtual void Execute(int key, int scancode, int action, int mods) = 0;
};

template< typename T >
class KeyCallbackHandler : public IKeyCallbackHandler
{
public:
	typedef T* Ptr;
	typedef void (T::*Callback)(int, int, int, int);

public:
	KeyCallbackHandler(Ptr object, Callback function)
		: m_object(object), m_function(function) {}

	void Execute(int key, int scancode, int action, int mods)
	{
		m_object->*m_function(key, scancode, action, mods);
	}

private:
	Ptr m_object;
	Callback m_function;
};


class SvcInput
{
public:
	friend void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

public:
	static bool Create();
	static void Destroy();

	SvcInput* GetInstance();

	template< typename T >
	KeyCallbackHandler< T > * RegisterKeyCallback(typename KeyCallbackHandler< T >::Ptr object, typename KeyCallbackHandler< T >::Callback function)
	{
		m_keyCallbackHandlers.push_back(new KeyCallbackHandler< T >(object, function));
	}

	void UnregisterKeyCallback(IKeyCallbackHandler * handler)
	{
		std::vector< IKeyCallbackHandler * >:: iterator it = std::find(m_keyCallbackHandlers.begin(), m_keyCallbackHandlers.end(), handler);
		if (it != m_keyCallbackHandlers.end())
		{
			m_keyCallbackHandlers.erase(it);
		}
		delete handler;
	}

private:
	SvcInput() {}
	~SvcInput() {}

	bool Init();
	void Release();

private:
	static SvcInput* ms_instance;

	std::vector< IKeyCallbackHandler * >	m_keyCallbackHandlers;
};
