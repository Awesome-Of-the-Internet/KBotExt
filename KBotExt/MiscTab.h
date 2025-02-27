#pragma once

#include <filesystem>

#include "Definitions.h"
#include "Includes.h"
#include "HTTP.h"
#include "Auth.h"
#include "Misc.h"

class MiscTab
{
public:
	static void Render()
	{
		if (ImGui::BeginTabItem("Misc"))
		{
			static std::string result;

			ImGui::Columns(2, 0, false);

			if (ImGui::Button("Launch another client"))
			{
				ShellExecute(NULL, NULL, L"C:\\Riot Games\\Riot Client\\RiotClientServices.exe", L"--launch-product=league_of_legends --launch-patchline=live --allow-multiple-clients", NULL, SW_SHOWNORMAL);
			}

			ImGui::NextColumn();

			if (ImGui::Button("Launch legacy client"))
			{
				if (!std::filesystem::exists("C:/Riot Games/League of Legends/"))
				{
					//todo typing in lol path
					result = "League isnt installed in default path";
				}
				else
				{
					Misc::LaunchLegacyClient();
				}
			}

			ImGui::Columns(1);

			if (ImGui::Button("Restart UX"))
			{
				result = http->Request("POST", "https://127.0.0.1/riotclient/kill-and-restart-ux", "", auth->leagueHeader, "", "", auth->leaguePort);
				if (result.find("failed") != std::string::npos)
				{
					if (auth->GetLeagueClientInfo())
						result = "Rehooked to new league client";
				}
			}

			if (ImGui::Button("Close client"))
				result = http->Request("POST", "https://127.0.0.1/process-control/v1/process/quit", "", auth->leagueHeader, "", "", auth->leaguePort);

			ImGui::Separator();

			if (ImGui::Button("Free Tristana + Riot Girl skin"))
				result = http->Request("POST", "https://127.0.0.1/lol-login/v1/session/invoke?destination=inventoryService&method=giftFacebookFan&args=[]", "", auth->leagueHeader, "", "", auth->leaguePort);

			ImGui::SameLine();
			Misc::HelpMarker("Relog after pressing the button");

			if (ImGui::Button("Remove all friends"))
			{
				if (MessageBoxA(0, "Are you sure?", 0, MB_OKCANCEL) == IDOK)
				{
					std::string getFriends = http->Request("GET", "https://127.0.0.1/lol-chat/v1/friends", "", auth->leagueHeader, "", "", auth->leaguePort);

					Json::CharReaderBuilder builder;
					const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
					JSONCPP_STRING err;
					Json::Value root;
					if (!reader->parse(getFriends.c_str(), getFriends.c_str() + static_cast<int>(getFriends.length()), &root, &err))
					{
						result = "Failed to parse JSON";
					}
					else
					{
						if (root.isArray())
						{
							for (Json::Value::ArrayIndex i = 0; i < root.size(); ++i)
							{
								std::string req = "https://127.0.0.1/lol-chat/v1/friends/" + root[i]["pid"].asString();
								http->Request("DELETE", req, "", auth->leagueHeader, "", "", auth->leaguePort);
							}
							result = "Deleted " + std::to_string(root.size()) + " friends";
						}
					}
				}
			}

			static Json::StreamWriterBuilder wBuilder;
			static std::string sResultJson;
			static char* cResultJson;

			if (!result.empty())
			{
				Json::CharReaderBuilder builder;
				const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
				JSONCPP_STRING err;
				Json::Value root;
				if (!reader->parse(result.c_str(), result.c_str() + static_cast<int>(result.length()), &root, &err))
					sResultJson = result;
				else
				{
					sResultJson = Json::writeString(wBuilder, root);
				}
				result = "";
			}

			if (!sResultJson.empty())
			{
				cResultJson = &sResultJson[0];
				ImGui::InputTextMultiline("##miscResult", cResultJson, sResultJson.size() + 1, ImVec2(600, 200));
			}

			ImGui::EndTabItem();
		}
	}
};