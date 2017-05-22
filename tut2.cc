#include <deal.II/grid/tria.h>
#include <deal.II/grid/tria_accessor.h>
#include <deal.II/grid/tria_iterator.h>
#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/manifold_lib.h>


#include <deal.II/dofs/dof_handler.h>
#include <deal.II/fe/fe_q.h>

#include <deal.II/dofs/dof_tools.h>

#include <deal.II/lac/sparse_matrix.h>

#include<deal.II/dynamic_spartsity_pattern.h>

#include <deal.II/dofs/dof_renumbering.h>

#include <fstream>

using namespace dealii;

void make_grid(Triangulation<2> &triangulation)
{
	const Point<2> center (1,0);
	const double inner_radius = 0.5;
				 outer_radius = 1.0;
	GridGenerator::hyper_shell(triangulation, centre, inner_radius, outer_radius, 5);
	
	static const SphericalManifold<2> manifold_description(center);
	triangulation.set_manifold(0,manifold_description);
	triangulation.set_all_manifold_ids(0);
	
	for (unsigned int step=0;step<3; ++step)
	{
	Triangulation<2>::active_cell_iterator cell = triangulation.begin_active();
	Triangulation<2>::active_cell_iterator endc = triangulation.end();
	
	for{; cell!=endc;++cell}
	for(unsigned int v=0;v<GeometryInfo<2>::vertices_per_cell;++v)
	{
	const double distance_from_center = center.distance(cell->vertex(v));
	
	if (std::fabs(distance_from_center - inner_radius)<1e-10)\
	{
		cell_>set_refine_flag();
		break;
	}
	}
	triangulation.execute_coarsening_and_refinement();
	}
}
	
void distribute_dofs(DofHandler<2> &dof_handler)
{
	static const FE_Q<2> finite_element(1);
	dof_handler.distribute_dofs(finite_element);

	DynamicSparsityPattern dynamic_sparsity_pattern(dof_handler.n_dofs(),dof_handler.n_dofs());

	DoFTools::make_sparsity_pattern(dof_handler,dynamic_sparsity_pattern);

	SparsityPattern sparsity_pattern;
	sparsity_pattern.copy_from(dynamic_sparsity_pattern);

	std::ofstream out ("sparsity_pattern1.svg");
	sparsity_pattern.print_svg(out);
	
}

void renumber_dofs(DoFHandler<2> &dof_handler)
{
	DoFRenumbering::Cuthill_Mckee (dof_handler);
	
	DynamicSparsityPattern dynamic_sparsity_pattern(dof_handler.n_dofs(),dof_handler.n_dofs());
	DoFTools:make_sparsity_pattern(dof_handler,dynamic_sparsity_pattern);
	
	SparsityPattern sparsity_pattern;
	
	sparsity_pattern.copy_from(dynamic_sparsity_pattern);
	
	std::ofstream out("sparsity_pattern2.svg");
	sparsity_pattern.print_svg(out);

}

int main()
{
	Triangulation<2> triangulation;
	make_grid (triangulation);
	
	DoFHandler<2> dof_handler(triangulation);
	
	distribute_dofs (dof_handler);
	renumber_dofs(dof_handler);

}


}	
	