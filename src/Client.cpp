#include "render/Renderer.hpp"

int main()
{
	Renderer app;

	const std::vector<Renderer::Vertex> vertices = {
		{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}};

	const std::vector<uint16_t> indices = {
		0, 1, 2, 2, 3, 0};

	try
	{
		app.RenderModelCreate(1, Renderer::RenderModel(vertices, indices));

		app.RenderTargetCreate(1, 1);
		app.RenderTargetUpdate(1, glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f)));

		app.RenderTargetCreate(2, 1);
		app.RenderTargetUpdate(2, glm::translate(glm::mat4(1.0f), glm::vec3(-1.5f, -0.5f, 0.0f)));

		app.ViewTransformUpdate(glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));

		while (!glfwWindowShouldClose(app.GetWindowHandle()))
		{
			glfwPollEvents();
			app.Draw();
		}
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}