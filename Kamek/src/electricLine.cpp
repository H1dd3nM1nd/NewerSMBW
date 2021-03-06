#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>

const char* ElectricLineFileList[] = {
	"boss_koopaJr_toge",
	NULL
};

class daElectricLine : public dEn_c {
	int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();

	mHeapAllocator_c allocator;
	nw4r::g3d::ResFile resFile;

	dEn_c *Needles;
	u32 delay;
	u32 timer;
	bool loops;

	static daElectricLine *build();

	USING_STATES(daElectricLine);
	DECLARE_STATE(Activate);
	DECLARE_STATE(Deactivate);
	DECLARE_STATE(Die);
};

daElectricLine *daElectricLine::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daElectricLine));
	return new(buffer) daElectricLine;
}

CREATE_STATE(daElectricLine, Activate);
CREATE_STATE(daElectricLine, Deactivate);
CREATE_STATE(daElectricLine, Die);

int daElectricLine::onCreate() {
	Vec temppos = this->pos;
	temppos.x += 24.0;
	
	char settings = this->settings & 1;

	// Setting for rotation: 0 = facing right, 1 = facing left
	if (settings)
		temppos.x -= 32.0;

	Needles = (daNeedles*)create(NEEDLE_FOR_KOOPA_JR_B, settings, &temppos, &this->rot, 0);
	Needles->doStateChange(&daNeedles::StateID_DemoWait);

	// Delay in 1/6ths of a second
	this->delay = (this->settings >> 16) * 10;
	this->loops = (this->settings >> 4);

	// State Changers
	doStateChange(&StateID_Activate);
	return true;
}

int daElectricLine::onDelete() {
	return true;
}

int daElectricLine::onExecute() {
	acState.execute();
	return true;
}

int daElectricLine::onDraw() {
	return true;
}

////////////////////
// Activate State //
////////////////////

void daElectricLine::beginState_Activate() {
	this->timer = this->delay;
	Needles->doStateChange(&daNeedles::StateID_Idle);
}

void daElectricLine::executeState_Activate() {
	if (this->loops) {
		this->timer--;
		if (this->timer == 0)
			doStateChange(&StateID_Deactivate);
	}
}

void daElectricLine::endState_Activate() {
}

//////////////////////
// Deactivate State //
//////////////////////

void daElectricLine::beginState_Deactivate() {
	this->timer = this->delay;
	Needles->removeMyActivePhysics();
	Needles->doStateChange(&daNeedles::StateID_DemoWait);
}

void daElectricLine::executeState_Deactivate() {
	this->timer--;
	if (this->timer == 0)
		doStateChange(&StateID_Activate);
}

void daElectricLine::endState_Deactivate() {
	Needles->addMyActivePhysics();
}

///////////////
// Die State //
///////////////

void daElectricLine::beginState_Die() {
	Needles->doStateChange(&daNeedles::StateID_Die);
}

void daElectricLine::executeState_Die() {
}

void daElectricLine::endState_Die() {
}
