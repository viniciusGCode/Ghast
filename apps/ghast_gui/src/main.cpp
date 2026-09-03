#include <Windows.h>

LRESULT CALLBACK window_proc(
    HWND window,
    UINT message,
    WPARAM wparam,
    LPARAM lparam)
{
  if (message == WM_DESTROY) {
    PostQuitMessage(0);
    return 0;
  }

  return DefWindowProcW(window, message, wparam, lparam);
}

int WINAPI wWinMain(
    HINSTANCE instance,
    HINSTANCE previous_instance,
    PWSTR command_line,
    int show_command)
{
  (void)previous_instance;
  (void)command_line;

  const wchar_t window_class_name[] = L"GhastGuiWindow";

  WNDCLASSW window_class{};

  window_class.lpfnWndProc = window_proc;
  window_class.hInstance = instance;
  window_class.lpszClassName = window_class_name;

  if(RegisterClassW(&window_class) == 0) {
      return 1;
  }

  HWND window = CreateWindowExW(
      0,
      window_class_name,
      L"Ghast",
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      900,
      600,
      nullptr,
      nullptr,
      instance,
      nullptr);

  if(window == nullptr) {
      return 1;
  }

  ShowWindow(window, show_command);
  UpdateWindow(window);

  MSG message {};

  while (GetMessageW(&message, nullptr, 0, 0) > 0) {
      TranslateMessage(&message);
      DispatchMessageW(&message);
  }

  return static_cast<int>(message.wParam);
}