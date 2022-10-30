#include <GLFW/glfw3.h>

#include "SoundGUI.h"

int main(int argc, char** argv) {
	
	SoundGUI gui;
	
	if (gui.Initialize() != 0) {
		return 1;
	}

	while (!glfwWindowShouldClose(gui.window))
	{
		//user input
		glfwPollEvents();

		//called each frame
		gui.NewFrame();
		gui.Update();
		gui.Render();
		glfwSwapBuffers(gui.window);
	}
	gui.ShutDown();
	
	return 0;
}