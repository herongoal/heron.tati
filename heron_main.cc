#include "heron_engine.h"


using namespace heron::tati;
using namespace std;


int main(int argc, char *argv[])
{
	string log_file="";
	heron_engine* engine = heron_engine::create(log_file, log_level_debug, 2000, 2,2);
	int result = engine->init();
	engine->start_service();
	engine->stop_service();
}
