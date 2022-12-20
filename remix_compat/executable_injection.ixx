/// Utility module, with helpers for patching game memory
module;
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <wil/result.h>
export module executable_injection;
#pragma warning(suppress : 5050)
import std.core;

/// X86 OpCodes
constexpr uint8_t x86_JMP_OPCODE = 0xE9;
constexpr uint8_t x86_CALL_OPCODE = 0xE8;

export template <typename Data, size_t size = sizeof(Data)>
void Patch(std::intptr_t address, const Data *data) {
  auto adr_ptr = reinterpret_cast<void *>(address);

  DWORD old_protect;
  FAIL_FAST_IF_WIN32_BOOL_FALSE(
      VirtualProtect(adr_ptr, size, PAGE_EXECUTE_READWRITE, &old_protect));
  memcpy(adr_ptr, data, size);
  DWORD tmp;
  FAIL_FAST_IF_WIN32_BOOL_FALSE(VirtualProtect(adr_ptr, size, old_protect, &tmp));
}

export void RedirectJump(std::intptr_t address, const void *func) {
  auto func_iptr = reinterpret_cast<std::intptr_t>(func);
  std::intptr_t jump_offset =
      func_iptr - (address + sizeof(x86_JMP_OPCODE) + sizeof(func));

  Patch(address, &x86_JMP_OPCODE);
  Patch((address + sizeof(x86_JMP_OPCODE)), &jump_offset);
}

export void RedirectCall(std::intptr_t address, void *func) {
  auto func_iptr = reinterpret_cast<std::intptr_t>(func);
  std::intptr_t jump_offset =
      func_iptr - (address + sizeof(x86_CALL_OPCODE) + sizeof(func));
  Patch(address, &x86_CALL_OPCODE);
  Patch((address + sizeof(x86_CALL_OPCODE)), &jump_offset);
}