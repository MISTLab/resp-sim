#include "configEngine.hpp"

/*
 * template<class issueWidth> class __EXAMPLE__Call : public reconfCB<issueWidth>{
 * private:
 * 	configEngine* cE;
 * public:
 * 	__EXAMPLE__Call(configEngine* mycE, sc_time latency = SC_ZERO_TIME, unsigned int width = 1, unsigned int height = 1):
 * 		 reconfCB<issueWidth>(latency, width, height), cE(mycE)	{}
 * 	bool operator()(ABIIf<issueWidth> &processorInstance){
 * 		processorInstance.preCall();
 * 		std::vector< issueWidth > callArgs = processorInstance.readArgs();
 * 
 * 		Arguments Manipulation Code
 * 
 * 		processorInstance.setRetVal(__RETURN_VALUE__);
 * 		processorInstance.returnFromCall();
 * 		processorInstance.postCall();
 * 		return true;
 * 	}
 * };
 */


template<class issueWidth> class printValueCall : public reconfCB<issueWidth>{
private:
	configEngine* cE;
public:
	printValueCall(configEngine* mycE, sc_time latency = SC_ZERO_TIME, unsigned int width = 1, unsigned int height = 1):
		reconfCB<issueWidth>(latency, width, height), cE(mycE)	{}
	bool operator()(ABIIf<issueWidth> &processorInstance){
		processorInstance.preCall();
		std::vector< issueWidth > callArgs = processorInstance.readArgs();

		(this->cE)->confexec("printValue", this->latency, this->width, this->height, false);
		(this->cE)->printSystemStatus();

		unsigned int param1 = callArgs[0];
		cout << "(CppCall) Value: " << param1 << endl;
//		cout << "Width: " << this->width << "\tHeight: " << this->height << "\t Latency: " << this->latency.to_string() << endl;

		processorInstance.setRetVal(param1 + 1);
		processorInstance.returnFromCall();
		processorInstance.postCall();
		return true;
	}
};

template<class issueWidth> class sumCall : public reconfCB<issueWidth>{
private:
	configEngine* cE;
public:
	sumCall(configEngine* mycE, sc_time latency = SC_ZERO_TIME, unsigned int width = 1, unsigned int height = 1):
		reconfCB<issueWidth>(latency, width, height), cE(mycE)	{}
	bool operator()(ABIIf<issueWidth> &processorInstance){
		processorInstance.preCall();
		std::vector< issueWidth > callArgs = processorInstance.readArgs();

		(this->cE)->confexec("sum", this->latency, this->width, this->height, false);
		(this->cE)->printSystemStatus();

		cout << "(CppCall) Summing values..." << endl;
//		cout << "Width: " << this->width << "\tHeight: " << this->height << "\t Latency: " << this->latency.to_string() << endl;
		unsigned int a = processorInstance.readMem(callArgs[0]);
		unsigned int b = processorInstance.readMem(callArgs[1]);
		unsigned int c = a + b;

		processorInstance.setRetVal(c);
		processorInstance.returnFromCall();
		processorInstance.postCall();
		return true;
	}
};

template<class issueWidth> class generateCall : public reconfCB<issueWidth>{
private:
	configEngine* cE;
public:
	generateCall(configEngine* mycE, sc_time latency = SC_ZERO_TIME, unsigned int width = 1, unsigned int height = 1):
		reconfCB<issueWidth>(latency, width, height), cE(mycE)	{}
	bool operator()(ABIIf<issueWidth> &processorInstance){
		processorInstance.preCall();
		std::vector< issueWidth > callArgs = processorInstance.readArgs();

		(this->cE)->confexec("generate", this->latency, this->width, this->height, false);
		(this->cE)->printSystemStatus();

		cout << "(CppCall) Generating value..." << endl;
//		cout << "Width: " << this->width << "\tHeight: " << this->height << "\t Latency: " << this->latency.to_string() << endl;
		unsigned int tmp = processorInstance.readMem(callArgs[0]);
		tmp = 20;
		processorInstance.writeMem(callArgs[0],tmp);

		processorInstance.setRetVal(0);
		processorInstance.returnFromCall();
		processorInstance.postCall();
		return true;
	}
};

void configEngine::registerCppCall(string funName, sc_time latency, unsigned int w, unsigned int h){
/*
 *	if (funName=="__EXAMPLE__") {
 *		__EXAMPLE__Call<unsigned int> *rcb = NULL;
 *		rcb = new __EXAMPLE__Call<unsigned int>(this,latency,w,h);
 *		if (!(this->recEmu.register_call(funName, *rcb))) delete rcb;
 *	}
 */
	if (funName=="printValue") {
		printValueCall<unsigned int> *rcb = NULL;
		rcb = new printValueCall<unsigned int>(this,latency,w,h);
		if (!(this->recEmu.register_call(funName, *rcb))) delete rcb;
	}
	if (funName=="sum") {
		sumCall<unsigned int> *rcb = NULL;
		rcb = new sumCall<unsigned int>(this,latency,w,h);
		if (!(this->recEmu.register_call(funName, *rcb))) delete rcb;
	}
	if (funName=="generate") {
		generateCall<unsigned int> *rcb = NULL;
		rcb = new generateCall<unsigned int>(this,latency,w,h);
		if (!(this->recEmu.register_call(funName, *rcb))) delete rcb;
	}
}

