#include "ofLog.h"
#include "ofConstants.h"
#include <ofUtils.h>
#include <map>
#include <cstdarg>
#ifdef TARGET_ANDROID
	#include "ofxAndroidLogChannel.h"
#endif

using namespace std;

static ofLogLevel currentLogLevel =  OF_LOG_NOTICE;

bool ofLog::bAutoSpace = false;
string & ofLog::getPadding() {
	static string * padding = new string;
	return *padding;
}

static map<string,ofLogLevel> & getModules(){
	static map<string,ofLogLevel> * modules = new map<string,ofLogLevel>;
	return *modules;
}

static void noopDeleter(ofBaseLoggerChannel*){}

shared_ptr<ofBaseLoggerChannel> & ofLog::channel(){
#ifdef TARGET_ANDROID
	static shared_ptr<ofBaseLoggerChannel> channel = shared_ptr<ofxAndroidLogChannel>(new ofxAndroidLogChannel, std::function<void(ofBaseLoggerChannel *)>(noopDeleter));
#elif defined(TARGET_WIN32)
	static shared_ptr<ofBaseLoggerChannel> channel = IsDebuggerPresent() ? shared_ptr<ofBaseLoggerChannel>(new ofDebugViewLoggerChannel, std::function<void(ofBaseLoggerChannel *)>(noopDeleter)) : shared_ptr<ofBaseLoggerChannel>(new ofConsoleLoggerChannel, std::function<void(ofBaseLoggerChannel *)>(noopDeleter));
#else
	static shared_ptr<ofBaseLoggerChannel> channel = shared_ptr<ofConsoleLoggerChannel>(new ofConsoleLoggerChannel, std::function<void(ofBaseLoggerChannel *)>(noopDeleter));
#endif

	return channel;
}

//--------------------------------------------------
void ofSetLogLevel(ofLogLevel level){
	currentLogLevel = level;
}

//--------------------------------------------------
void ofSetLogLevel(string moduleName, ofLogLevel level){
	getModules()[moduleName] = level;
}

//--------------------------------------------------
ofLogLevel ofGetLogLevel(){
	return currentLogLevel;
}

//--------------------------------------------------
ofLogLevel ofGetLogLevel(string moduleName){
	if (getModules().find(moduleName) == getModules().end()) {
		return currentLogLevel;
	} else {
		return getModules()[moduleName];
	}
}

//--------------------------------------------------
void ofLogToFile(const std::filesystem::path & path, bool append){
	ofLog::setChannel(std::make_shared<ofFileLoggerChannel>(path,append));
}

//--------------------------------------------------
void ofLogToConsole(){
	ofLog::setChannel(shared_ptr<ofConsoleLoggerChannel>(new ofConsoleLoggerChannel, std::function<void(ofBaseLoggerChannel *)>(noopDeleter)));
}

#ifdef TARGET_WIN32
void ofLogToDebugView() {
	ofLog::setChannel(shared_ptr<ofDebugViewLoggerChannel>(new ofDebugViewLoggerChannel, std::function<void(ofBaseLoggerChannel *)>(noopDeleter)));
}
#endif

//--------------------------------------------------
ofLog::ofLog(){
	level = OF_LOG_NOTICE;
	moduleName = "";
	bPrinted = false;
}
		
//--------------------------------------------------
ofLog::ofLog(ofLogLevel _level){
	level = _level;
	moduleName = "";
	bPrinted = false;
}

//--------------------------------------------------
ofLog::ofLog(ofLogLevel level, const string & message){
	_log(level,"",message);
	bPrinted = true;
}

//--------------------------------------------------
ofLog::ofLog(ofLogLevel level, const char* format, ...){
	if(checkLog(level,"")){
		va_list args;
		va_start( args, format );
		channel()->log(level,"",format,args);
		va_end( args );
	}
	bPrinted = true;
}

//--------------------------------------------------
void ofLog::setAutoSpace(bool autoSpace){
	bAutoSpace = autoSpace;
	if(bAutoSpace){
		ofLog::getPadding() = " ";
	}
	else{
		ofLog::getPadding() = "";
	}
}

//-------------------------------------------------------
ofLog::~ofLog(){
	// don't log if we printed in the constructor already
	if(!bPrinted){
		_log(level,moduleName,message.str());
	}
}

bool ofLog::checkLog(ofLogLevel level, const string & moduleName){
	if(getModules().find(moduleName)==getModules().end()){
		if(level >= currentLogLevel) return true;
	}else{
		if(level >= getModules()[moduleName]) return true;
	}
	return false;
}

//-------------------------------------------------------
void ofLog::_log(ofLogLevel level, const string & moduleName, const string & message){
	if(checkLog(level,moduleName)){
		channel()->log(level,moduleName, message);
	}
}

//--------------------------------------------------
ofLogVerbose::ofLogVerbose(const string & _module){
	level = OF_LOG_VERBOSE;
	moduleName = _module;
	bPrinted=false;
}

ofLogVerbose::ofLogVerbose(const string & _module, const string & _message){
	_log(OF_LOG_VERBOSE,_module,_message);
	bPrinted = true;
}

ofLogVerbose::ofLogVerbose(const string & moduleName, const char* format, ...){
	if(checkLog(OF_LOG_VERBOSE, moduleName)){
		va_list args;
		va_start(args, format);
		channel()->log(OF_LOG_VERBOSE, moduleName, format, args);
		va_end(args);
	}
	bPrinted = true;
}

//--------------------------------------------------
ofLogNotice::ofLogNotice(const string & _module){
	level = OF_LOG_NOTICE;
	moduleName = _module;
	bPrinted=false;
}

ofLogNotice::ofLogNotice(const string & _module, const string & _message){
	_log(OF_LOG_NOTICE,_module,_message);
	bPrinted = true;
}

ofLogNotice::ofLogNotice(const string & moduleName, const char* format, ...){
	if(checkLog(OF_LOG_NOTICE, moduleName)){
		va_list args;
		va_start(args, format);
		channel()->log(OF_LOG_NOTICE, moduleName, format, args);
		va_end(args);
	}
	bPrinted = true;
}

//--------------------------------------------------
ofLogWarning::ofLogWarning(const string & _module){
	level = OF_LOG_WARNING;
	moduleName = _module;
	bPrinted=false;
}

ofLogWarning::ofLogWarning(const string & _module, const string & _message){
	_log(OF_LOG_WARNING,_module,_message);
	bPrinted = true;
}

ofLogWarning::ofLogWarning(const string & moduleName, const char* format, ...){
	if(checkLog(OF_LOG_WARNING, moduleName)){
		va_list args;
		va_start(args, format);
		channel()->log(OF_LOG_WARNING, moduleName, format, args);
		va_end(args);
	}
	bPrinted = true;
}

//--------------------------------------------------
ofLogError::ofLogError(const string & _module){
	level = OF_LOG_ERROR;
	moduleName = _module;
	bPrinted=false;
}

ofLogError::ofLogError(const string & _module, const string & _message){
	_log(OF_LOG_ERROR,_module,_message);
	bPrinted = true;
}

ofLogError::ofLogError(const string & moduleName, const char* format, ...){
	if(checkLog(OF_LOG_ERROR, moduleName)){
		va_list args;
		va_start(args, format);
		channel()->log(OF_LOG_ERROR, moduleName, format, args);
		va_end(args);
	}
	bPrinted = true;
}

//--------------------------------------------------
ofLogFatalError::ofLogFatalError(const string &  _module){
	level = OF_LOG_FATAL_ERROR;
	moduleName = _module;
	bPrinted=false;
}

ofLogFatalError::ofLogFatalError(const string & _module, const string & _message){
	_log(OF_LOG_FATAL_ERROR,_module,_message);
	bPrinted = true;
}

ofLogFatalError::ofLogFatalError(const string & moduleName, const char* format, ...){
	if(checkLog(OF_LOG_FATAL_ERROR, moduleName)){
		va_list args;
		va_start(args, format);
		channel()->log(OF_LOG_FATAL_ERROR, moduleName, format, args);
		va_end(args);
	}
	bPrinted = true;
}

//--------------------------------------------------
void ofLog::setChannel(shared_ptr<ofBaseLoggerChannel> _channel){
	channel() = _channel;
}

void ofSetLoggerChannel(shared_ptr<ofBaseLoggerChannel> loggerChannel){
	ofLog::setChannel(loggerChannel);
}

shared_ptr<ofBaseLoggerChannel> ofLog::getChannel(){
	return channel();
}

shared_ptr<ofBaseLoggerChannel> ofGetLoggerChannel(){
	return ofLog::getChannel();
}

string ofGetLogLevelName(ofLogLevel level, bool pad){
	switch(level){
		case OF_LOG_VERBOSE:
			return "verbose";
		case OF_LOG_NOTICE:
			return pad ? "notice " : "notice";
		case OF_LOG_WARNING:
			return "warning";
		case OF_LOG_ERROR:
			return pad ? " error " : "error";
		case OF_LOG_FATAL_ERROR:
			return pad ? " fatal " : "fatal";
		case OF_LOG_SILENT:
			return pad ? "silent " : "silent";
		default:
			return "";
	}
}

//--------------------------------------------------
void ofConsoleLoggerChannel::log(ofLogLevel level, const string & moduleName, const string & message){
	// print to cerr for OF_LOG_ERROR and OF_LOG_FATAL_ERROR, everything else to cout 
	ostream& out = level < OF_LOG_ERROR ? cout : cerr;
	out << "[" << ofGetLogLevelName(level, true)  << "] ";
	// only print the moduleName name if it's not ""
	if(moduleName != ""){
		out << moduleName << ": ";
	}
	out << message << endl;
}

void ofConsoleLoggerChannel::log(ofLogLevel level, const string & moduleName, const char* format, ...){
	va_list args;
	va_start(args, format);
	log(level, moduleName, format, args);
	va_end(args);
}

void ofConsoleLoggerChannel::log(ofLogLevel level, const string & moduleName, const char* format, va_list args){
	//thanks stefan!
	//http://www.ozzu.com/cpp-tutorials/tutorial-writing-custom-printf-wrapper-function-t89166.html
	FILE* out = level < OF_LOG_ERROR ? stdout : stderr;
	fprintf(out, "[%s] ", ofGetLogLevelName(level, true).c_str());
	if(moduleName != ""){
		fprintf(out, "%s: ", moduleName.c_str());
	}
	vfprintf(out, format, args);
	fprintf(out, "\n");
}


#ifdef TARGET_WIN32
#include <array>
void ofDebugViewLoggerChannel::log(ofLogLevel level, const string & moduleName, const string & message) {
	// print to cerr for OF_LOG_ERROR and OF_LOG_FATAL_ERROR, everything else to cout 
	stringstream out;
	out << "[" << ofGetLogLevelName(level, true) << "] ";
	// only print the moduleName name if it's not ""
	if (moduleName != "") {
		out << moduleName << ": ";
	}
	out << message << endl;
	OutputDebugStringA(out.str().c_str());
}

void ofDebugViewLoggerChannel::log(ofLogLevel level, const string & moduleName, const char* format, ...) {
	va_list args;
	va_start(args, format);
	log(level, moduleName, format, args);
	va_end(args);

}

void ofDebugViewLoggerChannel::log(ofLogLevel level, const string & moduleName, const char* format, va_list args) {
	std::string buffer;;
	buffer =  "[" + ofGetLogLevelName(level, true) + "] ";
	if (moduleName != "") {
		buffer += moduleName + ": ";
	}
	buffer += ofVAArgsToString(format, args);
	buffer += "\n";
	OutputDebugStringA(buffer.c_str());
}
#endif

//--------------------------------------------------
ofFileLoggerChannel::ofFileLoggerChannel(){
}

ofFileLoggerChannel::ofFileLoggerChannel(const std::filesystem::path & path, bool append){
	setFile(path,append);
}

ofFileLoggerChannel::~ofFileLoggerChannel(){
	close();
}

void ofFileLoggerChannel::close(){
	file.close();
}

void ofFileLoggerChannel::setFile(const std::filesystem::path & path,bool append){
	file.open(path,append?ofFile::Append:ofFile::WriteOnly);
	file << endl;
	file << endl;
	file << "--------------------------------------- " << ofGetTimestampString() << endl;
}

void ofFileLoggerChannel::log(ofLogLevel level, const string & moduleName, const string & message){
	file << "[" << ofGetLogLevelName(level, true) << "] ";
	if(moduleName != ""){
		file << moduleName << ": ";
	}
	file << message << endl;
}

void ofFileLoggerChannel::log(ofLogLevel level, const string & moduleName, const char* format, ...){
	va_list args;
	va_start(args, format);
	log(level, moduleName, format, args);
	va_end(args);
}

void ofFileLoggerChannel::log(ofLogLevel level, const string & moduleName, const char* format, va_list args){
	file << "[" << ofGetLogLevelName(level, true) << "] ";
	if(moduleName != ""){
		file << moduleName << ": ";
	}
	file << ofVAArgsToString(format,args) << endl;
}
