#include "FireFX.h"


using namespace std;
using namespace glm;

FireFX::FireFX()
{
#if defined(PLATFORM_OSX)
	int smokeTextureID = TextureLoader::LoadTexture("Textures/fire_cutout_2x2.png");
	int fireTextureID = TextureLoader::LoadTexture("Textures/fire_cutout_2x2.png");
#else
	int smokeTextureID = TextureLoader::LoadTexture("../Assets/Textures/fire_cutout_2x2.png");
	int fireTextureID = TextureLoader::LoadTexture("../Assets/Textures/fire_cutout_2x2.png");
#endif

	assert(smokeTextureID != 0);
	assert(fireTextureID != 0);

	bList = std::vector<BillboardList*>();
	pSystems = std::vector<ParticleSystem*>();
	numOfpSystems = 1;

	bList.push_back(new BillboardList(2048, smokeTextureID, 2));

	loadProperties();
}


FireFX::~FireFX()
{
	for (size_t i = 0; i < numOfpSystems; i++)
	{
		// delete bList[i];
		delete pSystems[i];
	}
	delete parent;
}

void FireFX::Update(float dt)
{
	for (size_t i = 0; i < numOfpSystems; i++)
	{
		pSystems[i]->Update(dt);
		bList[i]->Update(dt);
	}

	parent->Update(dt);
}

void FireFX::Draw()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (size_t i = 0; i < numOfpSystems; i++)
	{
		bList[i]->Draw();
	}

	parent->Draw();

	glDisable(GL_BLEND);
}

void FireFX::loadProperties()
{
	// Using case-insensitive strings and streams for easier parsing

	string particle_path;

#if defined(PLATFORM_OSX)
    particle_path = "Scenes/FireFx.scene";
#else
    particle_path = "../Assets/Scenes/FireFx.scene";
#endif
    
    
	ci_ifstream input;
	input.open(particle_path, ios::in);

	int id = 0;

	// Invalid file
	if (input.fail())
	{
		//fprintf(stderr, "Error loading file: %s\n", particle_path);
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

				ParticleEmitter* emitter = new ParticleEmitter(glm::vec3(0.0f, 0.0f, 0.0f), parent);
				emitter->setIndex(id);

				pSystems.push_back(new ParticleSystem(emitter, psd, this, id++));

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
