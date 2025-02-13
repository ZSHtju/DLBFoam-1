/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | DLBFoam: Dynamic Load Balancing 
   \\    /   O peration     | for fast reactive simulations
    \\  /    A nd           | 
     \\/     M anipulation  | 2020, Aalto University, Finland
-------------------------------------------------------------------------------
License
    This file is part of DLBFoam library, derived from OpenFOAM.

    https://github.com/blttkgl/DLBFoam

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Class
    Foam::pyJacLoadBalancedChemistryModel

Description
    Extends loadBalancedChemistryModel model by adding analytical Jacobian.

SourceFiles
    pyJacLoadBalancedChemistryModel.C

\*---------------------------------------------------------------------------*/

#ifndef pyJacLoadBalancedChemistryModel_H
#define pyJacLoadBalancedChemistryModel_H

#include "clockTime.H"
#include "LoadBalancedChemistryModel.H"

// pyJac dependencies
extern "C" {
    #include "pyJacInclude/chem_utils.h"
    #include "pyJacInclude/dydt.h"
    #include "pyJacInclude/jacob.h"
};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam {

// Forward declaration of classes
class fvMesh;

/*---------------------------------------------------------------------------*\
                      Class pyJacLoadBalancedChemistryModel Declaration
\*---------------------------------------------------------------------------*/

template <class ReactionThermo, class ThermoType>
class pyJacLoadBalancedChemistryModel
    : public LoadBalancedChemistryModel<ReactionThermo, ThermoType> {
    template <class DeltaTType> scalar solve(const DeltaTType& deltaT);

public:
    //- Runtime type information
    TypeName("loadBalanced_pyJac");

    // Constructors

    //- Construct from thermo
    pyJacLoadBalancedChemistryModel(const ReactionThermo& thermo);

    //- Destructor
    virtual ~pyJacLoadBalancedChemistryModel();

    //- Disallow default bitwise copy construction
    pyJacLoadBalancedChemistryModel(const pyJacLoadBalancedChemistryModel&);

    inline virtual label nEqns() const;

    //- Override jacobian and derivatives function from StandardChemistryModel
    virtual void jacobian(const scalar        t,
                          const scalarField&  c,
                          const label         li,
                          scalarField&        dcdt,
                          scalarSquareMatrix& J) const override;

    virtual void derivatives(const scalar       t,
                             const scalarField& c,
                             const label        li,
                             scalarField&       dcdt) const override;

    //- Override Qdot function from StandardChemistryModel for using pyJac's enthalpy of formation
    virtual tmp<volScalarField> Qdot() const override;

private:
    ///
    ///@brief List of enthalpy of formation from pyJac
    ///
    ///
    scalarList sp_enth_form;

    //- Update the reaction rate of cell i
    void
    updateReactionRate(const ChemistrySolution& solution, const label& i);
    
    //- Return the main chemistry variable
    //  (concentration for standard, mass fraction for pyJac)
    scalarField getVariable(const scalarField& concentration, const scalarField& massFraction);

};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
#include "pyJacLoadBalancedChemistryModelI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
#include "pyJacLoadBalancedChemistryModel.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
