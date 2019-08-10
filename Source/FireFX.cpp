#include "FireFX.h"


using namespace std;
using namespace glm;

FireFX::FireFX()
{
#if defined(PLATFORM_OSX)
	//    int billboardTextureID = TextureLoader::LoadTexture("Textures/BillboardTest.bmp");
	int billboardTextureID = TextureLoader::LoadTexture("Textures/Particle.png");
#else
	//    int billboardTextureID = TextureLoader::LoadTexture("../Assets/Textures/BillboardTest.bmp");
	int billboardTextureID = TextureLoader::LoadTexture("../Assets/Textures/Particle.png");
#endif

	assert(billboardTextureID != 0);

	billboardList = new BillboardList(2048, billboardTextureID);

	loadProperties();
}


FireFX::~FireFX()
{
	delete particleSystem;
	delete billboardList;
	delete parent;
}

void FireFX::Update(float dt)
{
	billboardList->Update(dt);
	particleSystem->Update(dt);
}

void FireFX::Draw()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	billboardList->Draw();
	glDisable(GL_BLEND);
}

void FireFX::loadProperties()
{
	// Using case-insensitive strings and streams for easier parsing

	string particle_path = "../Assets/Scenes/FireFx.scene";

	ci_ifstream input;
	input.open(particle_path, ios::in);

	// Invalid file
	if (input.fail())
	{
		fprintf(stderr, "Error loading file: %s\n", particle_path);
		getchar();
		exit(-1);
	}

	ci_string item;
	while (std::getline(input, item, '['))
	{
		ci_istringstream iss(item);

		ci_string result;
		if (std::getline(iss, result, ']'))
		{
			if (result == "cube")
			{

				parent = new CubeModel();
				parent->Load(iss);

			}else if (result == "particledescriptor")
			{
				ParticleDescriptor* psd = new ParticleDescriptor();
				psd->Load(iss);

				ParticleEmitter* emitter = new ParticleEmitter(vec3(0.0f, 0.0f, 0.0f), parent);

				particleSystem = new ParticleSystem(emitter, psd, this);
			}
			else if (result.empty() == false && result[0] == '#')
			{
				// this is a comment line
			}
			else
			{
				fprintf(stderr, "Error loading scene file... !");
				getchar();
				exit(-1);
			}
		}
	}

	input.close();
}
