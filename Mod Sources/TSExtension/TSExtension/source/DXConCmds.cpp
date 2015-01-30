/**
 *	
 */
#define endian(hex) (((hex & 0x000000FF) << 24)+((hex & 0x0000FF00) << 8)+((hex & 0x00FF0000)>>8)+((hex & 0xFF000000) >> 24))
#include <LinkerAPI.h>
#include <DXAPI/DXAPI.h>
#include <DXAPI/MatMath.h>
#include <set>

static float counter=0;
static float mpsx=0.0;
static float mpsy=0.0;
static float mpsz=0.0;
static bool moveplayerstoo=1;
static bool clientupdate=false;
static float timecounter=0;
#define CMALLOC (char*)malloc

void sendCommandAll(int argc,char *cmd[])
{
	unsigned int mainsim=(unsigned int) Sim::findObjectc("ClientCommandGroup");
	if (mainsim) {
		DX::SimGroup ccg=DX::SimGroup(mainsim);
		for (unsigned int x=0; x<ccg.getCount(); x++){
			DX::GameConnection gc = DX::GameConnection(ccg.getObject(x));
			char argv2[256][256];
			for (int c=0; c<argc;c++) {
				strcpy(argv2[c+2],cmd[c]);
			}
			strcpy(argv2[0],"commandToClient");
			char id[30]="";
			sprintf (id,"%d",gc.identifier);
			strcpy(argv2[1],id);
			Con::execute((S32)argc+2,(const char **)cmd);
		}

	}
}
void sendGhostCommandAll(int argc,unsigned int obj, DX::Point3F pos)
{
	unsigned int mainsim=(unsigned int) Sim::findObjectc("ClientCommandGroup");
	if (mainsim) {
		DX::SimGroup ccg=DX::SimGroup(mainsim);
		for (unsigned int x=0; x<ccg.getCount(); x++){
			DX::GameConnection gc = DX::GameConnection(ccg.getObject(x));
			char id[30]="";

			
			if (obj!=0) {
				DX::SceneObject sobj = DX::SceneObject(obj);
				unsigned int idx = gc.getGhostIndex(sobj);
				if (idx!=0) {
					//Con::printf("%s %s %s %s %s %s",argv2[0],argv2[1],argv2[2],argv2[3],argv2[4],argv2[5]);
					//Con::execute((S32)argc+2,(const char **)argv2);
					char evalstring[2048]="";
					sprintf (evalstring,"commandToClient(%d,'%s',%d,%g,%g,%g);",gc.identifier,"setPosition",idx,pos.x,pos.y,pos.z);
					Con::eval(evalstring, false, NULL);
				}
			}
		}

	}
}

void setClientPosition(unsigned int object,DX::Point3F pos){
						sendGhostCommandAll(5,object,pos);
						
}
static unsigned int objcounter=0;
static unsigned int scenearray[40000000];
static unsigned int objcount1=0;
void collide(unsigned int simgroup){
	unsigned int mainsim=(unsigned int)simgroup;
	DX::Point3F tmppoint;
	objcounter=0;
	objcount1=0;
	if (mainsim) {
		DX::SimObject sim1=DX::SimObject(mainsim);
		if ((strcmp((sim1.getClassName()),"SimGroup")==0) || (strcmp((sim1.getClassName()),"SimSet")==0)) {
			DX::SimGroup sim2 = DX::SimGroup(mainsim);
			for (unsigned int x=0; x<sim2.getCount(); x++) {
				DX::SimObject obj2=DX::SimObject(sim2.getObject(x));
				if ((strcmp((obj2.getClassName()),"SimGroup")!=0)&&(strcmp((obj2.getClassName()),"SimSet")!=0)) {
						scenearray[objcounter]=sim2.getObject(x);
						objcounter++;
				} else {
					//collideRecursive(sim2.getObject(x));
				}
			}
			objcount1=objcounter;
			objcounter=0;
			for (unsigned int idx=0; idx<objcount1; idx++) {
				DX::SceneObject sobj2=DX::SceneObject(scenearray[idx]);
				DX::MatrixF mat1=DX::MatrixF(sobj2.objtoworld);
				DX::Point3F test;
				mat1.getColumn(3,&test);
				tmppoint=test;
				for (unsigned int iidx=0; iidx<objcount1; iidx++) {
						DX::SceneObject sobj3=DX::SceneObject(scenearray[iidx]);
						DX::MatrixF mat2=DX::MatrixF(sobj3.objtoworld);
						DX::Point3F test2;
						mat2.getColumn(3,&test2);
						if (DX::pointdistance(test,test2)>3) {
							char evalstring[1024]="";
							sprintf (evalstring,"ProjCollisionCallback(%d,%d);",sobj2.identifier,sobj3.identifier);
							Con::eval(evalstring, false, NULL);
						}
				}
			}





		}
	}
	
}

void moveRecursive(unsigned int simgroup,float xoff, float yoff, float zoff){
	
	unsigned int mainsim=(unsigned int)simgroup;
	if (mainsim) {
		DX::SimObject sim1=DX::SimObject(mainsim);
		if ((strcmp((sim1.getClassName()),"SimGroup")==0) || (strcmp((sim1.getClassName()),"SimSet")==0)) {
			DX::SimGroup sim2 = DX::SimGroup(mainsim);
			for (unsigned int x=0; x<sim2.getCount(); x++) {
				DX::SimObject obj2=DX::SimObject(sim2.getObject(x));
				if ((strcmp((obj2.getClassName()),"SimGroup")!=0)&&(strcmp((obj2.getClassName()),"SimSet")!=0)) {
					if (obj2.type&0x8){
						DX::SceneObject sobj = DX::SceneObject(sim2.getObject(x));
						DX::MatrixF mat1=DX::MatrixF(sobj.objtoworld);
						DX::Point3F test;
						mat1.getColumn(3,&test);
						test.x+=xoff;
						test.y+=yoff;
						test.z+=zoff;
						mat1.setColumn(3,&test);
						sobj.setTransform(mat1.m);
						if (moveplayerstoo==1) {
							char evalstr[512]="";
							
							sprintf (evalstr,"onMoveRoutine(%d,\"%g %g %g\",\"%g %g %g\",%g);",sobj.identifier,xoff,yoff,zoff,sobj.worldspherecenter.x,sobj.worldspherecenter.y,sobj.worldspherecenter.z,sobj.worldsphereradius);
							Con::eval(evalstr,false,NULL);
						}
						if (clientupdate==true) {
							setClientPosition(sobj.base_pointer_value,test);
						}
					}
				} else {
					moveRecursive(sim2.getObject(x),xoff,yoff,zoff);
				}
			}
		}
	}
}
bool conclientCmdSetPosition(Linker::SimObject *obj, S32 argc, const char* argv[]) 
{	DX::NetConnection conn = DX::NetConnection((unsigned int)Sim::findObjectc("ServerConnection"));
	unsigned int myptr=0;
	//Con::printf ("ghost index %d\n",atoi(argv[1]));
	//Con::printf ("vector %g %g %g\n",atof(argv[2]),atof(argv[3]),atof(argv[4]));
	if (conn.base_pointer_value) {
		myptr = conn.resolveGhost(atoi(argv[1])).base_pointer_value;
		DX::SceneObject scene1 = DX::SceneObject(myptr);
		if (scene1.base_pointer_value) {
			//Con::printf ("ghost obj id %d\n",scene1.identifier);
			DX::MatrixF mat1=DX::MatrixF(scene1.objtoworld);
			DX::MatrixF mat2=DX::MatrixF(scene1.renderobjtoworld);
			DX::Point3F test;
			mat1.getColumn(3,&test);
			test.x=atof(argv[2]);
			test.y=atof(argv[3]);
			test.z=atof(argv[4]);
			mat1.setColumn(3,&test);
			mat2.setColumn(3,&test);
			scene1.setTransform(mat1.m);
			//Con::printf ("done\n");
			return 0;
		} else {
			return 1;
		}
	}
	return 1;
}
void serverProcessReplacement(unsigned int timeDelta) {
	unsigned int servertickaddr=0x602350;
	unsigned int serverthisptr=0x9E5EC0;
	float basex=348.08;
	float basey=-178.761;
	float basez=113.037;
	//timecounter+=((float)((timeDelta)*0.001));
	//if (timecounter>=0.002) {
		clientupdate=true;	
	//}
	if (Sim::findObjectc("MoveGroup")) {
		basex+=counter;
		DX::SceneObject sobj = DX::SceneObject((unsigned int)Sim::findObjectc("MoveGroup"));
		moveRecursive((unsigned int)Sim::findObjectc("MoveGroup"),(mpsx*((float)((timeDelta)*0.001))),(mpsy*((float)((timeDelta)*0.001))),(mpsz*((float)((timeDelta)*0.001))));
		Con::eval("MoveRoutineDone();",false,NULL);
	}
	if (Sim::findObjectc("CollideGroup")) {
		basex+=counter;
		DX::SceneObject sobj = DX::SceneObject((unsigned int)Sim::findObjectc("CollideGroup"));
		collide((unsigned int)Sim::findObjectc("CollideGroup"));
//		Con::eval("MoveRoutineDone();",false,NULL);
	}
	if (clientupdate==true) {
	timecounter=0;
	//clientupdate=false;
	}

	__asm 
	{
		mov ecx,serverthisptr
		push timeDelta
		call servertickaddr
	}
	
	return;
}
const char* conGetPosition(Linker::SimObject * obj, S32 argc, const char *argv[]) {
	char returnstr[256]="";
	DX::SceneObject sobj = DX::SceneObject((unsigned int)obj);
	if (obj!=NULL) {
			unsigned int bpv = (sobj.base_pointer_value);
			unsigned int *matrixptr =(unsigned int *) (bpv+0x9C);
			float *matrix1=(float*) matrixptr;
			DX::MatrixF mat1=DX::MatrixF(sobj.objtoworld);
			DX::Point3F test=DX::Point3F();
			mat1.getColumn(3,&test);
			sprintf (returnstr,"%g %g %g",test.x,test.y,test.z);
		}
	return returnstr;
}
/*const char* congetTestAddr(Linker::SimObject *obj, S32 argc, const char *argv[]) {

char test[256]="";
int tpur=(signed int)*testpackUpdateReplacement;
sprintf(test,"B8%08XFFE0",endian(tpur));
return test;
}*/

const char* congetServPAddr(Linker::SimObject *obj, S32 argc, const char *argv[]) {
		char test[256] = "";
		char test2[256]="";
		int spr=(signed int)*serverProcessReplacement;
		sprintf(test2,"B8%08XFFD089EC5DC3",endian(spr));
		/*test2[0]=test[6];
		test2[1]=test[7];
		test2[2]=test[4];
		test2[3]=test[5];
		test2[4]=test[2];
		test2[5]=test[3];
		test2[6]=test[0];
		test2[7]=test[1];
		test2[8]=0;*/
		return test2;
}
bool conSetMPS(Linker::SimObject *obj, S32 argc, const char *argv[]) {
	mpsx=atof(argv[1]);
	mpsy=atof(argv[2]);
	mpsz=atof(argv[3]);
	return false;
}
const char *conGetAddress(Linker::SimObject *obj, S32 argc, const char *argv[])
{
	// Hmm...
	char result[256];
	sprintf(result, "%x", obj);
	return result;
}

bool conPlayerGetJumpingState(Linker::SimObject *obj, S32 argc, const char* argv[])
{
	DX::Player operand = DX::Player((unsigned int)obj);

	return operand.is_jumping;
}
bool conPlayerGetJettingState(Linker::SimObject *obj, S32 argc, const char* argv[])
{
	DX::Player operand = DX::Player((unsigned int)obj);

	return operand.is_jetting;
}

bool conGameConnectionSetHeatLevel(Linker::SimObject *obj, S32 argc, const char *argv[])
{
	DX::GameConnection operand = DX::GameConnection((unsigned int)obj);
	operand.getControlObject().heat_level = atof(argv[1]);
	return true;
}

bool conProjectileExplode(Linker::SimObject *obj, S32 argc, const char* argv[])
{
	Linker::Point3F position(0,0,0);
	Linker::Point3F normal(0,0,0);

	unsigned int collideType = atoi(argv[4]);
	DX::Projectile projectile((unsigned int)obj);

	projectile.net_flags |= DX::NetFlags::ScopeAlways;
	projectile.explode(position, normal, collideType);

	return true;
}

bool conProjectileMakeNerf(Linker::SimObject *obj, S32 argc, const char* argv[])
{
	DX::GrenadeProjectile grenade = DX::GrenadeProjectile((unsigned int)obj);
	grenade.hidden = true;

	return true;
}
bool conForceUpdate(Linker::SimObject *obj, S32 argc, const char* argv[]) {
	DX::NetConnection conn = DX::NetConnection((unsigned int)obj);
	DX::NetObject netobj = DX::NetObject((unsigned int)Sim::findObjectc(argv[2]));
	GhostInfo * mGhostRefs=conn.mGhostRefs;
	if (netobj.base_pointer_value!=0) {
	S32 index = conn.getGhostIndex(netobj);
		if (index > 0) {
			mGhostRefs[index].updateMask=mGhostRefs[index].updateMask | GameBaseMasks::InitialUpdateMask;
		}
		return 1;
	}
	else 
	{
		return 0;
	}
}
const char* conGetGhostIndex(Linker::SimObject *obj, S32 argc, const char* argv[]) {
	char outint[20]="";
	DX::NetConnection conn = DX::NetConnection((unsigned int)obj);
	DX::NetObject netobj = DX::NetObject((unsigned int)Sim::findObjectc(argv[2]));
	if (netobj.base_pointer_value!=0) {
		S32 index = conn.getGhostIndex(netobj);
		itoa(index,outint,10);
		return outint;
	} else {
		return "";
	}
}
const char* conResolveGhost(Linker::SimObject *obj, S32 argc, const char* argv[]) {
	char outint[20]="";
	DX::NetConnection conn = DX::NetConnection((unsigned int)obj);
	S32 id = atoi(argv[2]);
	DX::NetObject realobject = conn.resolveGhost(id);
	if (realobject.base_pointer_value) {
		return itoa(realobject.identifier,outint,10);
	}
	return "";
}
const char* conResolveGhostParent(Linker::SimObject *obj, S32 argc, const char* argv[]) {
	char outint[20]="";
	DX::NetConnection conn = DX::NetConnection((unsigned int)obj);
	S32 ghostindex = atoi(argv[2]);
	if (conn.base_pointer_value!=0) {
		if (conn.resolveGhostParent(ghostindex).base_pointer_value)
			{
				S32 objid = conn.resolveGhostParent(ghostindex).identifier;
				if (objid != 0) {
					itoa(objid,outint,10);
					return outint;
				}
			}
	}
		return "";
}

bool conclientCmdSetGhostTicks(Linker::SimObject *obj, S32 argc, const char* argv[]) {
		unsigned int result_ptr = 0;
		unsigned int my_ptr = 0;
		unsigned int ghostindex=atoi(argv[1]);
		DX::NetConnection conn = DX::NetConnection((unsigned int)Sim::findObjectc("ServerConnection"));
		if (conn.base_pointer_value) {
			DX::NetObject ghostobj = conn.resolveGhost(ghostindex);
			if (ghostobj.base_pointer_value) 
			{ 
				my_ptr=(unsigned int)ghostobj.base_pointer_value;
				if (atoi(argv[2])==1) {
					__asm
					{
						mov eax, my_ptr;
						add eax, 0x264;
						mov ebx,eax
						mov al, 1
						mov [ebx],al
						
					}	
				} else {
					__asm
					{
						mov eax, my_ptr;
						add eax, 0x264;
						mov ebx,eax
						mov al, 0
						mov [ebx],al
						
					}	
				}
			}
		}
		return 1;
}
bool conclientCmdSetProcessTicks(Linker::SimObject *obj, S32 argc, const char* argv[]) {
		unsigned int result_ptr = 0;
		unsigned int my_ptr = 0;
		DX::NetObject objptr=(unsigned int)Sim::findObjectc(argv[1]);
		if (objptr.base_pointer_value) 
		{ 
			my_ptr=objptr.base_pointer_value;
			if (atoi(argv[2])==1) {
				__asm
				{
					mov eax, my_ptr;
					add eax, 0x264;
					mov ebx,eax
					mov al, 1
					mov [ebx],al
					
				}	
			} else {
				__asm
				{
					mov eax, my_ptr;
					add eax, 0x264;
					mov ebx,eax
					mov al, 0
					mov [ebx],al
					
				}	
			}
		}
			return 1;
}
bool conSetProcessTicks(Linker::SimObject *obj, S32 argc, const char* argv[]) {

			unsigned int result_ptr = 0;
		unsigned int my_ptr = (unsigned int) obj;
		if (atoi(argv[2])==1) {
			__asm
			{
				mov eax, my_ptr;
				add eax, 0x264;
				mov ebx,eax
				mov al, 1
				mov [ebx],al
				
			}	
		} else {
			__asm
			{
				mov eax, my_ptr;
				add eax, 0x264;
				mov ebx,eax
				mov al, 0
				mov [ebx],al
				
			}	
		}

}
const char* conGrenadeProjectileGetPosition(Linker::SimObject *obj, S32 argc, const char* argv[])
{
	char result[256];

	DX::GrenadeProjectile grenade = DX::GrenadeProjectile((unsigned int)obj);
	sprintf_s<256>(result, "%f %f %f", grenade.position.x, grenade.position.y, grenade.position.z);
	return result;
}

const char* conGrenadeProjectileGetVelocity(Linker::SimObject *obj, S32 argc, const char* argv[])
{
	char result[256];

	DX::GrenadeProjectile grenade((unsigned int)obj);
	sprintf_s<256>(result, "%f %f %f", grenade.velocity.x, grenade.velocity.y, grenade.velocity.z);

	return result;
}

// General Commands ---------------------------------
#include <cstdarg>
#include <vector>
#include <string.h>

const char* conSprintf(Linker::SimObject *obj, S32 argc, const char* argv[])
{
	std::vector<const char*> input;
	for (unsigned int i = 2; i < argc; i++)
		input.push_back(argv[i]);

	char result[256];
	
	va_list variable_args = reinterpret_cast<va_list>(input.data());
	vsprintf(result, argv[1], variable_args);

	return result;
}