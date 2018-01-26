#include <cmath>
#include "../include/image.h"
#include "../include/emitter.h"


Emitter::Emitter(Image* image, bool autofade) {
	this->image = image;
	this->autofade = autofade;

	x = 0;
	y = 0;

	minrate = 0;
	maxrate = 0;
	minvelx = 0;
	maxvelx = 0;
	minvely = 0;
	maxvely = 0;
	minangvel = 0;
	maxangvel = 0;
	minlifetime = 0;
	maxlifetime = 0;

	minr = 0;
	ming = 0;
	minb = 0;
	maxr = 0;
	maxg = 0;
	maxb = 0;
	blendMode = Renderer::ADDITIVE;

	emitting = false;
	accumulatedRate = 0;
}

void Emitter::Update(double elapsed) {
	// Add new particles
	if (emitting) {
		double velx, vely, angvel, lifetime;
		uint8 r, g, b;

		double dRate = ((minrate + fmod(rand(), maxrate - minrate + 1)) * elapsed) + accumulatedRate;
		uint32 uRate = dRate;
		accumulatedRate = dRate - uRate;

		for (uint32 i = 0; i < uRate; i++) {
			velx = minvelx + fmod(rand(), maxvelx - minvelx + 1);
			vely = minvely + fmod(rand(), maxvely - minvely + 1);
			angvel = minangvel + fmod(rand(), maxangvel - minangvel + 1);
			lifetime = minlifetime + fmod(rand(), maxlifetime - minlifetime + 1);

			r = minr + (rand() % (maxr - minr + 1));
			g = ming + (rand() % (maxg - ming + 1));
			b = minb + (rand() % (maxb - minb + 1));

			Particle particle(image, velx, vely, angvel, lifetime, autofade);
			particle.SetPosition(x, y);
			particle.SetColor(r, g, b);
			particle.SetBlendMode(blendMode);

			particles.Add(particle);
		}
	}

	// Update current particles
	if (particles.Size() > 0) {
		for (uint32 i = 0; i < particles.Size(); i++) {
			particles[i].Update(elapsed);

			// Check if is affected by each affector
			for (uint32 j = 0; j < affectors.Size(); j++)
				affectors[j].Update(particles[i]);

			// Delete particle if necessary
			if (particles[i].GetLifeTime() <= 0)
				particles.RemoveAt(i);
		}
	}
}

void Emitter::Render() const {
	Renderer::Instance().SetBlendMode(blendMode);

	for (uint32 i = 0; i < particles.Size(); i++)
		particles[i].Render();
}