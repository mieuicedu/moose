/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*           (c) 2010 Battelle Energy Alliance, LLC             */
/*                   ALL RIGHTS RESERVED                        */
/*                                                              */
/*          Prepared by Battelle Energy Alliance, LLC           */
/*            Under Contract No. DE-AC07-05ID14517              */
/*            With the U. S. Department of Energy               */
/*                                                              */
/*            See COPYRIGHT for full restrictions               */
/****************************************************************/

#ifndef MOOSESYSTEM_H
#define MOOSESYSTEM_H

#include <vector>
#include <list>

//MOOSE includes
#include "Moose.h"  // for THREAD_ID
#include "KernelWarehouse.h"
#include "DGKernelWarehouse.h"
#include "BCWarehouse.h"
#include "AuxWarehouse.h"
#include "MaterialWarehouse.h"
#include "StabilizerWarehouse.h"
#include "InitialConditionWarehouse.h"
#include "DofData.h"
#include "ElementData.h"
#include "FaceData.h"
#include "AuxData.h"
#include "MaterialData.h"
#include "PostprocessorData.h"
#include "PostprocessorWarehouse.h"
#include "FunctionWarehouse.h"
#include "DamperData.h"
#include "DamperWarehouse.h"
#include "EmptyFunction.h"

//libmesh includes
#include "transient_system.h"
#include "dof_map.h"
#include "mesh_base.h"
#include "mesh_refinement.h"
#include "error_estimator.h"
#include "error_vector.h"
#include "node_range.h"

//Forward Declarations
class Material;
class InitialConditionWarehouse;
class Executioner;

namespace libMesh
{
  class EquationSystems;
  class MeshBase;
  template<class T> class NumericVector;
}

/**
 * This class represents one full coupled system of nonlinear equations including any
 * explicit (Aux) equations.
 *
 * You can have multiple MOOSE systems... and (one day) couple them together).
 */
class MooseSystem
{
public:
  MooseSystem();
  MooseSystem(Mesh &mesh);
  virtual ~MooseSystem();

  /**
   * Data Accessors for the various FE datastructures indexed by thread
   */
  //ElementData * getElementData(THREAD_ID tid);
  //FaceData * getFaceData(THREAD_ID tid);
  //AuxData * getAuxData(THREAD_ID tid);

  /**
   * Initialize the system
   */
  void init();

  /**
   * Initialize the Mesh for this MooseSystem and return a reference
   */
  Mesh * initMesh(unsigned int dim);

  /**
   * Initialize the Displaced Mesh for this MooseSystem and return a pointer
   *
   * @param displacements The names of the variables to be used as the displacements in x y z directions.
   */
  Mesh * initDisplacedMesh(std::vector<std::string> displacements);
  
  /**
   * Returns a writable reference to the mesh held wihin this MooseSystem
   */
  Mesh * getMesh(bool skip_full_check=false);

  /**
   * Returns a writable reference to the displaced version of the mesh held wihin this MooseSystem
   */
  Mesh * getDisplacedMesh(bool skip_full_check=false);

  /**
   * Whether or not this MooseSystem has a displaced version of the mesh.
   */
  bool hasDisplacedMesh();

  /**
   * Whether or not this system has dampers.
   */
  bool hasDampers();

  /**
   * Get displacement variables.
   */
  std::vector<std::string> getDisplacementVariables();

  inline unsigned int getDim() { return _dim; }

  /**
   * Initialize the EquationSystems object and add both the nonlinear and auxiliary systems
   * to that object for this MooseSystem
   */
  EquationSystems * initEquationSystems();

  /**
   * Returns a writable reference to the EquationSystems object held within this MooseSystem
   */
  EquationSystems * getEquationSystems();

  /**
   * Returns a writable reference to the displaced EquationSystems object held within this MooseSystem
   */
  EquationSystems * getDisplacedEquationSystems();

  void initExecutioner(Executioner * e);
  
  /**
   * Get the Executioner.
   */
  Executioner & getExecutioner();

  /**
   * Returns a reference to the main nonlinear system in this instance of MooseSystem
   */
  TransientNonlinearImplicitSystem * getNonlinearSystem();

  /**
   * Returns a reference to the auxillary system in this instance of MooseSystem
   */
  TransientExplicitSystem * getAuxSystem();

  /**
   * Returns a reference to the displaced system in this instance of MooseSystem
   */
  ExplicitSystem * getDisplacedSystem();

  /**
   * Checks if we have a variable named 'var_name
   */
  bool hasVariable(const std::string &var_name);

  /**
   * Checks if we have an auxiliary variable named 'var_name
   */
  bool hasAuxVariable(const std::string &var_name);

  unsigned int getVariableNumber(const std::string &var_name);

  unsigned int getAuxVariableNumber(const std::string &var_name);

  /**
   * Get the name of the nonlinear variable (using its number)
   */
  const std::string & getVariableName(unsigned int var_num) const;

  /**
   * Get the name of the auxiliary variable (using its number)
   */
  const std::string & getAuxVariableName(unsigned int var_num) const;

  /**
   * Computes the modified variable number for an auxiliary variable.
   * This is the variable number that Kernels know this variable to operate under.
   *
   * This is necessary because Kernels need unique variable numbers for computing
   * off-diagonal jacobian components.
   */
  unsigned int modifiedAuxVarNum(unsigned int var_num);

  /**
   * Initialize all of the FE datastructures
   */
  void initDataStructures();

  /**
   * Initialize the time stepping scheme
   */
  void initTimeSteppingScheme(Moose::TimeSteppingScheme scheme);

  /**
   * Get the order of used time integration scheme
   */
  Real getTimeSteppingOrder() { return _time_stepping_order; }

  /**
   * Check to see if MooseSystem is in a workable state before accessing data
   */
  void checkValid();

  /**
   * Get the Exodus Reader for this system.
   */
  ExodusII_IO * getExodusReader();

  unsigned int addVariable(const std::string &var, const FEType  &type, const std::set< subdomain_id_type  > *const active_subdomains=NULL);
  unsigned int addVariable(const std::string &var, const Order order=FIRST, const FEFamily=LAGRANGE, const std::set< subdomain_id_type > *const active_subdomains=NULL);

  unsigned int addAuxVariable(const std::string &var, const FEType  &type, const std::set< subdomain_id_type  > *const active_subdomains=NULL);
  unsigned int addAuxVariable(const std::string &var, const Order order=FIRST, const FEFamily=LAGRANGE, const std::set< subdomain_id_type > *const active_subdomains=NULL);

  void addKernel(std::string kernel_name,
                 const std::string & name,
                 InputParameters parameters);

  void addDGKernel(std::string kernel_name,
                   const std::string & name,
                   InputParameters parameters);

  void addBC(std::string bc_name,
             const std::string & name,
             InputParameters parameters);

  void addAuxKernel(std::string aux_name,
                    const std::string & name,
                    InputParameters parameters);

  // FIXME: was AuxKernel::addBC
  void addAuxBC(std::string aux_name,
                const std::string & name,
                InputParameters parameters);

  void addMaterial(std::string mat_name,
                   const std::string & name,
                   InputParameters parameters);

  void addStabilizer(std::string stabilizer_name,
                     const std::string & name,
                     InputParameters parameters);

  // FIXME: var_name should go to 'parameters'?
  void addInitialCondition(std::string ic_name,
                           const std::string & name,
                           InputParameters parameters,
                           std::string var_name);

  void addPostprocessor(std::string pp_name,
                        const std::string & name,
                        InputParameters parameters);

  void addFunction(std::string pp_name,
                   const std::string & name,
                   InputParameters parameters);

  void addDamper(std::string damper_name,
                 const std::string & name,
                 InputParameters parameters);

  /**
   * Computes a block diagonal jacobian for the full system.
   */
  virtual void computeJacobian (const NumericVector<Number>& soln, SparseMatrix<Number>&  jacobian);

  /**
   * Computes one block of the jacobian.
   *
   * @param ivar The block row to compute.
   * @param jvar The block column to compute.
   */
  virtual void computeJacobianBlock (const NumericVector<Number>& soln, SparseMatrix<Number>&  jacobian, System& precond_system, unsigned int ivar, unsigned int jvar);

  /**
   * Computes residual of the full system
   *
   */
  virtual void computeResidual (const NumericVector<Number>& soln, NumericVector<Number>& residual);

  virtual Number initialValue (const Point& p, const Parameters& parameters, const std::string& sys_name, const std::string& var_name);

  virtual Gradient initialGradient (const Point& p, const Parameters& parameters, const std::string& sys_name, const std::string& var_name);

  virtual void initialCondition(EquationSystems& es, const std::string& system_name);

  void reinitKernels(THREAD_ID tid, const NumericVector<Number>& soln, const Elem * elem, DenseVector<Number> * Re, DenseMatrix<Number> * Ke = NULL);

  void reinitDampers(THREAD_ID tid, const NumericVector<Number>& increment);

  void reinitDGKernels(THREAD_ID tid, const NumericVector<Number>& soln, const Elem * elem, const unsigned int side, const Elem * neighbor, DenseVector<Number> * Re, bool reinitKe = false);

  void reinitBCs(THREAD_ID tid, const NumericVector<Number>& soln, const Elem * elem, const unsigned int side, const unsigned int boundary_id);
  void reinitBCs(THREAD_ID tid, const NumericVector<Number>& soln, const Node & node, const unsigned int boundary_id, NumericVector<Number>& residual);

  void reinitAuxKernels(THREAD_ID tid, const NumericVector<Number>& soln, const Node & node);
  void reinitAuxKernels(THREAD_ID tid, const NumericVector<Number>& soln, const Elem & elem);


  virtual void computePostprocessors (const NumericVector<Number>& soln);

  virtual void outputPostprocessors();

  bool needPostprocessorsForResiduals() { return _compute_pps_each_residual_evaluation; }

  void needPostprocessorsForResiduals(bool state) { _compute_pps_each_residual_evaluation = state; }

  /**
   * Returns true if a copy of the residual vector is needed (useful if you are going to be modifying
   * the residual vector based on it's entries and you can't guarantee the order your going to
   * be modifying it in.
   */
  bool needResidualCopy() { return _need_residual_copy; }

  /**
   * Call this if your object is going to need a cpy of the residual vector.
   *
   * The residual vector is copied just after the internal residual fill... and before dirichlet bcs.
   * 
   * Useful if you are going to be modifying the residual vector based on it's entries
   * and you can't guarantee the order your going to be modifying it in.
   */
  void needResidualCopy(bool state) { _need_residual_copy = state; }

  /**
   * Returns true if the solution vector will be serialized before each residual and jacobian evaluation.
   */
  bool needSerializedSolution() { return _serialize_solution; }
      
  /**
   * Call this if your object is going to need a serialized solution vector.
   *
   * This might be necessary if you require extensive usage of off-processor solution values.
   */
  void needSerializedSolution(bool state) { _serialize_solution = state; }

  /**
   * Serialize the solution and auxiliary solution vectors.
   */
  void serializeSolution(const NumericVector<Number>& soln);

  virtual Real computeDamping(const NumericVector<Number>& soln, const NumericVector<Number>& update);

  virtual void subdomainSetup(THREAD_ID tid, unsigned int block_id);

  /**
   * Update materials
   * Transient executioners has to call this at the beginning of tha time step
   */
  void updateMaterials();

  /**
   * Re-Initializes temporal discretization/transient control data.
   */
  void reinitDT();

  /**
   * Copy the old solutions backwards
   */
  void copy_old_solutions();

  /**
   * Re-Initializes Eigenvalue computation
   */
  void reinitEigen();

  void checkSystemsIntegrity();

  virtual void projectSolution(Number fptr(const Point& p,
                                    const Parameters& parameters,
                                    const std::string& sys_name,
                                    const std::string& unknown_name),
                        Gradient gptr(const Point& p,
                                      const Parameters& parameters,
                                      const std::string& sys_name,
                                      const std::string& unknown_name));

  /**
   * Allows specification of per variable scaling factors.
   * The size of the vector MUST be the same as the number of Nonlinear Variables.
   * Should be called after Kernel::init() (because that sets the default scaling).
   * Can be called multiple times to change the scaling.
   * The initial scaling is just 1 for each variable.
   */
  void setVarScaling(std::vector<Real> scaling);

  /**
   * Get access to the active_local_element_range
   * Automatically builds it if it hasn't been initialized.
   */
  ConstElemRange * getActiveLocalElementRange();

  /**
   * Get access to the node_range
   * Automatically builds it if it hasn't been initialized.
   */
  NodeRange * getActiveNodeRange();

  /**
   * Should be called after the mesh has been modified in any way.
   */
  void meshChanged();

  /**
   * Tell MooseSystem to retrieve the dimension from the mesh
   */
  void updateDimension();

  virtual void solve();

  /**
   * Get the EquationSystems params
   */
  Parameters &parameters();

  /**
   * Output the given system to output files.
   */
  void outputSystem(unsigned int t_step, Real time);


  void setPreconditioner(Preconditioner<Real> *pc) { _preconditioner = pc; }

  bool & dontReinitFE();

  void setPrintMeshChanged(bool print_mesh_changed);

  void updateNewtonStep();

  /* Adaptivity interface ---------------------- */

  /**
   * Initialize adaptivity
   *
   * @param steps - Maximum r steps to take
   * @param inital_steps - The number of adaptivity steps to perform using the initial conditions
   */
  void initAdaptivity(unsigned int steps, unsigned int initial_steps = 0);

  unsigned int getInitialAdaptivityStepCount();

  void setErrorEstimator(const std::string &error_estimator_name);

  template<typename T>
  void setAdaptivityParam(const std::string &param_name, const T &param_value);

  void setErrorNorm(SystemNorm &sys_norm);

  void adaptMesh();

  /**
   * Get a reference to the value associated with the postprocessor.
   */
  Real & getPostprocessorValue(const std::string & name);

  /**
   * Get function by name
   * @param name name of the function
   * @return reference to the function
   */
  Function & getFunction(const std::string & name);

protected:
  void sizeEverything();

  virtual void computeResidualInternal (const NumericVector<Number>& soln, NumericVector<Number>& residual);

  void updateAuxVars(const NumericVector<Number>& soln);

  void updateDisplacedMesh(const NumericVector<Number>& soln);

protected:
  std::vector<DofData> _dof_data;
  std::vector<ElementData *> _element_data;
  std::vector<FaceData *> _face_data;
  std::vector<DofData> _neighbor_dof_data;
  std::vector<FaceData *> _neighbor_face_data;
  std::vector<AuxData *> _aux_data;
  std::vector<MaterialData> _material_data;
  std::vector<MaterialData> _bnd_material_data;
  std::vector<MaterialData> _neighbor_material_data;
  std::vector<PostprocessorData> _postprocessor_data;
  std::vector<DamperData *> _damper_data;
  
  DofMap * _dof_map;

  DofMap * _aux_dof_map;

  Executioner *_executioner;
  EquationSystems * _es;
  TransientNonlinearImplicitSystem * _system;
  TransientExplicitSystem * _aux_system;
  EquationSystems * _displaced_es;
  ExplicitSystem * _displaced_system;
  ExplicitSystem * _displaced_aux_system;

  /**
   * The list of blocks for a given variable number
   */
  std::map<unsigned int, std::set<unsigned int> > _var_map;

  /**
   * The list of blocks for a given aux variable number
   */
  std::map<unsigned int, std::set<unsigned int> > _aux_var_map;

  Moose::GeomType _geom_type;
  Mesh * _mesh;
  Mesh * _displaced_mesh;

  std::vector<std::string> _displacements;
  bool _has_displaced_mesh;
  bool _delete_mesh;                            // true if we own the mesh and we are responsible for its destruction
  unsigned int _dim;

  /**
   * Whether or not this system has any Dampers associated with it.
   */
  bool _has_dampers;

  /**
   * Needed for output.
   */
  ExodusII_IO * _ex_out;
  unsigned int _num_files;
  unsigned int _num_in_current_file;
  unsigned int _num_files_displaced;
  unsigned int _num_in_current_file_displaced;


  /**
   * True if we need old Isaac for solving our problems
   */
  bool _need_old_newton;

  NumericVector<Number> * _newton_soln;       /// solution vector for the current newton step
  NumericVector<Number> * _old_newton_soln;   /// solution vector for the previous newton step

  /**
   * TRUE if we need to compute postprocessors before every residual evaluation, otherwise FALSE
   */
  bool _compute_pps_each_residual_evaluation;

  /**
   * TRUE if a copy of the residual is needed during dirichlet BC computation.
   */
  bool _need_residual_copy;
  
  /**
   * TRUE if we need to serialize the solution vector before every residual and jacobian evaluation.
   */
  bool _serialize_solution;

  /**
   * Whether or not the mesh has changed recently.  Useful for doing separate output.
   */
  bool _mesh_changed;

  std::vector<KernelWarehouse> _kernels;
  std::vector<DGKernelWarehouse> _dg_kernels;
  std::vector<BCWarehouse> _bcs;
  std::vector<AuxWarehouse> _auxs;
  std::vector<MaterialWarehouse> _materials;
  std::vector<StabilizerWarehouse> _stabilizers;
  std::vector<InitialConditionWarehouse> _ics;
  std::vector<PostprocessorWarehouse> _pps;
  std::vector<FunctionWarehouse> _functions;
  std::vector<DamperWarehouse> _dampers;

  std::vector<bool> _first;

  /**
   * Whether or not we need to recompute the shape functions for each element.  Should only be true if EVERY element is exactly
   * the same shape.
   *
   * If this is true than the finite element objects will only get reinited _once_!
   * This is only valid if you are using a perfectly regular grid!
   * This can provide a huge speedup... but must be used with care.
   */
  bool _no_fe_reinit;

  /**
   * Preconditioner
   */
  Preconditioner<Real> * _preconditioner;

  /**
   * The ExodusIO Reader to support reading of solutions at element qps
   */
  ExodusII_IO * _exreader;

  bool _is_valid;

  /**
   * A mesh refinement object to be used with Adaptivity.
   */
  MeshRefinement * _mesh_refinement;

  /**
   * Error estimator to be used by the apps.
   */
  ErrorEstimator * _error_estimator;

  /**
   * Error vector for use with the error estimator.
   */
  ErrorVector * _error;

public:
  /**
   * Current time.
   */
  Real _t;

  /**
   * Current dt.
   */
  Real _dt;

  /**
   * Old dt.
   */
  Real _dt_old;

  /**
   * Whether or not the current simulation is transient.
   */
  bool _is_transient;

  /**
   * Whether or not the current simulation is Eigenvalue.
   */
  bool _is_eigenvalue;

  /**
   * Current time step.
   */
  int _t_step;

  /**
   * Coefficients (weights) for the time discretization
   */
  std::vector<Real> _time_weight;

  /**
   * Time stepping
   */
  Moose::TimeSteppingScheme _time_stepping_scheme;

  NumericVector<Number> * _residual_copy;   /// Copy of the residual vector

  NumericVector<Number> * _u_dot_soln;   /// solution vector for the time derivative (u_dot)

  NumericVector<Number> * _res_soln_old; /// residual evaluated at the old time step

  NumericVector<Number> * _du_dot_du_soln;   /// solution vector for the derivative of u_dot

  /**
   * This will be filled up with a full serialization of the solution
   * vector if _serialize_solution is true.
   */
  std::vector<Number> _serialized_solution;

  /**
   * This will be filled up with a full serialization of the auxiliary solution
   * vector if _serialize_solution is true.
   */
  std::vector<Number> _serialized_aux_solution;

  /**
   * Called before each residual evaluation
   */
  void computeTimeDeriv(const NumericVector<Number> & soln);

  /**
   * Called at the beginning of the time step in transient simulations
   */
  void onTimestepBegin();

  /**
   * Called after the residual is assembled
   */
  void finishResidual(NumericVector<Number> & residual);

  /**
   * Maximum quadrature order required by all variables.
   */
  Order _max_quadrature_order;

  /**
   * Scaling factors for each variable.
   */
  std::vector<Real> _scaling_factor;

  bool _auto_scaling;

  std::vector<Real> _manual_scaling;


  bool _print_mesh_changed;

  /**
   * Output related vars
   */
  std::string _file_base;
  std::string _gnuplot_format;
  int _interval;
  bool _exodus_output;
  bool _gmv_output;
  bool _tecplot_output;
  bool _tecplot_binary_output;
  bool _xda_output;
  bool _postprocessor_screen_output;
  bool _postprocessor_csv_output;
  bool _postprocessor_ensight_output;
  bool _postprocessor_exodus_output;
  bool _postprocessor_gnuplot_output;
  bool _print_out_info;
  bool _output_initial;


  // Solver convergence tolerance
  Real _l_abs_step_tol;
  Real _last_rnorm;
  Real _initial_residual;

  /**
   * Convenience zeros.
   */

public:
  MooseArray<Real> _real_zero;
  MooseArray<MooseArray<Real> > _zero;
  MooseArray<MooseArray<RealGradient> > _grad_zero;
  MooseArray<MooseArray<RealTensor> > _second_zero;

  /**
   * Empty function that does nothing but sets an "empty" reference where we do not need the function to do anything
   *
   * NOTE: Should be public.
   */
  EmptyFunction _empty_fn;

protected:
  /**
   * A range for use with TBB.  We do this so that it doesn't have
   * to get rebuilt all the time (which takes time).
   */
  ConstElemRange * _active_local_elem_range;
  NodeRange * _active_node_range;

  /**
   * The order of the time stepping scheme
   */
  Real _time_stepping_order;

public:
  /**
   * A map of all of the current nodes to the elements that they are connected to.
   */
  std::vector<std::vector<unsigned int> > node_to_elem_map;
protected:
  friend class ComputeInternalJacobians;
  friend class ComputeInternalJacobianBlocks;
  friend class ComputeInternalResiduals;
  friend class ComputeInternalPostprocessors;
  friend class GenericExecutionerBlock;
  friend class ComputeInternalDamping;

  friend class PDEBase;
  friend class InitialCondition;
  friend class Kernel;
  friend class AuxKernel;
  friend class DGKernel;
  friend class BoundaryCondition;
  friend class Material;
  friend class Stabilizer;
  friend class Executioner;
  friend class Steady;
  friend class Postprocessor;
  friend class GeneralPostprocessor;
  friend class FunctionNeumannBC;
  friend class Damper;

  friend class QuadraturePointData;
  friend class ElementData;
  friend class FaceData;
  friend class AuxData;
  friend class DamperData;
};

/**
 * Set parameters for adaptivity
 */
template<typename T>
void
MooseSystem::setAdaptivityParam(const std::string &param_name, const T &param_value)
{
  if (param_name == "refine fraction")
  {
    _mesh_refinement->refine_fraction() = param_value;
  }
  else if (param_name == "coarsen fraction")
  {
    _mesh_refinement->coarsen_fraction() = param_value;
  }
  else if (param_name == "max h-level")
  {
    _mesh_refinement->max_h_level() = param_value;
  }
  else
  {
    // TODO: spit out some warning/error
  }
}
  
#endif //MOOSESYSTEM_H
