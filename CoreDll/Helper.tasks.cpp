﻿/*
*    DDOA-DOAXVV-OPEN-ASSISTANT
*
*     Copyright 2023-2024  Tyler Parret True
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
#include "Helper.h"

#include "Settings.h"
#include "AskedForStop.h"

namespace ohms {

bool Helper::Task_StartUp() {
	system("start steam://rungameid/958260");

	return true;
}

bool Helper::Task_Challenge() {
	switch (m_handler->SetForGame()) {
	case WndHandler::SetReturnValue::WndNotFound:
		PushMsg(HelperReturnMessage::LOG_WorkError_NoWnd);
		throw 0;
		break;
	case WndHandler::SetReturnValue::CaptureFailed:
		PushMsg(HelperReturnMessage::LOG_WorkError_FailedCapture);
		throw 0;
		break;
	}

	std::unique_ptr<MatchTemplate> 
		temp_default   = std::make_unique<MatchTemplate>(m_templateList.at("default")),
		temp_lastFight = std::make_unique<MatchTemplate>(m_templateList.at("lastFight")),
		temp_newFight  = std::make_unique<MatchTemplate>(m_templateList.at("newFight")),
		temp_start     = std::make_unique<MatchTemplate>(m_templateList.at("start")),
		temp_loading   = std::make_unique<MatchTemplate>(m_templateList.at("loading")),
		temp_fp        = std::make_unique<MatchTemplate>(m_templateList.at("fp")),
		temp_result    = std::make_unique<MatchTemplate>(m_templateList.at("result")),
		temp_add       = std::make_unique<MatchTemplate>(m_templateList.at("add"));
	temp_default->LoadMat((m_assets / "default.png").string());
	temp_lastFight->LoadMat((m_assets / "lastFight.png").string());
	temp_newFight->LoadMat((m_assets / "newFight.png").string());
	temp_start->LoadMat((m_assets / "start.png").string());
	temp_loading->LoadMat((m_assets / "loading.png").string());
	temp_fp->LoadMat((m_assets / "fp.png").string());
	temp_result->LoadMat((m_assets / "result.png").string());
	temp_add->LoadMat((m_assets / "add.png").string());

	PushMsg(HelperReturnMessage::LOG_Challenge_Start);
	const bool forNew = Settings::g_set.ChaGame_ForNew; // 保存设置

	bool forAddThisTime = false;

	cv::Rect rect;
	cv::Point pt;

	unsigned long i = 0; // 次数

	try {
	begin_point:

		++i;
		if (i > 1)
			PushMsg(HelperReturnMessage::CMD_EmptyLine);
		PushMsgCode(HelperReturnMessage::LOG_Challenge_BeginNum, i);

		// 查找目标比赛按钮。
		if (-1 == m_handler->WaitFor(forNew ? *temp_newFight : *temp_lastFight))
			Step_TaskError(
				forNew ?
				HelperReturnMessage::STR_Task_Challenge_NoNew :
				HelperReturnMessage::STR_Task_Challenge_NoLast
			);


		// 点击比赛，直到进入编队画面（找到挑战按钮）。
		PushMsg(forAddThisTime ? HelperReturnMessage::LOG_Challenge_EnterAdd :
				(forNew ? HelperReturnMessage::LOG_Challenge_EnterNew : HelperReturnMessage::LOG_Challenge_EnterLast));
		rect = (forNew ? temp_newFight : temp_lastFight)->GetLastMatchRect();
		if (!Step_KeepClickingUntil({ 900, rect.y }, *temp_start))
			Step_TaskError(HelperReturnMessage::STR_Task_Challenge_NoEnter);

		forAddThisTime = false;

		// 查找“挑战”按钮。
		m_handler->WaitFor(*temp_start);
		rect = temp_start->GetLastMatchRect();
		PushMsg(HelperReturnMessage::LOG_Challenge_Play);
		{
			size_t tc = 0;
			while (!g_askedForStop) {
				// 点击挑战，直到进入加载画面。
				pt = { rect.x + 50, rect.y + 20 };
				if (Step_KeepClickingUntil(pt, *temp_loading, seconds(1.5f), seconds(0.3f)))
					break;
				// 瞟一眼是否是fp不足。
				if (0 == m_handler->WaitFor(*temp_fp, seconds(0.2f)))
					Step_TaskError(HelperReturnMessage::STR_Task_Challenge_LowFP);
				// 尝试次数超限则报错。
				if (++tc > 12)
					Step_TaskError(HelperReturnMessage::STR_Task_Challenge_NoStart);
			}
		}

		// 等待结束。
		PushMsg(HelperReturnMessage::LOG_Challenge_WaitForEnd);
		if (-1 == m_handler->WaitFor(*temp_result, seconds(5 * 60.0f)))
			Step_TaskError(HelperReturnMessage::STR_Task_Challenge_TimeOut);

		// 点击，直到进入加载画面。
		PushMsg(HelperReturnMessage::LOG_Challenge_End);
		rect = temp_result->GetLastMatchRect();
		pt = { rect.x + 100, rect.y };
		if (!Step_KeepClickingUntil(pt, *temp_loading, seconds(60.0f), seconds(0.1f)))
			Step_TaskError(HelperReturnMessage::STR_Task_Challenge_NoEnd);

		// 等待到挑战赛标签页出现。
		PushMsg(HelperReturnMessage::LOG_Challenge_Quiting);
		if (-1 == m_handler->WaitFor(*temp_default, seconds(60.0f)))
			Step_TaskError(HelperReturnMessage::STR_Task_Challenge_NoOver);
		PushMsg(HelperReturnMessage::LOG_Challenge_Over);

		// 检查是否有奖励挑战赛。
		if (Settings::g_set.ChaGame_CheckAddition) {
			if (0 == m_handler->WaitFor(*temp_add, seconds(2.0f))) {
				rect = temp_add->GetLastMatchRect();
				PushMsg(HelperReturnMessage::LOG_Challenge_FindAdd);
				if (Settings::g_set.ChaGame_EnterAddition) { // 进入奖励挑战赛。
					if (forNew) {
						pt.x = rect.x + 30;
						pt.y = rect.y + 50;
						if (Step_KeepClickingUntil(pt, *temp_newFight, seconds(10.0f), seconds(2.0f))) {
							PushMsg(HelperReturnMessage::LOG_Challenge_OpenedAdd);
							forAddThisTime = true;
						}
						else {
							Step_TaskError(HelperReturnMessage::STR_Task_Challenge_OpenAddFailed);
						}
						//Step_Click(pt);
					}
					else {
						Step_TaskError(HelperReturnMessage::STR_Task_Challenge_AddNotSup);
					}
				}
				else { // 回到“推荐”栏。
					if (0 == m_handler->WaitFor(*temp_default, seconds(5.0f))) {
						rect = temp_default->GetLastMatchRect();
						pt.x = rect.x + 40;
						pt.y = rect.y + 12;
						if (Step_KeepClickingUntilNo(pt, *temp_add, seconds(10.0f), seconds(0.5f))) {
							PushMsg(HelperReturnMessage::LOG_Challenge_IgnoredAdd);
						}
						else {
							Step_TaskError(HelperReturnMessage::STR_Task_Challenge_IgnoreAddFailed);
						}
						//Step_Click(pt);
					}
				}
			}
			else {
				PushMsg(HelperReturnMessage::LOG_Challenge_NotFindAdd);
			}
		}

		goto begin_point;
	}
	catch (int err) {
		if (err == 0) { // 返回false表示主动停止
			PushMsg(HelperReturnMessage::LOG_TaskStop);
			return false;
		}
		else { // 不是0就是真错误
			PushMsg(HelperReturnMessage::LOG_TaskError_Exception);
		}
	}
	catch (...) {
		PushMsg(HelperReturnMessage::LOG_TaskError_Exception);
	}
	PushMsg(HelperReturnMessage::LOG_Challenge_Exit);
	return true;
}

}
