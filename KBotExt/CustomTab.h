#pragma once

#include "Definitions.h"
#include "Includes.h"
#include "HTTP.h"
#include "Auth.h"

class CustomTab
{
public:
	static void Render()
	{
		if (ImGui::BeginTabItem("Custom"))
		{
			static char method[50];
			ImGui::Text("Method:");
			ImGui::InputText("##inputMethod", method, IM_ARRAYSIZE(method));

			static char urlText[1024 * 16];
			ImGui::Text("URL:");
			ImGui::InputTextMultiline("##inputUrl", urlText, IM_ARRAYSIZE(urlText), ImVec2(600, 20));

			static char requestText[1024 * 16];
			ImGui::Text("Body:");
			ImGui::InputTextMultiline("##inputBody", (requestText), IM_ARRAYSIZE(requestText), ImVec2(600, 100), ImGuiInputTextFlags_AllowTabInput);

			static std::string customHeader = auth->leagueHeader;
			static int customPort = auth->leaguePort;

			if (ImGui::CollapsingHeader("Custom Port/Header"))
			{
				static char inputPort[64] = "";
				ImGui::Text("Port:");
				ImGui::InputText("##inputPort", inputPort, 64, ImGuiInputTextFlags_CharsDecimal);
				std::string sPort = std::string(inputPort);
				if (!sPort.empty())
					customPort = std::stoi(sPort);
				else
					customPort = -1;

				static char inputHeader[1024 * 16];
				ImGui::Text("Header:");
				ImGui::InputTextMultiline("##inputHeader", (inputHeader), IM_ARRAYSIZE(inputHeader), ImVec2(600, 100), ImGuiInputTextFlags_AllowTabInput);
				std::string sHeader = std::string(inputHeader);
				customHeader = sHeader;
			}
			else
			{
				customHeader = auth->leagueHeader;
				customPort = auth->leaguePort;
			}

			static std::string result;
			if (ImGui::Button("Send custom request##customTab"))
			{
				std::string sURL = std::string(urlText);

				if (sURL.find("https://127.0.0.1") == std::string::npos)
				{
					if (sURL.find("https://") == std::string::npos && sURL.find("http://") == std::string::npos)
					{
						while (sURL[0] == ' ' || sURL[0] == '\n')
							sURL.erase(sURL.begin());
						if (sURL[0] != '/')
							sURL.insert(0, "/");
						sURL.insert(0, "https://127.0.0.1");
					}
				}
				result = http->Request(method, sURL, requestText, customHeader, "", "", customPort);
			}
			ImGui::Text("Result:");
			ImGui::SameLine();
			if (ImGui::Button("Copy to clipboard##customTab"))
			{
				utils->CopyToClipboard(result);
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
				ImGui::InputTextMultiline("##customResult", cResultJson, sResultJson.size() + 1, ImVec2(600, 300));
			}

			ImGui::EndTabItem();
		}
	}
};