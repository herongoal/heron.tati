#include "heron_engine.h"

#include <iostream>


using namespace heron::tati;
using namespace std;


int main(int argc, char *argv[])
{
	string log_file="";
	heron_engine* engine = heron_engine::create(log_file, log_level_debug, 2000, 2,2);
	cout << "create done" << endl;
	int result = engine->init();
	if (heron_result_state::success!=result){
		cout << "init err" << endl;
		exit(0);
	}

	result = engine->listen_at_port("0.0.0.0", 8888);
	if (heron_result_state::success!=result){
		cout << "create listen endpoint err" << endl;
		exit(0);
	}

	engine->start_service();
	cout << "start done" << endl;
	engine->stop_service();
	cout << "stop done" << endl;
}
