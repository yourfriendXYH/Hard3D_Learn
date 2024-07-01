#pragma once

//¥ÌŒÛ“Ï≥£∞Ô÷˙∫Í
#define XYHWND_EXCEPT(hr) Window::HrException(__LINE__, __FILE__, hr)
#define XYHWND_LAST_EXCEPT() Window::HrException(__LINE__, __FILE__, GetLastError())
#define XYHWND_NOGFX_EXCEPT() Window::NoGfxException(__LINE__, __FILE__)