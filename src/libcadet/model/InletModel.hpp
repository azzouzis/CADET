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

/**
 * @file 
 * Defines the inlet model which encapsulates an inlet profile as unit operation.
 */

#ifndef LIBCADET_INLETMODEL_HPP_
#define LIBCADET_INLETMODEL_HPP_

#include "cadet/SolutionExporter.hpp"

#include "UnitOperation.hpp"
#include "AutoDiff.hpp"
#include "ParamIdUtil.hpp"
#include "model/ModelUtils.hpp"

#include <array>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <tuple>

namespace cadet
{

class IInletProfile;

namespace model
{

/**
 * @brief Inlet model which encapsulates an IInletProfile as unit operation
 * @details This unit operation model does not possess DOFs. Its values (inlet profile)
 *          is directly injected into other models' DOFs.
 */
class InletModel : public IUnitOperation
{
public:

	InletModel(UnitOpIdx unitOpIdx);
	virtual ~InletModel() CADET_NOEXCEPT;

	virtual unsigned int numDofs() const CADET_NOEXCEPT;
	virtual unsigned int numPureDofs() const CADET_NOEXCEPT;
	virtual bool usesAD() const CADET_NOEXCEPT;
	virtual unsigned int requiredADdirs() const CADET_NOEXCEPT;

	virtual UnitOpIdx unitOperationId() const CADET_NOEXCEPT { return _unitOpIdx; }
	virtual unsigned int numComponents() const CADET_NOEXCEPT { return _nComp; }
	virtual void setFlowRates(const active& in, const active& out) CADET_NOEXCEPT { }
	virtual bool canAccumulate() const CADET_NOEXCEPT { return true; }

	static const char* identifier() { return "INLET"; }
	virtual const char* unitOperationName() const CADET_NOEXCEPT { return "INLET"; }

	virtual bool configure(IParameterProvider& paramProvider, IConfigHelper& helper);
	virtual bool reconfigure(IParameterProvider& paramProvider);
	virtual void notifyDiscontinuousSectionTransition(double t, unsigned int secIdx, active* const adRes, active* const adY, unsigned int adDirOffset);
	
	virtual std::unordered_map<ParameterId, double> getAllParameterValues() const;
	virtual bool hasParameter(const ParameterId& pId) const;

	virtual bool setParameter(const ParameterId& pId, int value);
	virtual bool setParameter(const ParameterId& pId, double value);
	virtual bool setParameter(const ParameterId& pId, bool value);

	virtual bool setSensitiveParameter(const ParameterId& pId, unsigned int adDirection, double adValue);
	virtual void setSensitiveParameterValue(const ParameterId& id, double value);

	virtual void clearSensParams();

	virtual void useAnalyticJacobian(const bool analyticJac);

	virtual void reportSolution(ISolutionRecorder& recorder, double const* const solution) const;
	virtual void reportSolutionStructure(ISolutionRecorder& recorder) const;

	virtual int residual(double t, unsigned int secIdx, double timeFactor, double const* const y, double const* const yDot, double* const res);
	virtual int residualWithJacobian(const active& t, unsigned int secIdx, const active& timeFactor, double const* const y, double const* const yDot, double* const res, active* const adRes, active* const adY, unsigned int adDirOffset);

	virtual int residualSensFwdAdOnly(const active& t, unsigned int secIdx, const active& timeFactor,
		double const* const y, double const* const yDot, active* const adRes);

	virtual int residualSensFwdCombine(const active& timeFactor, const std::vector<const double*>& yS, const std::vector<const double*>& ySdot,
		const std::vector<double*>& resS, active const* adRes, double* const tmp1, double* const tmp2, double* const tmp3);

	virtual int residualSensFwdWithJacobian(const active& t, unsigned int secIdx, const active& timeFactor, double const* const y, double const* const yDot, active* const adRes, active* const adY, unsigned int adDirOffset);

	// linearSolve is a null operation (the result is I^-1 *rhs -> rhs) since the Jacobian is an identity matrix
	virtual int linearSolve(double t, double timeFactor, double alpha, double tol, double* const rhs, double const* const weight,
		double const* const y, double const* const yDot, double const* const res) { return 0; }

	virtual void prepareADvectors(active* const adRes, active* const adY, unsigned int adDirOffset) const;

	virtual void applyInitialCondition(double* const vecStateY, double* const vecStateYdot);
	virtual void applyInitialCondition(IParameterProvider& paramProvider, double* const vecStateY, double* const vecStateYdot);

	virtual void consistentInitialState(double t, unsigned int secIdx, double timeFactor, double* const vecStateY, active* const adRes, active* const adY, unsigned int adDirOffset, double errorTol);
	virtual void consistentInitialTimeDerivative(double t, unsigned int secIdx, double timeFactor, double const* vecStateY, double* const vecStateYdot);

	virtual void consistentInitialSensitivity(const active& t, unsigned int secIdx, const active& timeFactor, double const* vecStateY, double const* vecStateYdot,
		std::vector<double*>& vecSensY, std::vector<double*>& vecSensYdot, active const* const adRes);

	virtual void leanConsistentInitialState(double t, unsigned int secIdx, double timeFactor, double* const vecStateY, active* const adRes, active* const adY, unsigned int adDirOffset, double errorTol);
	virtual void leanConsistentInitialTimeDerivative(double t, double timeFactor, double* const vecStateYdot, double* const res);

	virtual void leanConsistentInitialSensitivity(const active& t, unsigned int secIdx, const active& timeFactor, double const* vecStateY, double const* vecStateYdot,
		std::vector<double*>& vecSensY, std::vector<double*>& vecSensYdot, active const* const adRes);

	virtual void setExternalFunctions(IExternalFunction** extFuns, unsigned int size) { }

	virtual void multiplyWithJacobian(double const* yS, double alpha, double beta, double* ret);
	virtual void multiplyWithDerivativeJacobian(double const* sDot, double* ret, double timeFactor);
	virtual inline void multiplyWithJacobian(double const* yS, double* ret)
	{
		multiplyWithJacobian(yS, 1.0, 0.0, ret);
	}

	virtual bool hasInlet() const CADET_NOEXCEPT { return false; }
	virtual bool hasOutlet() const CADET_NOEXCEPT { return true; }

	virtual unsigned int localOutletComponentIndex() const CADET_NOEXCEPT { return 0; }
	virtual unsigned int localOutletComponentStride() const CADET_NOEXCEPT { return 1; }
	virtual unsigned int localInletComponentIndex() const CADET_NOEXCEPT { return 0; }
	virtual unsigned int localInletComponentStride() const CADET_NOEXCEPT { return 0; }

	virtual void setSectionTimes(double const* secTimes, bool const* secContinuity, unsigned int nSections);

	virtual void expandErrorTol(double const* errorSpec, unsigned int errorSpecSize, double* expandOut) { }

#ifdef CADET_BENCHMARK_MODE
	virtual std::vector<double> benchmarkTimings() const { return std::vector<double>(0); }
	virtual char const* const* benchmarkDescriptions() const { return nullptr; }
#endif

protected:

	template <typename T> T const* moveInletValues(double const* const rawValues, const active& t, unsigned int secIdx) const;

	template <typename T> T const* const getData() const;

	template <typename ResidualType, typename ParamType>
	int residualImpl(const ParamType& t, unsigned int secIdx, const ParamType& timeFactor, double const* const y, double const* const yDot, ResidualType* const res);

	UnitOpIdx _unitOpIdx; //!< Unit operation index
	unsigned int _nComp; //!< Number of components
	std::vector<double> _tempState; // Temporary storage for the state vector

	IInletProfile* _inlet; //!< Inlet profile (owned by library user)

	double* _inletConcentrationsRaw; //!< Provides memory for getting the inlet concentrations from the inlet profile
	double* _inletDerivatives; //!< Points to memory used for retrieving the derivatives of the inlet profile (do not delete since the memory is owned by _inletConcentrationsRaw)
	active* _inletConcentrations; //!< Provides memory for the inlet concentrations

	std::unordered_map<ParameterId, std::tuple<unsigned int, double>> _sensParamsInlet; //!< Maps an inlet parameter to its AD direction and derivative value

	class Exporter : public ISolutionExporter
	{
	public:

		Exporter(unsigned int nComp, double const* data) : _data(data), _nComp(nComp) { }

		virtual bool hasMultipleBoundStates() const CADET_NOEXCEPT { return false; }
		virtual bool hasNonBindingComponents() const CADET_NOEXCEPT { return true; }
		virtual bool hasParticleFlux() const CADET_NOEXCEPT { return false; }
		virtual bool hasParticleMobilePhase() const CADET_NOEXCEPT { return false; }

		virtual unsigned int numComponents() const CADET_NOEXCEPT { return _nComp; }
		virtual unsigned int numAxialCells() const CADET_NOEXCEPT { return 1; }
		virtual unsigned int numRadialCells() const CADET_NOEXCEPT { return 0; }
		virtual unsigned int numBoundStates() const CADET_NOEXCEPT { return 0; }
		virtual unsigned int const* numBoundStatesPerComponent() const CADET_NOEXCEPT { return nullptr; }
		virtual unsigned int numBoundStates(unsigned int comp) const CADET_NOEXCEPT { return 0; }
		virtual unsigned int numColumnDofs() const CADET_NOEXCEPT { return _nComp; }
		virtual unsigned int numParticleDofs() const CADET_NOEXCEPT { return 0; }
		virtual unsigned int numFluxDofs() const CADET_NOEXCEPT { return 0; }
		
		virtual double concentration(unsigned int component, unsigned int axialCell) const { return _data[component]; }
		virtual double flux(unsigned int component, unsigned int axialCell) const { return 0.0; }
		virtual double mobilePhase(unsigned int component, unsigned int axialCell, unsigned int radialCell) const { return 0.0; }
		virtual double solidPhase(unsigned int component, unsigned int axialCell, unsigned int radialCell, unsigned int boundState) const { return 0.0; }
		
		virtual double const* concentration() const { return _data; }
		virtual double const* flux() const { return nullptr; }
		virtual double const* mobilePhase() const { return nullptr; }
		virtual double const* solidPhase() const { return nullptr; }
		virtual double const* inlet(unsigned int& stride) const
		{
			stride = 1;
			return _data;
		}
		virtual double const* outlet(unsigned int& stride) const
		{
			stride = 1;
			return _data;
		}

		virtual StateOrdering const* concentrationOrdering(unsigned int& len) const
		{
			len = _concentrationOrdering.size();
			return _concentrationOrdering.data();
		}

		virtual StateOrdering const* fluxOrdering(unsigned int& len) const
		{
			len = 0;
			return nullptr;
		}

		virtual StateOrdering const* mobilePhaseOrdering(unsigned int& len) const
		{
			len = 0;
			return nullptr;
		}

		virtual StateOrdering const* solidPhaseOrdering(unsigned int& len) const
		{
			len = 0;
			return nullptr;
		}

	protected:
		double const* const _data;
		unsigned int _nComp;

		const std::array<StateOrdering, 1> _concentrationOrdering = { { StateOrdering::Component } };
	};
};

} // namespace model
} // namespace cadet

#endif  // LIBCADET_INLETMODEL_HPP_
