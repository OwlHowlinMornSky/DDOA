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
#include "Task_Challenge.h"
#include "CoreLog.h"
#include "TaskExceptionCode.h"
#include "Task_Navigate.h"
#include "Sleep.h"

namespace ohms {

Task_Challenge::Task_Challenge() :
	r_helper(nullptr),
	r_handler(nullptr),
	m_set(Settings::Challenge::DEFAULT) {}

Task_Challenge::~Task_Challenge() {}

bool Task_Challenge::Run(Helper& h) {
	r_handler = WndHandler::Instance();
	r_helper = &h;

	bool taskReturnCode = true; // 若返回false表示终止后续任务。
	try {
		CoreLog() << "Task.Challenge: Start." << std::endl;
		h.GuiLogF(ReturnMsgEnum::ChaStart);

		const bool forNew = m_set.PlayNewMatchOrPrevious; // 保存设置

		switch (r_handler->SetForGame()) {
		case WndHandler::SetReturnValue::WndNotFound:
			CoreLog() << "Task.Challenge: Game Window Not Found." << std::endl;
			h.TaskError(ReturnMsgEnum::TaskErrNoWnd);
			break;
		case WndHandler::SetReturnValue::CaptureFailed:
			CoreLog() << "Task.Challenge: Game Window Cannot Capture." << std::endl;
			h.TaskError(ReturnMsgEnum::TaskErrNoCap);
			break;
		}

		std::unique_ptr<MatchTemplate>
			temp_chaBar = h.CreateTemplate("default"),
			temp_lastFight = h.CreateTemplate("lastFight"),
			temp_newFight = h.CreateTemplate("newFight"),
			temp_startGame = h.CreateTemplate("start"),
			temp_loading = h.CreateTemplate("loading"),
			temp_lowFp = h.CreateTemplate("fp"),
			temp_gameResult = h.CreateTemplate("result"),
			temp_awardCha = h.CreateTemplate("add");
		m_camFp = h.CreateTemplate("UseFp");
		m_camFpNo = h.CreateTemplate("cha/shotFpNo");
		m_camFpComf[0] = h.CreateTemplate("UseFpComf0");
		m_camFpComf[1] = h.CreateTemplate("UseFpComf1");

		cv::Point pt;
		unsigned long playCnt = 0; // 次数
		bool forAddThisTime = false; // 是否已经进入奖励挑战赛
		unsigned long playAwardCnt = 0; // 奖励挑战赛次数

		Task_Navigate::Instance()->ShakeCursor();

		while (true) {
			// 检查导航位置
			switch (Task_Navigate::Instance()->TryToDeterminePage()) {
			case NavigateEnum::Challenge:
				if (-1 == r_handler->WaitFor(*temp_chaBar)) {
					throw TaskExceptionCode::KnownErrorButNotCritical; // 不影响后续任务的错误。
				}
				r_handler->ClickAt(temp_chaBar->GetSpecialPointInResult(0));
				sleep(500_msec);
				break;
			case NavigateEnum::MatchConfirm:
				goto MatchComfirm;
				break;
			case NavigateEnum::MatchResult:
				goto MatchEnd;
				break;
			default:
				if (!Task_Navigate::Instance()->NavigateTo(NavigateEnum::Challenge)) {
					throw TaskExceptionCode::KnownErrorButNotCritical; // 不影响后续任务的错误。
				}
				sleep(2.0_sec);
				if (-1 == r_handler->WaitFor(*temp_chaBar)) {
					throw TaskExceptionCode::KnownErrorButNotCritical; // 不影响后续任务的错误。
				}
				r_handler->ClickAt(temp_chaBar->GetSpecialPointInResult(0));
				sleep(500_msec);
				break;
			}

				// 查找目标比赛按钮。
			if (-1 == r_handler->WaitFor(forNew ? *temp_newFight : *temp_lastFight))
				h.TaskError(
					forNew ?
					ReturnMsgEnum::TaskErrChaNoNew :
					ReturnMsgEnum::TaskErrChaNoPri
				);

			// 点击比赛，直到进入编队画面（找到挑战按钮）。
			CoreLog() << "Task.Challenge: Enter Game <" << (forAddThisTime ? "Award" : (forNew ? "New" : "Last")) << ">." << std::endl;
			pt = { 900, (forNew ? temp_newFight : temp_lastFight)->GetLastMatchRect().y };
			if (!r_handler->KeepClickingUntil(pt, *temp_startGame))
				h.TaskError(ReturnMsgEnum::TaskErrChaNoEnter);

			forAddThisTime = false;

		MatchComfirm:

			// 查找“挑战”按钮。
			r_handler->WaitFor(*temp_startGame);
			CoreLog() << "Task.Challenge: Play Game." << std::endl;
			for (int i = 0, n = 10; i < n; ++i) {
				pt = temp_startGame->GetSpecialPointInResult(0);
				r_handler->ClickAt(pt);
				switch (r_handler->WaitForMultiple({ temp_loading.get(), temp_lowFp.get() }, seconds(2.0f))) {
				default:
				case -1:
					if (i == n - 1)
						h.TaskError(ReturnMsgEnum::TaskErrChaNoStart);
					break;
				case 0:
					i = n;
					break;
				case 1:
					if (m_set.AutoUseCamFP) { // 使用cam补充fp
						switch (r_handler->WaitForMultiple({ m_camFp.get(), m_camFpNo.get() }, 2.0_sec)) {
						case 0:
						{
							pt = m_camFp->GetSpecialPointInResult(0);
							if (r_handler->KeepClickingUntil(pt, *(m_camFpComf[0]))) {
								pt = m_camFpComf[0]->GetSpecialPointInResult(0);
								if (r_handler->KeepClickingUntil(pt, *(m_camFpComf[1]))) {
									pt = m_camFpComf[1]->GetSpecialPointInResult(0);
									if (r_handler->KeepClickingUntil(pt, *temp_startGame)) {
										break;
									}
								}
							}
							break;
						}
						default:
							throw TaskExceptionCode::TaskComplete;
						}
					}
					else {
						throw TaskExceptionCode::TaskComplete;
					}
					break;
				}
			}

			if (forAddThisTime) {
				++playAwardCnt;
				h.GuiLogF_I(ReturnMsgEnum::ChaBeginAdd_I, playAwardCnt);
			}
			else {
				++playCnt;
				h.GuiLogF_I(ReturnMsgEnum::ChaBegin_I, playCnt);
			}

		MatchEnd:

			// 等待结束。
			CoreLog() << "Task.Challenge: In Game, Waitting for End." << std::endl;
			if (-1 == r_handler->WaitFor(*temp_gameResult, seconds(5 * 60.0f)))
				h.TaskError(ReturnMsgEnum::TaskErrChaTimeOut);

			// 点击，直到进入加载画面。
			CoreLog() << "Task.Challenge: Game End, Trying to Exit." << std::endl;
			pt = temp_gameResult->GetSpecialPointInResult(0);
			if (!r_handler->KeepClickingUntil(pt, *temp_loading, seconds(60.0f), seconds(0.1f)))
				h.TaskError(ReturnMsgEnum::TaskErrChaNoEnd);

			// 等待到挑战赛标签页出现。
			CoreLog() << "Task.Challenge: Game Exit, Waitting for Challenge Page." << std::endl;
			if (-1 == r_handler->WaitFor(*temp_chaBar, seconds(60.0f)))
				h.TaskError(ReturnMsgEnum::TaskErrChaNoOver);
			h.GuiLogF(ReturnMsgEnum::ChaOver);

			// 检查是否有奖励挑战赛。
			if (m_set.CheckAddition) {
				if (0 == r_handler->WaitFor(*temp_awardCha, seconds(2.0f))) {
					h.GuiLogF(ReturnMsgEnum::ChaFindAdd);
					if (m_set.EnterAddition) { // 进入奖励挑战赛。
						if (forNew) {
							pt = temp_awardCha->GetSpecialPointInResult(0);
							if (r_handler->KeepClickingUntil(pt, *temp_newFight, seconds(10.0f), seconds(2.0f))) {
								h.GuiLogF(ReturnMsgEnum::ChaOpenedAdd);
								forAddThisTime = true;
								playAwardCnt = 0;
							}
							else {
								h.TaskError(ReturnMsgEnum::TaskErrChaOpenAddFailed);
							}
						}
						else {
							h.TaskError(ReturnMsgEnum::TaskErrChaAddNotSup);
						}
					}
					else { // 回到“推荐”栏。
						if (0 == r_handler->WaitFor(*temp_chaBar, seconds(5.0f))) {
							pt = temp_chaBar->GetSpecialPointInResult(0);
							if (r_handler->KeepClickingUntilNo(pt, *temp_awardCha, seconds(10.0f), seconds(0.5f))) {
								h.GuiLogF(ReturnMsgEnum::ChaIgnoredAdd);
							}
							else {
								h.TaskError(ReturnMsgEnum::TaskErrChaIgnoreAddFailed);
							}
						}
					}
				}
				else {
					h.GuiLogF(ReturnMsgEnum::ChaNotFindAdd);
				}
			}
		}
	}
	catch (int err) {
		switch (err) {
		case TaskExceptionCode::UserStop:
			h.GuiLogF(ReturnMsgEnum::TaskStop);
			taskReturnCode = false;
			break;
		case TaskExceptionCode::TaskComplete:
			h.GuiLogF(ReturnMsgEnum::TaskComplete);
			break;
		case TaskExceptionCode::KnownErrorButNotCritical:
			break;
		case TaskExceptionCode::KnownErrorCritical:
			taskReturnCode = false;
			break;
		default:
			h.GuiLogF(ReturnMsgEnum::TaskException);
			taskReturnCode = false;
		}
	}
	catch (...) {
		h.GuiLogF(ReturnMsgEnum::TaskException);
		taskReturnCode = false;
	}
	CoreLog() << "Task.Challenge: Exit." << std::endl;
	h.GuiLogF(ReturnMsgEnum::ChaExit);
	return taskReturnCode;
}

}
