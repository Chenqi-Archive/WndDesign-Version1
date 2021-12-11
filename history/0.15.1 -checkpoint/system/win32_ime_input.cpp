#include "win32_ime_input.h"


#pragma comment(lib, "imm32.lib")


BEGIN_NAMESPACE(WndDesign)

BEGIN_NAMESPACE(Anonymous)

// Determines whether or not the given attribute represents a target(a selection).
inline bool IsTargetAttribute(char attribute) {
	return (attribute == ATTR_TARGET_CONVERTED || attribute == ATTR_TARGET_NOTCONVERTED);
}

// Helper function for ImeInput::GetCompositionInfo() method, to get the target
// range that's selected by the user in the current composition string.
void GetCompositionTargetRange(HIMC imm_context, uint& target_start, uint& target_end) {
	uint attribute_size = ::ImmGetCompositionString(imm_context, GCS_COMPATTR, NULL, 0);
	if (attribute_size == 0) { return; }

	uint start = 0, end = 0;
	vector<char> attribute_data(attribute_size);
	::ImmGetCompositionString(imm_context, GCS_COMPATTR, attribute_data.data(), attribute_size);

	for (start = 0; start < attribute_size; ++start) {
		if (IsTargetAttribute(attribute_data[start])) { break; }
	}
	for (end = start; end < attribute_size; ++end) {
		if (!IsTargetAttribute(attribute_data[end])) { break; }
	}
	if (start == attribute_size) {
		// This composition clause does not contain any target clauses,
		// i.e. this clauses is an input clause.
		// We treat the whole composition as a target clause.
		start = 0;
		end = attribute_size;
	}
	target_start = start;
	target_end = end;
}

// Helper function for ImeInput::GetCompositionInfo() method, to get underlines
// information of the current composition string.
void GetCompositionUnderlines(HIMC imm_context, uint target_start, uint target_end, vector<UnderlineStyle>& underlines) {
	uint clause_size = ::ImmGetCompositionString(imm_context, GCS_COMPCLAUSE, NULL, 0);
	uint clause_length = clause_size / sizeof(uint);
	if (clause_length == 0) { return; }

	std::vector<uint> clause_data(clause_length);
	::ImmGetCompositionString(imm_context, GCS_COMPCLAUSE, clause_data.data(), clause_size);
	for (uint i = 0; i < clause_length - 1; ++i) {
		UnderlineStyle underline;
		underline.begin = clause_data[i];
		underline.end = clause_data[i + 1];
		underline.color = ColorSet::Black;
		// Use thick underline for the target clause.
		if (underline.begin >= target_start && underline.end <= target_end) {
			underline.thick = true;
		} else {
			underline.thick = false;
		}
		underlines.push_back(underline);
	}
}

END_NAMESPACE(Anonymous)


ImeInput& ImeInput::Get() {
	static ImeInput ime;
	return ime;
}

void ImeInput::EnableIME(HWND window_handle) {
	// Load the default IME context.
	// NOTE(hbono)
	//   IMM ignores this call if the IME context is loaded. Therefore, we do
	//   not have to check whether or not the IME context is loaded.
	::ImmAssociateContextEx(window_handle, NULL, IACE_DEFAULT);
}

void ImeInput::DisableIME(HWND window_handle) {
	// A renderer process have moved its input focus to a password input
	// when there is an ongoing composition, e.g. a user has clicked a
	// mouse button and selected a password input while composing a text.
	// For this case, we have to complete the ongoing composition and
	// clean up the resources attached to this object BEFORE DISABLING THE IME.
	CleanupComposition(window_handle);
	::ImmAssociateContextEx(window_handle, NULL, 0);
}

void ImeInput::CancelIME(HWND window_handle) {
	if (_is_composing) {
		HIMC imm_context = ::ImmGetContext(window_handle);
		if (imm_context) {
			::ImmNotifyIME(imm_context, NI_COMPOSITIONSTR, CPS_CANCEL, 0);
			::ImmReleaseContext(window_handle, imm_context);
		}
		ResetComposition();
	}
}

bool ImeInput::UpdateInputLanguage() {
	// Retrieve the current keyboard layout from Windows and determine whether
	// or not the current input context has IMEs.
	HKL keyboard_layout = ::GetKeyboardLayout(0);
	_language = static_cast<LANGID>(reinterpret_cast<size_t>(keyboard_layout));
	_has_ime = (::ImmIsIME(keyboard_layout) == TRUE) ? true : false;
	return _has_ime;
}


void ImeInput::CreateImeWindow(HWND window_handle) {
	// Create a temporary system caret for Chinese IMEs and use it during this input context.
	if (PRIMARYLANGID(_language) == LANG_CHINESE || PRIMARYLANGID(_language) == LANG_JAPANESE) {
		if (!_has_system_caret) {
			if (::CreateCaret(window_handle, NULL, 1, 1)) {
				_has_system_caret = true;
			}
		}
	}
	// Restore the positions of the IME windows.
	UpdateImeWindow(window_handle);
}

void ImeInput::SetImeWindowStyle(HWND window_handle, UINT message, WPARAM wparam, LPARAM lparam) {
	// To prevent the IMM (Input Method Manager) from displaying the IME
	// composition window, Update the styles of the IME windows and EXPLICITLY
	// call ::DefWindowProc() here.
	lparam &= ~ISC_SHOWUICOMPOSITIONWINDOW;
	::DefWindowProc(window_handle, message, wparam, lparam);
}

void ImeInput::DestroyImeWindow(HWND window_handle) {
	// Destroy the system caret if we have created for this IME input context.
	if (_has_system_caret) {
		::DestroyCaret();
		_has_system_caret = false;
	}
}

void ImeInput::MoveImeWindow(HIMC imm_context) {
	int x = _caret_rect.point.x;
	int y = _caret_rect.point.y;
	const int kCaretMargin = 1;

	CANDIDATEFORM candidate_position = { 0, CFS_CANDIDATEPOS, {x, y}, {0, 0, 0, 0} };
	::ImmSetCandidateWindow(imm_context, &candidate_position);

	if (_has_system_caret) {
		if (PRIMARYLANGID(_language) == LANG_JAPANESE) {
			::SetCaretPos(x, y + _caret_rect.size.height);
		} else{
			::SetCaretPos(x, y);
		}
	}

	if (PRIMARYLANGID(_language) == LANG_KOREAN) {
		y += kCaretMargin;
	}

	CANDIDATEFORM exclude_rectangle = { 0, CFS_EXCLUDE, {x, y},
		{x, y, x + static_cast<int>(_caret_rect.size.width), y + static_cast<int>(_caret_rect.size.height)} };
	::ImmSetCandidateWindow(imm_context, &exclude_rectangle);
}

void ImeInput::UpdateImeWindow(HWND window_handle) {
	// Just move the IME window attached to the given window.
	if (_caret_rect.point.x >= 0 && _caret_rect.point.y >= 0) {
		HIMC imm_context = ::ImmGetContext(window_handle);
		if (imm_context) {
			MoveImeWindow(imm_context);
			::ImmReleaseContext(window_handle, imm_context);
		}
	}
}

void ImeInput::UpdateCaretRect(HWND window_handle, Rect caret_rect) {
	// Save the caret position, and Update the position of the IME window.
	// This update is used for moving an IME window when a renderer process
	// resize/moves the input caret.
	if (_caret_rect == caret_rect) { return; }

	_caret_rect = caret_rect;
	// Move the IME windows.
	HIMC imm_context = ::ImmGetContext(window_handle);
	if (imm_context) {
		MoveImeWindow(imm_context);
		::ImmReleaseContext(window_handle, imm_context);
	}
}

void ImeInput::CleanupComposition(HWND window_handle) {
	// Notify the IMM attached to the given window to complete the ongoing
	// composition, (this case happens when the given window is de-activated
	// while composing a text and re-activated), and reset the omposition status.
	if (_is_composing) {
		HIMC imm_context = ::ImmGetContext(window_handle);
		if (imm_context) {
			::ImmNotifyIME(imm_context, NI_COMPOSITIONSTR, CPS_COMPLETE, 0);
			::ImmReleaseContext(window_handle, imm_context);
		}
		ResetComposition();
	}
}

void ImeInput::ResetComposition() {
	_is_composing = false;	// Currently, just reset the composition status.
}

void ImeInput::CompleteComposition(HIMC imm_context) {
	// We have to confirm there is an ongoing composition before completing it.
	// This is for preventing some IMEs from getting confused while completing an
	// ongoing composition even if they do not have any ongoing compositions.)
	if (_is_composing) {
		::ImmNotifyIME(imm_context, NI_COMPOSITIONSTR, CPS_COMPLETE, 0);
		ResetComposition();
	}
}

void ImeInput::GetCompositionInfo(HIMC imm_context, LPARAM lparam, ImeComposition& composition) {
	// We only care about GCS_COMPATTR, GCS_COMPCLAUSE and GCS_CURSORPOS, and
	// convert them into underlines and selection range respectively.
	composition.underlines.clear();
	uint length = static_cast<uint>(composition.string.length());

	// Retrieve the selection range information. If CS_NOMOVECARET is specified,
	// that means the cursor should not be moved, then we just place the caret at
	// the beginning of the composition string. Otherwise we should honour the
	// GCS_CURSORPOS value if it's available.
	if (lparam & CS_NOMOVECARET) {
		composition.begin = composition.end = 0;
	} else if (lparam & GCS_CURSORPOS) {
		composition.begin = composition.end = ::ImmGetCompositionString(imm_context, GCS_CURSORPOS, NULL, 0);
	} else {
		composition.begin = composition.end = length;
	}

	// Find out the range selected by the user.
	uint target_start = 0, target_end = 0;
	if (lparam & GCS_COMPATTR) {
		GetCompositionTargetRange(imm_context, target_start, target_end);
	}
	// Retrieve the clause segmentations and convert them to underlines.
	if (lparam & GCS_COMPCLAUSE) {
		GetCompositionUnderlines(imm_context, target_start, target_end, composition.underlines);
	}

	// Set default underlines in case there is no clause information.
	if (composition.underlines.size() == 0) {
		UnderlineStyle underline;
		underline.color = ColorSet::Black;
		if (target_start > 0) {
			underline.begin = 0;
			underline.end = target_start;
			underline.thick = false;
			composition.underlines.push_back(underline);
		}
		if (target_end > target_start) {
			underline.begin = target_start;
			underline.end = target_end;
			underline.thick = true;
			composition.underlines.push_back(underline);
		}
		if (target_end < length) {
			underline.begin = target_end;
			underline.end = length;
			underline.thick = false;
			composition.underlines.push_back(underline);
		}
	}
}

bool ImeInput::GetString(HIMC imm_context, WPARAM lparam, uint type, ImeComposition& composition) {
	bool result = false;
	if (lparam & type) {
		uint string_size = ::ImmGetCompositionString(imm_context, type, NULL, 0);
		if (string_size > 0) {
			uint string_length = string_size / sizeof(wchar_t);
			composition.string.resize(string_length);
			// Fill the given ImeComposition object.
			::ImmGetCompositionString(imm_context, type, const_cast<wchar_t*>(composition.string.data()), string_size);
			composition.type = static_cast<decltype(composition.type)>(type);
			result = true;
		}
	}
	return result;
}

bool ImeInput::UpdateResult(HWND window_handle, LPARAM lparam) {
	bool result = false;
	HIMC imm_context = ::ImmGetContext(window_handle);
	if (imm_context) {
		// Copy the result string to the ImeComposition object.
		result = GetString(imm_context, lparam, GCS_RESULTSTR, _result);

		// Reset all the other parameters because a result string does not
		// have composition attributes.
		::ImmReleaseContext(window_handle, imm_context);
	}
	return result;
}

bool ImeInput::UpdateComposition(HWND window_handle, LPARAM lparam) {
	bool result = false;
	HIMC imm_context = ::ImmGetContext(window_handle);
	if (imm_context) {
		// Copy the composition string to the ImeComposition object.
		result = GetString(imm_context, lparam, GCS_COMPSTR, _composition);

		// This is a dirty workaround for facebook. Facebook deletes the placeholder
		// character (U+3000) used by Traditional-Chinese IMEs at the beginning of
		// composition text. This prevents WebKit from replacing this placeholder
		// character with a Traditional-Chinese character, i.e. we cannot input any
		// characters in a comment box of facebook with Traditional-Chinese IMEs.
		// As a workaround, we replace U+3000 at the beginning of composition text
		// with U+FF3F, a placeholder character used by Japanese IMEs.
		if (_language == MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_TRADITIONAL) && _composition.string[0] == 0x3000) {
			_composition.string[0] = 0xFF3F;
		}

		// Retrieve the composition underlines and selection range information.
		GetCompositionInfo(imm_context, lparam, _composition);

		// Mark that there is an ongoing composition.
		_is_composing = true;
		::ImmReleaseContext(window_handle, imm_context);
	}
	return result;
}


END_NAMESPACE(WndDesign)