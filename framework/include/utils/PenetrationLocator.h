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

#ifndef PENETRATIONLOCATOR_H
#define PENETRATIONLOCATOR_H

// Moose includes
#include "GeometricSearchInterface.h"

// libmesh includes
#include "libmesh_common.h"
#include "mesh.h"
#include "vector_value.h"
#include "point.h"
#include "fe_type.h"
#include "fe.h"

#include <vector>
#include <map>

//Forward Declarations
class MooseSystem;

class PenetrationLocator : protected GeometricSearchInterface
{
public:

  PenetrationLocator(MooseSystem & moose_system, GeometricSearchData & geom_search_data, Mesh & mesh, unsigned int master, unsigned int slave);
  ~PenetrationLocator();
  void detectPenetration();

  Real penetrationDistance(unsigned int node_id);
  RealVectorValue penetrationNormal(unsigned int node_id);
  
  /**
   * Data structure used to hold penetation information
   */
  class PenetrationInfo 
  {
  public:
    PenetrationInfo(Node * node, Elem * elem, Elem * side, unsigned int side_num, RealVectorValue norm, Real norm_distance, const Point & closest_point, const Point & closest_point_ref);

    PenetrationInfo(const PenetrationInfo & p);

    ~PenetrationInfo();
    Node * _node;
    Elem * _elem;
    Elem * _side;
    unsigned int _side_num;
    RealVectorValue _normal;
    Real _distance;
    Point _closest_point;
    Point _closest_point_ref;
  };

  MooseSystem & _moose_system;

  void findContactPoint(const Elem * master_elem, unsigned int side_num, const Point & slave_point,
                        bool start_with_centroid, Point & contact_ref, Point & contact_phys,
                        Real & distance, RealGradient & normal, bool & contact_point_on_side);

  Real normDistance(const Elem & elem, const Elem & side, const Node & p0, Point & closest_point, RealVectorValue & normal);

  int intersect2D_Segments( Point S1P0, Point S1P1, Point S2P0, Point S2P1, Point* I0, Point* I1 );
  int inSegment(Point P, Point SP0, Point SP1);
  
  Mesh & _mesh;
  unsigned int _master_boundary;
  unsigned int _slave_boundary;

  FEType fe_type;
  FEBase * fe;

  NearestNodeLocator & _nearest_node;

  /**
   * Data structure of nodes and their associated penetration information
   */
  std::map<unsigned int, PenetrationInfo *> _penetration_info;

  std::map<unsigned int, bool> _has_penetrated;
};


#endif //PENETRATIONLOCATOR_H
