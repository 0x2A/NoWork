#include "Editor.h"
#include "NoWork/imgui/imgui.h"
#include <chrono>
#include <thread>


static bool m_SubWindowVisible = true;
static bool m_SubWindow2Visible = true;
void Editor::Init()
{

}

void Editor::OnLoadContent()
{

}

void Editor::OnUpdate(double deltaTime)
{

}

void Editor::OnLoadRender()
{

}

void Editor::OnRender()
{
	RenderMainWindow();

	//Dumy Window
	static bool dummyCheckBox = false;
	if (m_SubWindowVisible)
	{
		ImGui::Begin("SubWindow", &m_SubWindowVisible);
		ImGui::Text("Test");
		ImGui::Checkbox("Test", &dummyCheckBox);
		ImGui::End();
	}
	if (m_SubWindow2Visible)
	{
		ImGui::Begin("SubWindow2", &m_SubWindow2Visible);
		ImGui::Text("Test 2");
		ImGui::Checkbox("Test 2", &dummyCheckBox);
		ImGui::End();
	}
}

void Editor::OnWindowResize(int newWidth, int newHeight)
{

}

static void HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

void Editor::RenderMainWindow()
{
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
	//if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
	//		window_flags |= ImGuiWindowFlags_NoBackground;

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("MainDockSpace", 0, window_flags);
	ImGui::PopStyleVar();

	ImGui::PopStyleVar(2);

	// DockSpace
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
	}

	RenderMainMenu();

	ImGui::End();
}


void Editor::RenderMainMenu()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit", ""))
				m_Framework->Exit();

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window"))
		{
			if (ImGui::MenuItem("Test Window 1", "", m_SubWindowVisible))
				m_SubWindowVisible = !m_SubWindowVisible;
			if (ImGui::MenuItem("Test Window 2", "", m_SubWindow2Visible))
				m_SubWindow2Visible = !m_SubWindow2Visible;
			ImGui::EndMenu();
		}

		
		ImGui::EndMenuBar();
	}
}

void Editor::OnShutdown()
{

}

void Editor::Exit()
{

}
