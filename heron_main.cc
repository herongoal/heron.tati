#include "heron_engine.h"


using namespace heron::tati;
using namespace std;


int main(int argc, char *argv[])
{
	heron_engine engine;
	int result = engine.init();
	engine.start_service();
	engine.stop_service();
}
