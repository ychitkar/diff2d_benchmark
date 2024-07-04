/****************************************************************
 ****
 **** This file belongs with the course
 **** Parallel Programming in MPI and OpenMP
 **** copyright 2019-2024 Victor Eijkhout eijkhout@tacc.utexas.edu
 ****
 **** diy2e.hpp : based on span.hpp but my own cartesian rangee
 ****
 ****************************************************************/

#ifndef LINALG_DIY2E_H
#define LINALG_DIY2E_H

#include "base.hpp"

namespace linalg {

  //codesnippet d2ebordered
  template< typename real >
  class bordered_array_diy2e : public bordered_array_base<real> {
    friend class distributed_array<real>;
  public:
    using bordered_array_base<real>::log_flops;
    using bordered_array_base<real>::log_bytes;
    //codesnippet end

    // constructors
    bordered_array_diy2e( int64_t m,int64_t n,int border );
    bordered_array_diy2e( int64_t m,int64_t n,real *data )
      : bordered_array_base<real>(m,n,data) {};

    void central_difference_from( const linalg::bordered_array_base<real>&,bool=false ) override;
    void scale_interior( const linalg::bordered_array_base<real>&, real ) override;
    real l2norm() override;
    void set( real value,bool trace=false ) override;
    void set_bc(bool down, bool right, bool trace=false) override;
    void view( std::string="" ) override;

    /*
     * Iterator class
     */
    class cartesian_product_diy {
    private: 
      std::int64_t m, n; int b;
    public:
      class cartesian_iterator {
      private:
        std::int64_t m, n; int b; /* global domain description */
        std::int64_t i{0}, j{0};  /* local iteration point */
	std::int64_t c{-1};
      public:
        cartesian_iterator\
            ( std::int64_t m, std::int64_t n, int b, std::int64_t i, std::int64_t j )
              : m(m),n(n),b(b),i(i),j(j) { c=m; };
        bool operator==( const cartesian_iterator& other ) const {
          return this->j==other.j and this->i==other.i; };
        auto operator*() const {
          return std::make_pair( (i+b), (j+b) ); };
        // auto& operator++(  ) {
	//   std::cout << i << "," << j << "pp " << '\n';
        //   j++; i+= (j/m); j = j%m; return *this; };
        //codesnippet d2ediyiter
        auto& operator++(  ) {
	  c--;
          j++; j *= (c>0);
	  i += (c==0);
	  c += m*(c==0);
	  return *this;
	};
        //codesnippet end
        auto operator+( std::int64_t dist ) const {
          auto displaced(*this);
          auto lin = ( i*m+j ) + dist;
          displaced.i = lin/m; displaced.j = lin%m; 
          return displaced;
        };
        // gcc needs +=, intel only +
        // auto& operator+=( std::int64_t dist ) {
	//   std::cout << dist << " " << '\n';
	//   auto lin = ( i*m+j ) + dist;
	//   i = lin/m; j = lin%m; 
        //   return *this;
        // };
        std::int64_t operator-( const cartesian_iterator& other ) const {
          return (i-other.i)*m + (j-other.j); }
      };
      cartesian_product_diy( std::int64_t m, std::int64_t n,int b ) 
        : m(m),n(n),b(b) {};
      auto begin() {
        return cartesian_iterator(m,n,b, /* start */ 0,0); };
      auto end()   {
        return cartesian_iterator(m,n,b, /* end   */ m,0); };
    };
    auto inner_diy() { 
      return cartesian_product_diy(this->m(),this->n(),this->border());
    };
  };
};

#endif

#if 0
        //codesnippet d2ediyiter
        auto& operator++(  ) {
	  c--;
          j++; j *= (c>0);
	  i += (c==0);
	  c += m*(c==0);
	  return *this;
	};
        //codesnippet end
#endif

#if 0
	  if ( dist==1 ) {
	    j++; auto t = (j==m); j *= t; i+=t;
	  } else if (dist==m) {
	    i++; j *= (i<n);
	  } else {
	    auto lin = ( i*m+j ) + dist;
	    i = lin/m; j = lin%m; 
	  }
	  // attmempt1
	  // while( dist>=m ) { i++; dist -= m; }
	  // j += dist;
	  // if (j>m) { i++; j -= m; }
	  // if (i==n) j=0;
#endif

