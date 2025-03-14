#ifndef VEM_UTILS_HPP
#define VEM_UTILS_HPP

#include <array>
#include <tuple>
#include <vector>
#include <opm/grid/polyhedralgrid.hh>
#ifdef HAVE_ALUGRID
#include <dune/alugrid/grid.hh>
#endif
#include <opm/geomech/boundaryutils.hh>
namespace vem
{
    using PolyGrid = Dune::PolyhedralGrid<3, 3>;
    #ifdef HAVE_ALUGRID
    using AluGrid3D = Dune::ALUGrid<3, 3, Dune::cube, Dune::nonconforming >;
    #endif
    void getGridVectors(const PolyGrid& grid, std::vector<double>& coords,
                std::vector<int>& num_cell_faces,
                std::vector<int>& num_face_corners,
                std::vector<int>& face_corners)
{
   static constexpr int dim = PolyGrid::dimension;
    using namespace std;
    using namespace Dune;
    const auto& gv = grid.leafGridView();
    const int comp = 3+(dim-2)*3;
    static const int bfunc = 4+(dim-2)*4;
    //int loadcase = -1;
    //Dune::FieldVector<ctype,comp> eps0 = {1, 1, 1, 0, 0, 0};
    //eps0 = 0;

    // start VEM assembly
    // make global point coordinate vector
    //vector<double> coords;
    for (const auto& v : vertices(gv)) {
        const auto c = v.geometry().corner(0);
        coords.insert(coords.end(), c.begin(), c.end());
    }

  const int num_cells = gv.size(0); // entities of codim 0
  const auto& ixset = gv.indexSet();
  // count cell faces
  //vector<int> num_cell_faces;
  for (const auto& c : elements(gv))
    num_cell_faces.push_back(Dune::subEntities(c, Dune::Codim<1>{}).size());
  const int tot_num_cfaces = accumulate(num_cell_faces.begin(), num_cell_faces.end(), 0);

  // count face corners
  //vector<int> num_face_corners;
  for (const auto& c : elements(gv))
    for (const auto& f : Dune::subEntities(c, Dune::Codim<1>{}))
      num_face_corners.push_back(f.geometry().corners());

  // establish all face corners
  //vector<int> face_corners;
  for (const auto& c : elements(gv))
    for (const auto& f: Dune::subEntities(c, Dune::Codim<1>{}))
      for (int i = 0, f_ix = 0;
           f_ix != tot_num_cfaces && i != num_face_corners[f_ix];
           ++i, f_ix += (i == num_face_corners[f_ix]))
        face_corners.push_back(ixset.subIndex(f, i, 3));

  // correct order of nodes in each face, to ensure they are mentioned in
  // clockwise or counterclockwise order. @@ This is a hack that might only work
  // for 4-faces!
  // for (int i = 0, j = 0; i != (int)num_face_corners.size(); j += num_face_corners[i++])
  //   swap(face_corners[j], face_corners[j+1]);

  // body force

  // dirichlet boundary conditions

}
    template<class GridType>
    void getGridVectors(const GridType& grid, std::vector<double>& coords,
                std::vector<int>& num_cell_faces,
                std::vector<int>& num_face_corners,
                std::vector<int>& face_corners)
{
   static constexpr int dim = GridType::dimension;
    using namespace std;
    using namespace Dune;
    const auto& gv = grid.leafGridView();
  const int comp = 3+(dim-2)*3;
  static const int bfunc = 4+(dim-2)*4;

  for (const auto& v : vertices(gv)) {
    const auto c = v.geometry().corner(0);
    coords.insert(coords.end(), c.begin(), c.end());
  }

  const int num_cells = gv.size(0); // entities of codim 0
  const auto& ixset = gv.indexSet();


  for (const auto& cell : elements(gv)){
      num_cell_faces.push_back(6);
      for(int i = 0; i < 6; ++i){
          auto faceDir = Opm::Elasticity::faceToFaceDir(i);
          std::array<int,4> nodes = Opm::Elasticity::faceDirToNodes(faceDir);
          num_face_corners.push_back(nodes.size());
          for(auto nind: nodes){
              auto global_ind = gv.indexSet().subIndex(cell,nind,dim);
              face_corners.push_back(global_ind);
          }
      }
  }
  const int tot_num_cfaces = accumulate(num_cell_faces.begin(), num_cell_faces.end(), 0);
}

}
#endif
