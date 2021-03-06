// =============================================================================
//  CADET - The Chromatography Analysis and Design Toolkit
//  
//  Copyright © 2008-2017: The CADET Authors
//            Please see the AUTHORS and CONTRIBUTORS file.
//  
//  All rights reserved. This program and the accompanying materials
//  are made available under the terms of the GNU Public License v3.0 (or, at
//  your option, any later version) which accompanies this distribution, and
//  is available at http://www.gnu.org/licenses/gpl.html
// =============================================================================

#include <json.hpp>

#define CADETTEST_JSONPARAMETERPROVIDER_NOFORWARD
#include "JsonParameterProvider.hpp"

using json = nlohmann::json;

namespace cadet
{

JsonParameterProvider::JsonParameterProvider(const char* data) : _root(new json(json::parse(data)))
{
	_opened.push(_root);
}

JsonParameterProvider::JsonParameterProvider(const std::string& data) : _root(new json(json::parse(data)))
{
	_opened.push(_root);
}

JsonParameterProvider::JsonParameterProvider(const json& data) : _root(new json(data))
{
	_opened.push(_root);
}

JsonParameterProvider::JsonParameterProvider(const JsonParameterProvider& cpy)
{
	_root = new json(*cpy._root);
	_opened = cpy._opened;
}

JsonParameterProvider::JsonParameterProvider(JsonParameterProvider&& cpy) CADET_NOEXCEPT : _root(cpy._root), _opened(std::move(cpy._opened))
{
	cpy._root = nullptr;
	cpy._opened = std::stack<json*>();
}

JsonParameterProvider::~JsonParameterProvider() CADET_NOEXCEPT
{
	delete _root;
}

JsonParameterProvider& JsonParameterProvider::operator=(const JsonParameterProvider& cpy)
{
	delete _root;

	_root = new json(*cpy._root);
	_opened = cpy._opened;
	return *this;
}

#ifdef COMPILER_SUPPORT_NOEXCEPT_DEFAULTED_MOVE
	JsonParameterProvider& JsonParameterProvider::operator=(JsonParameterProvider&& cpy) CADET_NOEXCEPT
#else
	JsonParameterProvider& JsonParameterProvider::operator=(JsonParameterProvider&& cpy)
#endif
{
	delete _root;
	_opened = std::stack<nlohmann::json*>();
	_root = cpy._root;
	cpy._root = nullptr;
	cpy._opened = std::stack<nlohmann::json*>();

	_opened.push(_root);
	return *this;
}

double JsonParameterProvider::getDouble(const std::string& paramName)
{
	return _opened.top()->at(paramName).get<double>();
}

int JsonParameterProvider::getInt(const std::string& paramName)
{
	const json p = _opened.top()->at(paramName);
	if (p.is_boolean())
		return p.get<bool>();
	else
		return p.get<int>();
}

uint64_t JsonParameterProvider::getUint64(const std::string& paramName)
{
	return _opened.top()->at(paramName).get<uint64_t>();
}

bool JsonParameterProvider::getBool(const std::string& paramName)
{
	const json p = _opened.top()->at(paramName);
	if (p.is_number_integer())
		return p.get<int>();
	else
		return p.get<bool>();
}

std::string JsonParameterProvider::getString(const std::string& paramName)
{
	return _opened.top()->at(paramName).get<std::string>();
}

std::vector<double> JsonParameterProvider::getDoubleArray(const std::string& paramName)
{
	return _opened.top()->at(paramName).get<std::vector<double>>();
}

std::vector<int> JsonParameterProvider::getIntArray(const std::string& paramName)
{
	return _opened.top()->at(paramName).get<std::vector<int>>();
}

std::vector<uint64_t> JsonParameterProvider::getUint64Array(const std::string& paramName)
{
	return _opened.top()->at(paramName).get<std::vector<uint64_t>>();
}

std::vector<bool> JsonParameterProvider::getBoolArray(const std::string& paramName)
{
	return _opened.top()->at(paramName);
}

std::vector<std::string> JsonParameterProvider::getStringArray(const std::string& paramName)
{
	return _opened.top()->at(paramName).get<std::vector<std::string>>();
}

bool JsonParameterProvider::exists(const std::string& paramName)
{
	return _opened.top()->find(paramName) != _opened.top()->end();
}

bool JsonParameterProvider::isArray(const std::string& paramName)
{
	return _opened.top()->at(paramName).is_array();
}

void JsonParameterProvider::pushScope(const std::string& scope)
{
	_opened.push(&_opened.top()->at(scope));
}

void JsonParameterProvider::popScope()
{
	_opened.pop();
}

void JsonParameterProvider::set(const std::string& paramName, double val)
{
	(*_opened.top())[paramName] = val;
}

void JsonParameterProvider::set(const std::string& paramName, int val)
{
	(*_opened.top())[paramName] = val;
}

void JsonParameterProvider::set(const std::string& paramName, uint64_t val)
{
	(*_opened.top())[paramName] = val;
}

void JsonParameterProvider::set(const std::string& paramName, bool val)
{
	(*_opened.top())[paramName] = val;
}

void JsonParameterProvider::set(const std::string& paramName, const std::string& val)
{
	(*_opened.top())[paramName] = val;
}

void JsonParameterProvider::set(const std::string& paramName, const std::vector<double>& val)
{
	(*_opened.top())[paramName] = val;
}

void JsonParameterProvider::set(const std::string& paramName, const std::vector<int>& val)
{
	(*_opened.top())[paramName] = val;
}

void JsonParameterProvider::set(const std::string& paramName, const std::vector<uint64_t>& val)
{
	(*_opened.top())[paramName] = val;
}

void JsonParameterProvider::set(const std::string& paramName, const std::vector<std::string>& val)
{
	(*_opened.top())[paramName] = val;
}

std::ostream& operator<<(std::ostream& out, const JsonParameterProvider& jpp)
{
	out << (*jpp.data());
	return out;
}

} // namespace cadet

json createGRMwithSMAJson()
{
	json config;
	config["UNIT_TYPE"] = std::string("GENERAL_RATE_MODEL");
	config["NCOMP"] = 4;
	config["VELOCITY"] = 5.75e-4;
	config["COL_DISPERSION"] = 5.75e-8;
	config["FILM_DIFFUSION"] = {6.9e-6, 6.9e-6, 6.9e-6, 6.9e-6};
	config["PAR_DIFFUSION"] = {7e-10, 6.07e-11, 6.07e-11, 6.07e-11};
	config["PAR_SURFDIFFUSION"] = {0.0, 0.0, 0.0, 0.0};

	// Geometry
	config["COL_LENGTH"] = 0.014;
	config["PAR_RADIUS"] = 4.5e-5;
	config["COL_POROSITY"] = 0.37;
	config["PAR_POROSITY"] = 0.75;

	// Initial conditions
	config["INIT_C"] = {50.0, 0.0, 0.0, 0.0};
	config["INIT_Q"] = {1.2e3, 0.0, 0.0, 0.0};

	// Adsorption
	config["ADSORPTION_MODEL"] = std::string("STERIC_MASS_ACTION");
	{
		json ads;
		ads["IS_KINETIC"] = 1;
		ads["SMA_LAMBDA"] = 1.2e3;
		ads["SMA_KA"] = {0.0, 35.5, 1.59, 7.7};
		ads["SMA_KD"] = {0.0, 1000.0, 1000.0, 1000.0};
		ads["SMA_NU"] = {0.0, 4.7, 5.29, 3.7};
		ads["SMA_SIGMA"] = {0.0, 11.83, 10.6, 10.0};
		config["adsorption"] = ads;
	}

	// Discretization
	{
		json disc;

		disc["NCOL"] = 16;
		disc["NPAR"] = 4;
		disc["NBOUND"] = {1, 1, 1, 1};

		disc["PAR_DISC_TYPE"] = std::string("EQUIDISTANT_PAR");

		disc["USE_ANALYTIC_JACOBIAN"] = true;
		disc["MAX_KRYLOV"] = 0;
		disc["GS_TYPE"] = 1;
		disc["MAX_RESTARTS"] = 10;
		disc["SCHUR_SAFETY"] = 1e-8;

		// WENO
		{
			json weno;

			weno["WENO_ORDER"] = 3;
			weno["BOUNDARY_MODEL"] = 0;
			weno["WENO_EPS"] = 1e-10;
			disc["weno"] = weno;
		}
		config["discretization"] = disc;
	}

	return config;

/*
	return R"json({
	"UNIT_TYPE": "GENERAL_RATE_MODEL",
	"NCOMP": 4,
	"VELOCITY": 5.75e-4,
	"COL_DISPERSION": 5.75e-8,
	"FILM_DIFFUSION": [6.9e-6, 6.9e-6, 6.9e-6, 6.9e-6],
	"PAR_DIFFUSION": [7e-10, 6.07e-11, 6.07e-11, 6.07e-11],
	"PAR_SURFDIFFUSION": [0.0, 0.0, 0.0, 0.0],
	"COL_LENGTH": 0.014,
	"PAR_RADIUS": 4.5e-5,
	"COL_POROSITY": 0.37,
	"PAR_POROSITY": 0.75,
	"INIT_C": [50.0, 0.0, 0.0, 0.0],
	"INIT_Q": [1.2e3, 0.0, 0.0, 0.0],
	"ADSORPTION_MODEL": "STERIC_MASS_ACTION",
	"adsorption": 
	{
		"IS_KINETIC": 1,
		"SMA_LAMBDA": 1.2e3,
		"SMA_KA": [0.0, 35.5, 1.59, 7.7],
		"SMA_KD": [0.0, 1000.0, 1000.0, 1000.0],
		"SMA_NU": [0.0, 4.7, 5.29, 3.7],
		"SMA_SIGMA": [0.0, 11.83, 10.6, 10.0]
	},
	"discretization":
	{
		"NCOL": 16,
		"NPAR": 4,
		"NBOUND": [1, 1, 1, 1],
		"PAR_DISC_TYPE": "EQUIDISTANT_PAR",
		"USE_ANALYTIC_JACOBIAN": true,
		"MAX_KRYLOV": 0,
		"GS_TYPE": 1,
		"MAX_RESTARTS": 10,
		"SCHUR_SAFETY": 1e-8,
		"weno":
		{
			"WENO_ORDER": 3,
			"BOUNDARY_MODEL": 0,
			"WENO_EPS": 1e-10
		}
	}
	})json";
*/
}

cadet::JsonParameterProvider createGRMwithSMA()
{
	return cadet::JsonParameterProvider(createGRMwithSMAJson());
}

json createGRMwithLinearJson()
{
	json config;
	config["UNIT_TYPE"] = std::string("GENERAL_RATE_MODEL");
	config["NCOMP"] = 2;
	config["VELOCITY"] = 5.75e-4;
	config["COL_DISPERSION"] = 5.75e-8;
	config["FILM_DIFFUSION"] = {6.9e-6, 6.9e-6};
	config["PAR_DIFFUSION"] = {7e-10, 6.07e-11};
	config["PAR_SURFDIFFUSION"] = {1e-10, 1e-10};

	// Geometry
	config["COL_LENGTH"] = 0.014;
	config["PAR_RADIUS"] = 4.5e-5;
	config["COL_POROSITY"] = 0.37;
	config["PAR_POROSITY"] = 0.75;

	// Initial conditions
	config["INIT_C"] = {1.0, 2.0, 3.0};
	config["INIT_Q"] = {5.0, 6.0, 7.0};

	// Adsorption
	config["ADSORPTION_MODEL"] = std::string("LINEAR");
	{
		json ads;
		ads["IS_KINETIC"] = 1;
		ads["LIN_KA"] = {12.3, 35.5, 1.59};
		ads["LIN_KD"] = {45.0, 20.0, 10.0};
		config["adsorption"] = ads;
	}

	// Discretization
	{
		json disc;

		disc["NCOL"] = 15;
		disc["NPAR"] = 5;
		disc["NBOUND"] = {1, 1, 1};

		disc["PAR_DISC_TYPE"] = std::string("EQUIDISTANT_PAR");

		disc["USE_ANALYTIC_JACOBIAN"] = true;
		disc["MAX_KRYLOV"] = 0;
		disc["GS_TYPE"] = 1;
		disc["MAX_RESTARTS"] = 10;
		disc["SCHUR_SAFETY"] = 1e-8;

		// WENO
		{
			json weno;

			weno["WENO_ORDER"] = 3;
			weno["BOUNDARY_MODEL"] = 0;
			weno["WENO_EPS"] = 1e-10;
			disc["weno"] = weno;
		}
		config["discretization"] = disc;
	}

	return config;
}

cadet::JsonParameterProvider createGRMwithLinear()
{
	return cadet::JsonParameterProvider(createGRMwithLinearJson());
}

json createLWEJson()
{
	json config;	
	// Model
	{
		json model;
		model["NUNITS"] = 2;
		model["unit_000"] = createGRMwithSMAJson();

		// Inlet - unit 001
		{
			json inlet;

			inlet["UNIT_TYPE"] = std::string("INLET");
			inlet["INLET_TYPE"] = std::string("PIECEWISE_CUBIC_POLY");
			inlet["NCOMP"] = 4;

			{
				json sec;

				sec["CONST_COEFF"] = {50.0, 1.0, 1.0, 1.0};
				sec["LIN_COEFF"] = {0.0, 0.0, 0.0, 0.0};
				sec["QUAD_COEFF"] = {0.0, 0.0, 0.0, 0.0};
				sec["CUBE_COEFF"] = {0.0, 0.0, 0.0, 0.0};

				inlet["sec_000"] = sec;
			}

			{
				json sec;

				sec["CONST_COEFF"] = {50.0, 0.0, 0.0, 0.0};
				sec["LIN_COEFF"] = {0.0, 0.0, 0.0, 0.0};
				sec["QUAD_COEFF"] = {0.0, 0.0, 0.0, 0.0};
				sec["CUBE_COEFF"] = {0.0, 0.0, 0.0, 0.0};

				inlet["sec_001"] = sec;
			}

			{
				json sec;

				sec["CONST_COEFF"] = {100.0, 0.0, 0.0, 0.0};
				sec["LIN_COEFF"] = {0.2, 0.0, 0.0, 0.0};
				sec["QUAD_COEFF"] = {0.0, 0.0, 0.0, 0.0};
				sec["CUBE_COEFF"] = {0.0, 0.0, 0.0, 0.0};

				inlet["sec_002"] = sec;
			}

			model["unit_001"] = inlet;
		}

		// Valve switches
		{
			json con;
			con["NSWITCHES"] = 1;

			{
				json sw;

				// This switch occurs at beginning of section 0 (initial configuration)
				sw["SECTION"] = 0;

				// Connection list is 1x5 since we have 1 connection between
				// the two unit operations (and we need to have 5 columns)
				sw["CONNECTIONS"] = {1.0, 0.0, -1.0, -1.0, 1.0};
				// Connections: From unit operation 1 to unit operation 0,
				//              connect component -1 (i.e., all components)
				//              to component -1 (i.e., all components) with
				//              volumetric flow rate 1.0 m^3/s

				con["switch_000"] = sw;
			}
			model["connections"] = con;
		}

		// Solver settings
		{
			json solver;

			solver["MAX_KRYLOV"] = 0;
			solver["GS_TYPE"] = 1;
			solver["MAX_RESTARTS"] = 10;
			solver["SCHUR_SAFETY"] = 1e-8;
			model["solver"] = solver;
		}

		config["model"] = model;
	}

	// Return
	{
		json ret;
		ret["WRITE_SOLUTION_TIMES"] = true;
	
		json grm;
		grm["WRITE_SOLUTION_COLUMN"] = false;
		grm["WRITE_SOLUTION_PARTICLE"] = false;
		grm["WRITE_SOLUTION_FLUX"] = false;
		grm["WRITE_SOLUTION_COLUMN_INLET"] = true;
		grm["WRITE_SOLUTION_COLUMN_OUTLET"] = true;
		
		ret["unit_000"] = grm;
		config["return"] = ret;
	}

	// Solver
	{
		json solver;

		{
			std::vector<double> solTimes;
			solTimes.reserve(1501);
			for (double t = 0.0; t <= 1500.0; t += 1.0)
				solTimes.push_back(t);

			solver["USER_SOLUTION_TIMES"] = solTimes;
		}

		solver["NTHREADS"] = 1;

		// Sections
		{
			json sec;

			sec["NSEC"] = 3;
			sec["SECTION_TIMES"] = {0.0, 10.0, 90.0, 1500.0};
			sec["SECTION_CONTINUITY"] = {false, false};

			solver["sections"] = sec;
		}

		// Time integrator
		{
			json ti;

			ti["ABSTOL"] = 1e-8;
			ti["RELTOL"] = 1e-6;
			ti["ALGTOL"] = 1e-12;
			ti["INIT_STEP_SIZE"] = 1e-6;
			ti["MAX_STEPS"] = 10000;

			solver["time_integrator"] = ti;
		}

		config["solver"] = solver;
	}
	return config;
}

cadet::JsonParameterProvider createLWE()
{
	return cadet::JsonParameterProvider(createLWEJson());
}

cadet::JsonParameterProvider createLinearBenchmark(bool dynamicBinding)
{
	json config;	
	// Model
	{
		json model;
		model["NUNITS"] = 2;

		// GRM - unit 000
		{
			json grm;
			grm["UNIT_TYPE"] = std::string("GENERAL_RATE_MODEL");
			grm["NCOMP"] = 1;
			grm["VELOCITY"] = 0.5 / (100.0 * 60.0);
			grm["COL_DISPERSION"] = 0.002 / (100.0 * 100.0 * 60.0);
			grm["FILM_DIFFUSION"] = {0.01 / (100.0 * 60.0)};
			grm["PAR_DIFFUSION"] = {3.003e-6};
			grm["PAR_SURFDIFFUSION"] = {0.0};

			// Geometry
			grm["COL_LENGTH"] = 0.017;
			grm["PAR_RADIUS"] = 4e-5;
			grm["COL_POROSITY"] = 0.4;
			grm["PAR_POROSITY"] = 0.333;

			// Initial conditions
			grm["INIT_C"] = {0.0};
			grm["INIT_Q"] = {0.0};

			// Adsorption
			grm["ADSORPTION_MODEL"] = std::string("LINEAR");
			{
				json ads;
				ads["IS_KINETIC"] = (dynamicBinding ? 1 : 0);
				ads["LIN_KA"] = {2.5};
				ads["LIN_KD"] = {1.0};
				grm["adsorption"] = ads;
			}

			// Discretization
			{
				json disc;

				disc["NCOL"] = 512;
				disc["NPAR"] = 4;
				disc["NBOUND"] = {1};

				disc["PAR_DISC_TYPE"] = std::string("EQUIDISTANT_PAR");

				disc["USE_ANALYTIC_JACOBIAN"] = true;
				disc["MAX_KRYLOV"] = 0;
				disc["GS_TYPE"] = 1;
				disc["MAX_RESTARTS"] = 10;
				disc["SCHUR_SAFETY"] = 1e-8;

				// WENO
				{
					json weno;

					weno["WENO_ORDER"] = 3;
					weno["BOUNDARY_MODEL"] = 0;
					weno["WENO_EPS"] = 1e-10;
					disc["weno"] = weno;
				}
				grm["discretization"] = disc;
			}

			model["unit_000"] = grm;
		}

		// Inlet - unit 001
		{
			json inlet;

			inlet["UNIT_TYPE"] = std::string("INLET");
			inlet["INLET_TYPE"] = std::string("PIECEWISE_CUBIC_POLY");
			inlet["NCOMP"] = 1;

			{
				json sec;

				sec["CONST_COEFF"] = {1.0};
				sec["LIN_COEFF"] = {0.0};
				sec["QUAD_COEFF"] = {0.0};
				sec["CUBE_COEFF"] = {0.0};

				inlet["sec_000"] = sec;
			}

			{
				json sec;

				sec["CONST_COEFF"] = {0.0};
				sec["LIN_COEFF"] = {0.0};
				sec["QUAD_COEFF"] = {0.0};
				sec["CUBE_COEFF"] = {0.0};

				inlet["sec_001"] = sec;
			}

			model["unit_001"] = inlet;
		}

		// Valve switches
		{
			json con;
			con["NSWITCHES"] = 1;

			{
				json sw;

				// This switch occurs at beginning of section 0 (initial configuration)
				sw["SECTION"] = 0;

				// Connection list is 1x5 since we have 1 connection between
				// the two unit operations (and we need to have 5 columns)
				sw["CONNECTIONS"] = {1.0, 0.0, -1.0, -1.0, 1.0};
				// Connections: From unit operation 1 to unit operation 0,
				//              connect component -1 (i.e., all components)
				//              to component -1 (i.e., all components) with
				//              volumetric flow rate 1.0 m^3/s

				con["switch_000"] = sw;
			}
			model["connections"] = con;
		}

		// Solver settings
		{
			json solver;

			solver["MAX_KRYLOV"] = 0;
			solver["GS_TYPE"] = 1;
			solver["MAX_RESTARTS"] = 10;
			solver["SCHUR_SAFETY"] = 1e-8;
			model["solver"] = solver;
		}

		config["model"] = model;
	}

	// Return
	{
		json ret;
		ret["WRITE_SOLUTION_TIMES"] = true;
	
		json grm;
		grm["WRITE_SOLUTION_COLUMN"] = false;
		grm["WRITE_SOLUTION_PARTICLE"] = false;
		grm["WRITE_SOLUTION_FLUX"] = false;
		grm["WRITE_SOLUTION_COLUMN_INLET"] = true;
		grm["WRITE_SOLUTION_COLUMN_OUTLET"] = true;
		
		ret["unit_000"] = grm;
		config["return"] = ret;
	}

	// Solver
	{
		json solver;

		{
			std::vector<double> solTimes;
			solTimes.reserve(3001);
			for (double t = 0.0; t <= 100.0 * 60.0; t += 2.0)
				solTimes.push_back(t);

			solver["USER_SOLUTION_TIMES"] = solTimes;
		}

		solver["NTHREADS"] = 1;

		// Sections
		{
			json sec;

			sec["NSEC"] = 2;
			sec["SECTION_TIMES"] = {0.0, 20.0 * 60.0, 100.0 * 60.0};
			sec["SECTION_CONTINUITY"] = {false, false};

			solver["sections"] = sec;
		}

		// Time integrator
		{
			json ti;

			ti["ABSTOL"] = 1e-8;
			ti["RELTOL"] = 1e-6;
			ti["ALGTOL"] = 1e-12;
			ti["INIT_STEP_SIZE"] = 1e-6;
			ti["MAX_STEPS"] = 10000;

			solver["time_integrator"] = ti;
		}

		config["solver"] = solver;
	}
	return cadet::JsonParameterProvider(config);
}
