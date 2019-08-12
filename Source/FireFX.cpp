#include "FireFX.h"


using namespace std;
using namespace glm;

std::vector<ParticleDescriptor> FireFX::descriptors = std::vector<ParticleDescriptor>();

FireFX::FireFX(int type, Model* parent): parent(parent)
{
#if defined(PLATFORM_OSX)
	int fireTextureID = TextureLoader::LoadTexture("Textures/fire_cutout_2x2.png");
#else
	int fireTextureID = TextureLoader::LoadTexture("../Assets/Textures/fire_cutout_2x2.png");
#endif

	assert(fireTextureID != 0);

	numOfpSystems = 1;
	id = 0;
	bList = std::vector<BillboardList*>();
	pSystems = std::vector<ParticleSystem*>();

	bList.push_back(new BillboardList(2048, fireTextureID, 2));
	
	ParticleEmitter* emitter = new ParticleEmitter(glm::vec3(0.0f, 1.0f, 0.0f), parent);
	
	emitter->setIndex(id);

	pSystems.push_back(new ParticleSystem(emitter, &(descriptors[type]), this, id));
	
	id++;

}


FireFX::~FireFX()
{
	for (size_t i = 0; i < numOfpSystems; i++)
	{
		// delete bList[i];
		delete pSystems[i];
	}
}

void FireFX::Update(float dt)
{
	for (size_t i = 0; i < numOfpSystems; i++)
	{
		pSystems[i]->Update(dt);
		bList[i]->Update(dt);
	}
}

void FireFX::Draw()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (size_t i = 0; i < numOfpSystems; i++)
	{
		bList[i]->Draw();
	}

	glDisable(GL_BLEND);
}

void FireFX::loadDescriptors()
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
			if (result == "particledescriptor")
			{
				ParticleDescriptor* psd = new ParticleDescriptor();
				psd->Load(iss);

				descriptors.push_back(*psd);
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
