#include <iostream>

using namespace std;

template<class T, size_t N, size_t M>
struct simple_container {
    T items[N][M];

    T operator()(const std::size_t i, size_t j) const{
      return items[i][j];
    }

    T& operator()(const std::size_t i, size_t j){
      return items[i][j];
    }
};

template<class T>
struct const_mult_container {
    T value;

    const_mult_container(T val) : value(val){};

    T operator()(const std::size_t i, size_t j) const{
      return (i == j) ? value : 0;
    }
};

template<class T>
struct const_sum_container {
    T value;

    const_sum_container(T val) : value(val){};

    T operator()(const std::size_t i, size_t j) const{
      return value;
    }
};

template<class T, class EXPR1, class EXPR2>
struct sum_container {
  EXPR1 expr1;
  EXPR2 expr2;

  sum_container(const EXPR1& a, const EXPR2& b): expr1(a), expr2(b){}

  T operator()(const std::size_t i, size_t j) const{
    return expr1(i,j) + expr2(i,j);
  }
};

template<class T, class EXPR1, class EXPR2>
struct neg_container {
  EXPR1 expr1;
  EXPR2 expr2;

  neg_container(const EXPR1& a, const EXPR2& b): expr1(a), expr2(b){}

  T operator()(const std::size_t i, size_t j) const{
    return expr1(i,j) - expr2(i,j);
  }
};

// M is the matching index of the matrices
template<class T, class EXPR1, class EXPR2, int M>
struct mul_container {
  EXPR1 expr1;
  EXPR2 expr2;

  mul_container(const EXPR1& a, const EXPR2& b): expr1(a), expr2(b){ }

  T operator()(const std::size_t i, size_t k) const{
    return (expr1(i,M-1) * expr2(M-1,k)) + (mul_container<T, EXPR1, EXPR2, M-1>(expr1, expr2))(i,k);
  }
};

template<class T,  class EXPR1, class EXPR2>
struct mul_container <T, EXPR1, EXPR2, 0> {

  mul_container(const EXPR1& a, const EXPR2& b){}

  T operator()(const std::size_t i, size_t k) const{
    return 0;
  }
};

template<class T, size_t N, size_t M, class Cont= simple_container<T, N, M> >
class my_matrix{
  Cont cont;

public:
  my_matrix() {}
  my_matrix(const Cont& other) : cont(other){}

  // assignment operator for MyVector of different type
  template<class T2, class Cont2>
  my_matrix& operator=(const my_matrix<T2, N, M, Cont2>& other){
    for (std::size_t i = 0; i < N; ++i)
      for (size_t j = 0; j < M; j++)
      {
        cont(i,j) = other(i,j);
      }
    return *this;
  }

  // index operators
  T operator()(const std::size_t i, size_t j) const{
    return cont(i,j);
  }

  T& operator()(const std::size_t i, size_t j) {
    return cont(i,j);
  }

  // returns the underlying data
  const Cont& data() const{
    return cont;
  }

  Cont& data(){
    return cont;
  }

  template<class Other_cont>
  my_matrix<T, N, M, sum_container<T, Cont, Other_cont> > operator+(const my_matrix<T, N, M, Other_cont>& other){
    return my_matrix<T, N, M, sum_container<T, Cont, Other_cont> > (sum_container<T, Cont, Other_cont>(this->data(), other.data()));
  }

  template<class Other_cont>
  my_matrix<T, N, M, sum_container<T, Cont, Other_cont> > operator+(const my_matrix<T, N, M, Other_cont>& other) const{
    return my_matrix<T, N, M, sum_container<T, Cont, Other_cont> > (sum_container<T, Cont, Other_cont>(this->data(), other.data()));
  }

  my_matrix<T, N, M, sum_container<T, Cont, const_sum_container<T> > > operator+(const T& other) {
    return this->operator+(my_matrix<T, N, M, const_sum_container<T> >(const_sum_container<T>(other)));
  }

  my_matrix<T, N, M, sum_container<T, Cont, const_sum_container<T> > > operator+(const T& other) const {
    return this->operator+(my_matrix<T, N, M, const_sum_container<T> >(const_sum_container<T>(other)));
  }

  friend my_matrix<T, N, M, sum_container<T, Cont, const_sum_container<T> > > operator+(const T c, const my_matrix<T, N, M, Cont>& mtx) {
    return mtx.operator+(c);
  }

  template<class Other_cont>
  my_matrix<T, N, M, neg_container<T, Cont, Other_cont> > operator-(const my_matrix<T, N, M, Other_cont>& other){
    return my_matrix<T, N, M, neg_container<T, Cont, Other_cont> > (neg_container<T, Cont, Other_cont>(this->data(), other.data()));
  }

  template<class Other_cont>
  my_matrix<T, N, M, neg_container<T, Cont, Other_cont> > operator-(const my_matrix<T, N, M, Other_cont>& other) const {
    return my_matrix<T, N, M, neg_container<T, Cont, Other_cont> > (neg_container<T, Cont, Other_cont>(this->data(), other.data()));
  }

  my_matrix<T, N, M, neg_container<T, Cont, const_sum_container<T> > > operator-(const T other){
    return this->operator-(my_matrix<T, N, M, const_sum_container<T> >(const_sum_container<T>(other)));
  }

  template<size_t K, class Other_cont>
  my_matrix<T, N, K, mul_container<T, Cont, Other_cont, M> > operator*(const my_matrix<T, M, K, Other_cont>& other){
    return my_matrix<T, N, K, mul_container<T, Cont, Other_cont, M> > (mul_container<T, Cont, Other_cont, M>(this->data(), other.data()));
  }

  my_matrix<T, N, M, mul_container<T, Cont, const_mult_container<T>, M> > operator*(const T other){
    return this->operator*(my_matrix<T, N, M, const_mult_container<T> >(const_mult_container<T>(other)));
  }
};

int main() {
    my_matrix<int,2,2> mtx1;
    my_matrix<int,2,2> mtx2;
    my_matrix<int,2,2> mtx3;
    my_matrix<int,2,2> mtx4;
    my_matrix<int,1,1> mtx5;

    for (size_t i=0; i<2; i++) {
        for (size_t j=0; j<2; j++) {
            mtx1(i,j) = 1;
            mtx2(i,j) = 2;
            mtx3(i,j) = 3;
        }
    }


    for (size_t i=0; i<2; i++) {
        for (size_t j=0; j<2; j++) {
            cout << mtx1(i,j) << "\t";
        }
        cout << endl;
      }
      cout << endl;

      for (size_t i=0; i<2; i++) {
          for (size_t j=0; j<2; j++) {
              cout << mtx2(i,j) << "\t";
          }
          cout << endl;
        }
        cout << endl;

        mtx1 = 10 + mtx1;

        for (size_t i=0; i<2; i++) {
            for (size_t j=0; j<2; j++) {
                cout << mtx1(i,j) << "\t";
            }
            cout << endl;
          }
          cout << endl;

          for (size_t i=0; i<2; i++) {
              for (size_t j=0; j<2; j++) {
                  cout << mtx2(i,j) << "\t";
              }
              cout << endl;
            }
            cout << endl;
    /*mtx1 = mtx1*3;

    for (size_t i=0; i<2; i++) {
        for (size_t j=0; j<3; j++) {
            mtx5(i,j) = 1;
        }
    }

    mtx4 = mtx1 + mtx2;

    for (size_t i=0; i<2; i++) {
        for (size_t j=0; j<3; j++) {
            cout << mtx4(i,j) << endl;
        }
    }*/

    return 0;
}
