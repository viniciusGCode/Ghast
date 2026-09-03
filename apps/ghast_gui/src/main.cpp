#include <ghast/windows/process_snapshot.hpp>

#include <Windows.h>
#include <CommCtrl.h>

#if defined(_MSC_VER)
#pragma comment(linker, "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#include <cstddef>
#include <cwctype>
#include <optional>
#include <sstream>
#include <string>
#include <system_error>

// buttons = 1000
constexpr int refresh_button_id = 1001;
constexpr int read_button_id = 1002;

//inputs = 2000
constexpr int filter_input_id = 2001;
constexpr int pid_input_id = 2002;
constexpr int address_input_id = 2003;
constexpr int size_input_id = 2004;

// output = 3000
constexpr int output_text_id = 3001;

HWND output_text = nullptr;
HWND filter_input = nullptr;
HFONT ui_font = nullptr;
HFONT monospace_font = nullptr;
HBRUSH window_background = nullptr;
HBRUSH output_background = nullptr;

HWND create_label(
  HWND parent,
  HINSTANCE instance,
  const wchar_t* text,
  int x,
  int y,
  int width,
  int height)
{
  return CreateWindowExW(
      0,
      L"STATIC",
      text,
      WS_CHILD | WS_VISIBLE,
      x,
      y,
      width,
      height,
      parent,
      nullptr,
      instance,
      nullptr
  );
}

HWND create_button(
  HWND parent,
  HINSTANCE instance,
  int id,
  const wchar_t* text,
  int x,
  int y,
  int width,
  int height)
{
  return CreateWindowExW(
      0,
      L"BUTTON",
      text,
      WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
      x,
      y,
      width,
      height,
      parent,
      reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),
      instance,
      nullptr
  );
}

HWND create_input(
  HWND parent,
  HINSTANCE instance,
  int id,
  int x,
  int y,
  int width,
  int height)
{
  return CreateWindowExW(
      WS_EX_CLIENTEDGE,
      L"EDIT",
      L"",
      WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
      x,
      y,
      width,
      height,
      parent,
      reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),
      instance,
      nullptr
  );
}

HWND create_output_text(
  HWND parent,
  HINSTANCE instance,
  int id,
  int x,
  int y,
  int width,
  int height)
{
  return CreateWindowExW(
      WS_EX_CLIENTEDGE,
      L"EDIT",
      L"",
      WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
      x,
      y,
      width,
      height,
      parent,
      reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),
      instance,
      nullptr
  );
}

HFONT create_font(const wchar_t* family, int point_size)
{
  const int dpi = GetDpiForSystem();
  const int height = -MulDiv(point_size, dpi, 72);

  return CreateFontW(
      height,
      0,
      0,
      0,
      FW_NORMAL,
      FALSE,
      FALSE,
      FALSE,
      DEFAULT_CHARSET,
      OUT_DEFAULT_PRECIS,
      CLIP_DEFAULT_PRECIS,
      CLEARTYPE_QUALITY,
      DEFAULT_PITCH | FF_DONTCARE,
      family);
}

void set_control_font(HWND control, HFONT font)
{
  if (control == nullptr || font == nullptr) {
    return;
  }

  SendMessageW(
      control,
      WM_SETFONT,
      reinterpret_cast<WPARAM>(font),
      TRUE);
}

bool initialize_theme()
{
  INITCOMMONCONTROLSEX common_controls{};
  common_controls.dwSize = sizeof(common_controls);
  common_controls.dwICC = ICC_STANDARD_CLASSES;

  const bool common_controls_initialized =
      InitCommonControlsEx(&common_controls) != FALSE;

  ui_font = create_font(L"Segoe UI", 10);
  monospace_font = create_font(L"Consolas", 10);
  window_background = CreateSolidBrush(RGB(245, 247, 250));
  output_background = CreateSolidBrush(RGB(255, 255, 255));

  return common_controls_initialized
      && ui_font != nullptr
      && monospace_font != nullptr
      && window_background != nullptr
      && output_background != nullptr;
}

void destroy_theme()
{
  if (ui_font != nullptr) {
    DeleteObject(ui_font);
    ui_font = nullptr;
  }

  if (monospace_font != nullptr) {
    DeleteObject(monospace_font);
    monospace_font = nullptr;
  }

  if (window_background != nullptr) {
    DeleteObject(window_background);
    window_background = nullptr;
  }

  if (output_background != nullptr) {
    DeleteObject(output_background);
    output_background = nullptr;
  }
}

void create_main_controls(HWND window, HINSTANCE instance)
{
  const HWND filter_label = create_label(window, instance, L"Filter", 16, 16, 40, 24);
  filter_input = create_input(window, instance, filter_input_id, 58, 14, 240, 26);
  const HWND refresh_button = create_button(window, instance, refresh_button_id, L"Refresh", 314, 13, 100, 30);

  const HWND pid_label = create_label(window, instance, L"PID", 16, 58, 40, 24);
  const HWND pid_input = create_input(window, instance, pid_input_id, 56, 56, 120, 26);

  const HWND address_label = create_label(window, instance, L"Address", 192, 58, 56, 24);
  const HWND address_input = create_input(window, instance, address_input_id, 254, 56, 180, 26);

  const HWND size_label = create_label(window, instance, L"Size", 450, 58, 32, 24);
  const HWND size_input = create_input(window, instance, size_input_id, 488, 56, 80, 26);

  const HWND read_button = create_button(window, instance, read_button_id, L"Read", 584, 55, 80, 30);

  output_text = create_output_text(window, instance, output_text_id, 16, 104, 850, 432);

  set_control_font(filter_label, ui_font);
  set_control_font(filter_input, ui_font);
  set_control_font(refresh_button, ui_font);

  set_control_font(pid_label, ui_font);
  set_control_font(pid_input, ui_font);

  set_control_font(address_label, ui_font);
  set_control_font(address_input, ui_font);

  set_control_font(size_label, ui_font);
  set_control_font(size_input, ui_font);

  set_control_font(read_button, ui_font);
  set_control_font(output_text, monospace_font);
}

void set_output_text(const wchar_t* text)
{
  if (output_text == nullptr) {
    return;
  }

  SetWindowTextW(output_text, text);
}

std::wstring get_window_text(HWND window)
{
  const int length = GetWindowTextLengthW(window);

  std::wstring text(static_cast<std::size_t>(length + 1), L'\0');

  const int copied = GetWindowTextW(window, text.data(), length + 1);

  text.resize(static_cast<std::size_t>(copied));

  return text;
}

std::wstring to_lowercase(std::wstring value)
{
  for (auto& character : value) {
    character = static_cast<wchar_t>(std::towlower(character));
  }

  return value;
}

bool matches_filter(
  const ghast::windows::ProcessSummary& process,
  const std::optional<std::wstring>& filter)
{
  if (!filter.has_value()) {
    return true;
  }

  const auto executable_name = to_lowercase(process.executable_name);

  return executable_name.find(*filter) != std::wstring::npos;
}

std::wstring build_process_list_text(const std::optional<std::wstring>& filter)
{
  const auto processes = ghast::windows::enumerate_processes();

  std::wstringstream output;

  for (const auto& process : processes) {
    if (!matches_filter(process, filter)) {
      continue;
    }
    output << L"PID: " << process.process_id
           << L" | " << process.executable_name;
    
    if (process.executable_path.has_value()) {
      output << L" | " << *process.executable_path;
    }
    output << L"\r\n";
  }

  return output.str();
}

void refresh_process_list()
{
  try {
    const auto raw_filter = to_lowercase(get_window_text(filter_input));

    const std::optional<std::wstring> filter =
      raw_filter.empty()
        ? std::nullopt
        : std::optional<std::wstring>{raw_filter};

    const auto text = build_process_list_text(filter);

    set_output_text(text.c_str());
  } catch (const std::system_error& error) {
    std::wstringstream output;
    output << L"Windows API error: " << error.code().value() << L"\r\n";

    set_output_text(output.str().c_str());
  }
}

LRESULT CALLBACK window_proc(
    HWND window,
    UINT message,
    WPARAM wparam,
    LPARAM lparam)
{

  if (message == WM_CTLCOLORSTATIC) {
    const HWND control = reinterpret_cast<HWND>(lparam);
    HDC device_context = reinterpret_cast<HDC>(wparam);

    if (control == output_text) {
      if (output_background == nullptr) {
        return DefWindowProcW(window, message, wparam, lparam);
      }

      SetTextColor(device_context, RGB(20, 24, 28));
      SetBkColor(device_context, RGB(255, 255, 255));

      return reinterpret_cast<LRESULT>(output_background);
    }

    if (window_background == nullptr) {
      return DefWindowProcW(window, message, wparam, lparam);
    }

    SetTextColor(device_context, RGB(32, 36, 40));
    SetBkMode(device_context, TRANSPARENT);

    return reinterpret_cast<LRESULT>(window_background);
  }

  if (message == WM_ERASEBKGND) {
    if (window_background == nullptr) {
      return DefWindowProcW(window, message, wparam, lparam);
    }
    HDC device_context = reinterpret_cast<HDC>(wparam);

    RECT client_rect{};
    GetClientRect(window, &client_rect);

    FillRect(device_context, &client_rect, window_background);

    return 1;
  }

  if(message == WM_COMMAND) {
    const int control_id = LOWORD(wparam);

    if(control_id == refresh_button_id) {
      refresh_process_list();
      return 0;
    }

    if(control_id == read_button_id) {
      set_output_text(L"Read clicked\r\n");
      return 0;
    }
  }

  if(message == WM_DESTROY) {
    destroy_theme();
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

  initialize_theme();

  const wchar_t window_class_name[] = L"GhastGuiWindow";

  WNDCLASSW window_class{};

  window_class.lpfnWndProc = window_proc;
  window_class.hInstance = instance;
  window_class.lpszClassName = window_class_name;

  if (window_background != nullptr) {
    window_class.hbrBackground = window_background;
  }

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

  create_main_controls(window, instance);

  ShowWindow(window, show_command);
  UpdateWindow(window);

  MSG message {};

  while (GetMessageW(&message, nullptr, 0, 0) > 0) {
      TranslateMessage(&message);
      DispatchMessageW(&message);
  }

  return static_cast<int>(message.wParam);
}
