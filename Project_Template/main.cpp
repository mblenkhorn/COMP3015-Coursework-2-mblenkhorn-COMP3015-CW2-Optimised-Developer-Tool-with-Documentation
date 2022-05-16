#include "helper/scene.h"
#include "helper/scenerunner.h"
#include "scenebasic_uniform.h"


int main(int argc, char* argv[])
{
	SceneRunner runner("COMP3015 Coursework 2");

	std::unique_ptr<Scene> scene;

	scene = std::unique_ptr<Scene>(new SceneBasic_Uniform());


	return runner.run(*scene);
}