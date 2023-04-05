#ifndef OPM_ECLPROBLEM_GEOMECH_MODEL_HH
#define OPM_ECLPROBLEM_GEOMECH_MODEL_HH
#include <opm/models/discretization/common/baseauxiliarymodule.hh>
#include <opm/material/densead/Evaluation.hpp>
#include <opm/material/densead/Math.hpp>
#include <opm/elasticity/elasticity_preconditioners.hpp>
#include <opm/elasticity/elasticity_upscale.hpp>
#include <opm/geomech/elasticity_solver.hpp>
//#include <opm/geomech/ElasticitySolverUpscale.hpp>
namespace Opm{
    template<typename TypeTag>
    class EclGeoMechModel : public BaseAuxiliaryModule<TypeTag>
    {
        //using Parent = BaseAuxiliaryModule<TypeTag>;
        using Simulator = GetPropType<TypeTag, Properties::Simulator>;
        using GlobalEqVector = GetPropType<TypeTag, Properties::GlobalEqVector>;
        using NeighborSet = typename BaseAuxiliaryModule<TypeTag>::NeighborSet;
        using SparseMatrixAdapter = GetPropType<TypeTag, Properties::SparseMatrixAdapter>;
        using GridView = GetPropType<TypeTag, Properties::GridView>;
        using Grid = GetPropType<TypeTag, Properties::Grid>;
        using Scalar = GetPropType<TypeTag, Properties::Scalar>;
        using Evaluation = GetPropType<TypeTag, Properties::Evaluation>;
        using Stencil = GetPropType<TypeTag, Properties::Stencil>;
        using FluidSystem = GetPropType<TypeTag, Properties::FluidSystem>;
        using ElementContext = GetPropType<TypeTag, Properties::ElementContext>;
        enum { waterPhaseIdx = FluidSystem::waterPhaseIdx };
        using Toolbox = MathToolbox<Evaluation>;
    public:
        EclGeoMechModel(Simulator& simulator):
            //           Parent(simulator) 
            simulator_(simulator),
            elacticitysolver_(simulator.vanguard().grid(),
                              1e-14,// tol for matching not needed
                              1e9, // scaleof youngs modulo
                              true)
        {
            //const auto& eclstate = simulator_.vanguard().eclState();                
        };
        //ax model things
        void postSolve(GlobalEqVector& deltaX){
            std::cout << "Geomech dummy PostSolve Aux" << std::endl;
        }
        void addNeighbors(std::vector<NeighborSet>& neighbors) const{
            std::cout << "Geomech add neigbors" << std::endl;
        };
        void applyInitial(){
            std::cout << "Geomech applyInitial" << std::endl;
        };
        unsigned numDofs() const{return 0;};
        void linearize(SparseMatrixAdapter& matrix, GlobalEqVector& residual){
            std::cout << "Geomech Dummy Linearize" << std::endl;
        };

        // model things
        void beginIteration(){
            //Parent::beginIteration();
            std::cout << "Geomech begin iteration" << std::endl;
        }
        void endIteration(){
            //Parent::endIteration();
            std::cout << "Geomech end iteration" << std::endl;
        }
        void beginTimeStep(){
            //Parent::beginIteration();
            std::cout << "Geomech begin iteration" << std::endl;            
        }
        void endTimeStep(){
            //Parent::endIteration();
            std::cout << "Geomech end iteration" << std::endl;
            size_t numDof = simulator_.model().numGridDof();
            const auto& problem = simulator_.problem();
            for(size_t dofIdx=0; dofIdx < numDof; ++dofIdx){    
                const auto& iq = simulator_.model().intensiveQuantities(dofIdx,0);
                const auto& fs = iq.fluidState();
                const auto& press = fs.pressure(waterPhaseIdx);
                pressDiff_[dofIdx] = Toolbox::value(press) - problem.initPressure(dofIdx);
            }
        }
        
        template<class Serializer>
        void serializeOp(Serializer& serializer)
        {
            //serializer(tracerConcentration_);
            //serializer(wellTracerRate_);           
        }

        // used in eclproblemgeomech
        void init(bool restart){
            std::cout << "Geomech init" << std::endl;
            size_t numDof = simulator_.model().numGridDof();
            pressDiff_.resize(numDof);
        };
        double pressureDiff(unsigned dofIx) const{
            return pressDiff_[dofIx];
        }
        void setMaterial(const std::vector<std::shared_ptr<Opm::Elasticity::Material>>& materials){
            elacticitysolver_.setMaterial(materials);
        }
    private:
        Simulator& simulator_;
        //std::vector<double> pressDiff_;
        Dune::BlockVector<Dune::FieldVector<double,1>> pressDiff_;
        //Dune::BlockVector<Dune::FieldVector<double,1> > solution_;
        //Dune::BlockVector<Dune::FieldVector<double,3> > displacement_;
        //Dune::BCRSMatrix<Dune::FieldMatrix<double,1,1> > A_;
        //ElasticitySolver  elasticitysolver_;
        //using PC =  Opm::Elasticity::AMG;
        using AMG = Opm::Elasticity::AMG1< Opm::Elasticity::ILUSmoother >;
        Opm::Elasticity::ElasticitySolver<Grid> elacticitysolver_;
        //Dune::VTKWriter<typename GridType::LeafGridView> vtkwriter(grid.leafGridView());
    };
}



#endif
