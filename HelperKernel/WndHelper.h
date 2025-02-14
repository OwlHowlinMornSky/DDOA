/*
*    DDOA-DOAXVV-OPEN-ASSISTANT
*
*     Copyright 2023-2025  Tyler Parret True
*
*    Licensed under the Apache License, Version 2.0 (the "License");
*    you may not use this file except in compliance with the License.
*    You may obtain a copy of the License at
*
*        http://www.apache.org/licenses/LICENSE-2.0
*
*    Unless required by applicable law or agreed to in writing, software
*    distributed under the License is distributed on an "AS IS" BASIS,
*    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*    See the License for the specific language governing permissions and
*    limitations under the License.
*
* @Authors
*    Tyler Parret True <mysteryworldgod@outlook.com><https://github.com/OwlHowlinMornSky>
*/
#pragma once
using namespace System;

namespace HelperKernel {

public ref class WndHelper {
public:
	static bool WindowExist(System::String^ cls, System::String^ title);

	static void ShellStart(System::String^ path);

	/**
	 * @brief SetThreadExecutionState
	 * @param continuous  ES_CONTINUOUS
	 * @param system      ES_SYSTEM_REQUIRED
	 * @param display     ES_DISPLAY_REQUIRED
	 * @param awaymode    ES_AWAYMODE_REQUIRED
	 */
	static void SetState(
		int continuous,
		int system,
		int display,
		int awaymode
	);
};

}
