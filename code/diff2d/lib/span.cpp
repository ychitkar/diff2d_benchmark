/****************************************************************
 ****
 **** This file belongs with the course
 **** Parallel Programming in MPI and OpenMP
 **** copyright 2019-2024 Victor Eijkhout eijkhout@tacc.utexas.edu
 ****
 **** linalg.cpp : bordered vector routines using mdspan
 ****
 ****************************************************************/

#include <cmath>
#include <string>
using std::string;

#include <iostream>
using std::cout;
#include <format>
using std::format;

#include "omp.h"
#include "span.hpp"

namespace linalg {

  //! Compute the 5-point Laplace stencil from an input array
  //codesnippet d2d5ptrng
  template< typename real >
  void bordered_array_span<real>::central_difference_from
      ( const linalg::bordered_array_base<real>& _other,bool trace ) {
    const auto& other = dynamic_cast<const linalg::bordered_array_span<real>&>(_other);
    auto out = this->data2d();
    auto in = other.data2d();
    #pragma omp parallel for 
    for ( auto ij : this->inner() ) {
      auto [i,j] = ij;
      out[ i,j ] = 4*in[ i,j ]
	- in[ i-1,j ] - in[ i+1,j ] - in[ i,j-1 ] - in[ i,j+1 ];
    }
  };
  //codesnippet end

  //! Scale the interior, leaving the border alone
  //codesnippet d2dscalerng
  template< typename real >
  void bordered_array_span<real>::scale_interior
      ( const linalg::bordered_array_base<real>& _other, real factor ) {
    const auto& other = dynamic_cast<const linalg::bordered_array_span<real>&>(_other);
    auto out = this->data2d();
    auto in = other.data2d();
#   pragma omp parallel for
    for ( auto ij : this->inner() ) {
      auto [i,j] = ij;
      out[ i,j ] = in[ i,j] * factor;
    }
  };
  //codesnippet end

  //! Compute the L2 norm of the interior
  //codesnippet d2dnormrng
  template< typename real >
  real bordered_array_span<real>::l2norm() {
    real sum_of_squares{0};
    auto array = this->data2d();
#   pragma omp parallel for reduction(+:sum_of_squares)
    for ( auto ij : this->inner() ) {
      auto [i,j] = ij;
      auto v = array[i,j];
      sum_of_squares += v*v;
    }
    return std::sqrt(sum_of_squares);
  };
  //codesnippet end

  //! Set the interior to a value
  template< typename real >
  void bordered_array_span<real>::set( real value, bool trace ) {
    auto out = this->data2d();
    for ( auto ij : this->inner() ) {
      auto [i,j] = ij;
      out[i,j] = value;
    }
  };

  template< typename real >
  void bordered_array_span<real>::set_bc( bool down,bool right, bool trace ) {
    auto out = this->data2d();
    auto m = this->m(), n = this->n(), border = this->border(), n2b = this->n2b();
#   pragma omp parallel for 
    for ( int i=0; i<m; i++ )
      for ( int j=0; j<n; j++ )
	if ( i==m-1 or j==n-1 )
	  out[ i,j ] = 1.;
  };

  template< typename real >
  void bordered_array_span<real>::view( string caption ) {
    if (caption!="")
      cout << format("{}:\n",caption);
    auto out = this->data();
    auto m = this->m(), n = this->n(), border = this->border(), n2b = this->n2b();
    for ( size_t i=0; i<m+2*border; i++ ) {
      for ( size_t j=0; j<n+2*border; j++ ) {
        char c = ( j<n+2*border-1 ? ' ' : '\n' );
        std::cout << std::format("{:5.2}{}",out[ this->oindex(i,j) ],c);
      }
    }
    // auto out = this->data2d();
    // auto m = this->m(), n = this->n(), border = this->border(), n2b = this->n2b();
    // for ( int i=0; i<m; i++ )
    //   for ( int j=0; j<n; j++ )
    // 	if ( i==m-1 or j==n-1 ) {
    //       char c = ( j<n2b-1 ? ' ' : '\n' );
    //       cout << format("{:5.2}{}",out[i,j],c);
    //     }
  };
};

namespace linalg {
  template class bordered_array_span<float>;
  template class bordered_array_span<double>;
};
