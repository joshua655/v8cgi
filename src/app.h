#ifndef _JS_APP_H
#define _JS_APP_H

#include <map>
#include <utility>
#include <vector>
#include <list>
#include <v8.h>
#include "cache.h"
#include "gc.h"

/**
 * This class defines a basic v8-based application.
 */
class v8cgi_App {
public:
	typedef std::vector<v8::Persistent<v8::Function> > funcvector;
	
	/* resolved native module, resolved js module */
	typedef std::vector<std::string> modulefiles;

	virtual ~v8cgi_App() {};
	/* once per app lifetime */
	virtual void init(); 
	/* once per request */
	void execute(char ** envp); 
	v8::Handle<v8::Object> require(std::string name, std::string moduleId);
	
	/* list of "onexit" functions */
	funcvector onexit;
	
	bool show_errors;
	int exit_code;

protected:
	/* env. preparation */
	virtual void prepare(char ** envp);

	/* config file */
	std::string cfgfile;
	/* main script file */
	std::string mainfile; 
	/* arguments after mainfile */
	std::vector<std::string> mainfile_args;
	/* create new v8 execution context */
	void create_context();
	/* delete existing context */
	void delete_context();
	/* setup the v8cgi free variable */
	void setup_v8cgi(v8::Handle<v8::Object> target);

private:
	/* current active context */
	v8::Persistent<v8::Context> context; 
	
	v8::Persistent<v8::Value> global;
	v8::Persistent<v8::ObjectTemplate> globalt;
	
	/* get configuration option */
	v8::Handle<v8::Value> get_config(std::string name);

	/* cache */
	Cache cache;
	/* GC notification engine */
	GC gc;

	std::string format_exception(v8::TryCatch* try_catch);
	void findmain();
	void js_error(std::string message);
	void finish();
	void clear_global();
	
	/* instance type info */
	virtual const char * instanceType() = 0;
	virtual const char * executableName() = 0;

	modulefiles resolve_module(std::string name, std::string relativeRoot);
	modulefiles resolve_extension(std::string path);
	int load_js(std::string filename, v8::Handle<v8::Function> require, v8::Handle<v8::Object> exports, v8::Handle<v8::Object> module);
	void load_dso(std::string filename, v8::Handle<v8::Function> require, v8::Handle<v8::Object> exports, v8::Handle<v8::Object> module);
	v8::Handle<v8::Function> build_require(std::string path, v8::Handle<v8::Value> (*func) (const v8::Arguments&));
	
	v8::Persistent<v8::Array> paths; /* require.paths */
	v8::Local<v8::Object> mainModule;
};

#endif
