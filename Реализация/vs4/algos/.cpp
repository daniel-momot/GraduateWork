#include "application.h"
#include <logger.h>
#include <System/garbage.h>
#include <Drivers/app_arg_parser.h>
#include <Algebra/Grobner/F4/f4_involutive.h>
#include <Algebra/Grobner/XL/xl_involutive.h>

//////////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER

static class __init__ {
public: __init__() {
			//_CrtSetBreakAlloc(13016);
			int tmpFlag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );
			_CrtSetDbgFlag( tmpFlag | _CRTDBG_LEAK_CHECK_DF);
		}
public: ~__init__() {}
} gl_init;

#endif

//////////////////////////////////////////////////////////////////////////////////

application * g_self_application = nullptr;

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

application::application(t_int argc, const t_char* argv[])
{
	// Это необходимо для отключения кэширования вывода в консоли Linux
	std::cout << std::unitbuf;

	if (gl_garbage == nullptr)
		gl_garbage = new garbage();

	app_arg_parser ap;
	ap.parse(argc, argv, &m_app_args);

	m_app_args.dump_params();

	_ASSERT(g_self_application == nullptr);
	g_self_application = this;

	if (!_initialize())
		throw CPRExcept("Application initialization error!");

	start();
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

application::~application(void)
{
	_ASSERT(g_self_application == this);
	g_self_application = nullptr;

	stop();

	_clear();

	if (gl_garbage != nullptr)
		delete gl_garbage;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

t_void application::run()
{
	_print_polynoms("", "_original");

	// подстановка заданных во внешнем файле констант
	_substitute_idents();

	// поиск базиса, построение графа
	_define_core();

	//_experimental();

	// переупорядочивание переменных в зависимости от размещения в графе
	_print_collections("core_x", "");
	_lex_reordering();
	_print_collections("core_z", "");

	// подстановка базисных переменных по уравнения связи и по чекерам
	_lead_basis();

	// выполнение инволютивных алгоритмов
	_xl_involutive();
	_f4_involutive();

	// финальный вывод информации
	_print_collections("final", "");

	// Полная зачистка
	_clear();
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

t_bool application::_initialize()
{
	load_equations();

	return _true;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

t_void application::_clear()
{
	hold_delete(m_boolean_spaces);
	m_boolean_spaces.clear();
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

t_void application::load_equations()
{
	const std::vector<t_string> * paths = m_app_args.get_values(APP_PARAM_KEY_PATHEQUATION);
	if (paths == nullptr)
		return;

	for(size_t i = 0; i < paths->size(); ++i) {
		std::unique_ptr<boolean_space> space(new boolean_space());
		
		space->init((*paths)[i].c_str());

		m_boolean_spaces.push_back(space.release());
	}

	return _void;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

t_void application::_substitute_idents()
{
	const std::vector<t_string> * paths = m_app_args.get_values(APP_PARAM_KEY_PATHSOLUTION);
	if (paths == nullptr)
		return;

	if (paths->size() != m_boolean_spaces.size())
		throw CPRExcept("The file with the solution does not match any file with equations!");

	for(size_t i = 0; i < paths->size(); ++i) {
		boolean_space * space = m_boolean_spaces[i];

		space->get_identities()->read((*paths)[i].c_str());
		space->substitute_idents();
	}
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

t_void application::_define_core()
{
	const std::vector<t_string> * paths = m_app_args.get_values(APP_PARAM_KEY_PATHBASIS);

	for(size_t i = 0; i < m_boolean_spaces.size(); ++i) {
		if (paths != nullptr && i < paths->size())
			m_boolean_spaces[i]->build_core((*paths)[i].c_str());
		else
			m_boolean_spaces[i]->build_core();
	}

	return _void;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

t_void application::_lex_reordering()
{
	if (m_app_args.is_key_contained(APP_PARAM_KEY_DISABLEREORDER))
		return _void;

	for(size_t i = 0; i < m_boolean_spaces.size(); ++i) {
		m_boolean_spaces[i]->lex_reorder_variables();
	}

	return _void;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

t_void application::_f4_involutive()
{
	if (!m_app_args.is_key_contained(APP_PARAM_KEY_ENABLEF4INVOLUTIVE))
		return _void;

	throw CPRExcept("Feature f4-involutive not supported!");

	for(size_t i = 0; i < m_boolean_spaces.size(); ++i) {
		f4_involutive f4(m_boolean_spaces[i]);
		f4.run();
	}

	return _void;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

t_void application::_xl_involutive()
{
	if (!m_app_args.is_key_contained(APP_PARAM_KEY_ENABLEXLINVOLUTIVE))
		return _void;

	try {
		for(size_t i = 0; i < m_boolean_spaces.size(); ++i) {
			xl_involutive xl(m_boolean_spaces[i]);
			xl.run();
		}
	}
	catch(std::exception & e) {
		std::cerr << "ERROR: " << e.what();
	}

	return _void;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

t_void application::_lead_basis()
{
	if (!m_app_args.is_key_contained(APP_PARAM_KEY_ENABLELEAD))
		return _void;

	t_long max_count_mons = m_app_args.get_long_simple_value(APP_PARAM_KEY_ENABLELEAD, LONG_MAX);

	for(size_t i = 0; i < m_boolean_spaces.size(); ++i) {
		boolean_collections new_collection = m_boolean_spaces[i]->get_core()->lead_basis([&max_count_mons](tp_variable var, tp_polynom poly)->t_bool {
			_ASSERT(!poly->is_need_simplify());
			return (poly->get_count_monomials() < max_count_mons);
		});

		polynoms_storage new_storage = new_collection.build_storage_polynoms();

		new_collection.dump("lead_basis", "");
		new_storage.dump("lead_basis", "");

		new_collection.hard_clear();
	}

	return _void;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

t_void application::_print_polynoms(const t_char * sub_directory, const t_char * path_suffix) const
{
	for(size_t i = 0; i < m_boolean_spaces.size(); ++i) {
		if (m_boolean_spaces[i]->get_core()->is_empty()) {
			m_boolean_spaces[i]->get_polynoms_storage()->dump(sub_directory, path_suffix);
			continue;
		}
		m_boolean_spaces[i]->get_core()->build_storage_polynoms().dump(sub_directory, path_suffix);
	}

	return _void;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

t_void application::_print_collections(const t_char * sub_directory, const t_char * path_suffix) const
{
	for(size_t i = 0; i < m_boolean_spaces.size(); ++i) {
		m_boolean_spaces[i]->get_variables_storage()->dump(sub_directory, path_suffix);
		m_boolean_spaces[i]->get_core()->dump(sub_directory, path_suffix);

		if (m_boolean_spaces[i]->get_core()->is_empty()) {
			m_boolean_spaces[i]->get_polynoms_storage()->dump(sub_directory, path_suffix);
			continue;
		}
		m_boolean_spaces[i]->get_core()->build_storage_polynoms().dump(sub_directory, path_suffix);
	}

	if (path_suffix == nullptr || *path_suffix == '\0')
		_dump(t_string(sub_directory).append("/dump").c_str(), "");
	else
		_dump(t_string(sub_directory).append(1, '_').append(path_suffix).append("/dump").c_str(), "");

	return _void;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

t_void application::_dump(const t_char * sub_directory, const t_char * path_suffix) const
{
	if (!m_app_args.is_key_contained(APP_PARAM_KEY_ENABLEDUMP))
		return _void;

	for(size_t i = 0; i < m_boolean_spaces.size(); ++i) {
		m_boolean_spaces[i]->dump(sub_directory, path_suffix);
	}

	return _void;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
