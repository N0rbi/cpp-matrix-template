#include <iostream>
class matrix_base {};

/**
A simple container tartalmazza a memoriaba lementett matrix ertekeket.
*/
template<class T, int N, int M>
struct simple_container {
    /**
    T tipusu tarolo tomb
    */
    T items[N][M];

    /**
    visszaadunk egy konstans referenciat a tarolo (i,j). elemere
    */
    const T& operator()(int i,  int j) const{
      return items[i][j];
    }

    /**
    visszaadunk egy referenciat a tarolo (i,j). elemere, ekkor valtoztathato
    az erteke
    */
    T& operator()(int i, int j){
      return items[i][j];
    }
};

/**
const_mult_container konstans szorzashoz
*/
template<class T, class EXPR1>
struct const_mult_container {
    /**
    szorzo
    */
    T value;
    /**
    szorzando matrix
    */
    EXPR1 expr1;

    /**
    konstruktor
    */
    const_mult_container(T val, EXPR1 expr1) : value(val), expr1(expr1){};

    /**
    a matrix osszes elemet beszorozzuk a value-val
    */
    const T operator()(int i, int j) const{
      return expr1(i,j) * value;
    }
};

// template<class T>
// struct const_sum_container {
//     T value;
//
//     const_sum_container(T val) : value(val){};
//
//     T operator()(int i, int j) const{
//       return value;
//     }
// };

/**
sum_container ket azonos metertu matrixot var, virtualisan osszeadva oket.
*/
template<class T, class EXPR1, class EXPR2>
struct sum_container {
  /**
  bal oldali operator
  */
  EXPR1 expr1;
  /**
  jobb oldali operator
  */
  EXPR2 expr2;

  /**
  konstruktor
  */
  sum_container(const EXPR1& a, const EXPR2& b): expr1(a), expr2(b){
  }

  /**
  a ket parameterul kapott matrix (i,j). elem osszeget adja vissza
  */
  const T operator()(int i, int j) const{
    return expr1(i,j) + expr2(i,j);
  }
};

/**
neg_container hasonló a sum_container-hez, csak kivonassal
*/
template<class T, class EXPR1, class EXPR2>
struct neg_container {
  /**
  bal oldali operator
  */
  EXPR1 expr1;
  /**
  jobb oldali operator
  */
  EXPR2 expr2;

  /**
  konstruktor
  */
  neg_container(const EXPR1& a, const EXPR2& b): expr1(a), expr2(b){}

  /**
  az (i,j) parameterre visszaadja a ket parameterul kapott matrix
  (i,j). elemeinek a kulonbseget
  */
  const T operator()(int i, int j) const{
    return expr1(i,j) - expr2(i,j);
  }
};

/**
mul_container egy N*M es M*K matrixok virtualis osszeszorzasara alkalmas
*/
template<class T, class EXPR1, class EXPR2, int M>
struct mul_container {
  /**
  bal oldali operator
  */
  EXPR1 expr1;
  /**
  jobb oldali operator
  */
  EXPR2 expr2;

  /**
  konstruktor
  */
  mul_container(const EXPR1& a, const EXPR2& b): expr1(a), expr2(b){
  }
  /**
  rekurziv fuggveny matrix szorzasra, az (i,k). elem megadja a definicio
  szerinti erteket a ket matrix szorzatanak
  a rekurzio alapja a ket matrix "kapcsolodasi dimenzioja"
  */
  const T operator()(int i, int k) const{
    return (expr1(i,M-1) * expr2(M-1,k)) + (mul_container<T, EXPR1, EXPR2, M-1>(expr1, expr2))(i,k);
  }
};

/**
a mul_container specializacioja, ha a "kapcsolodo dimenzio" 0 akkor az (i, k).
elem sem lehet kulonbozo 0-nal.
*/
template<class T,  class EXPR1, class EXPR2>
struct mul_container <T, EXPR1, EXPR2, 0> {

  /**
  konstruktor
  */
  mul_container(const EXPR1& a, const EXPR2& b){}

  /**
  leszallasi feltetel matrix szorzashoz
  */
  const T operator()(int i, int k) const{
    return 0;
  }
};

/**
my_matrix az alap matrix osztaly, mely a fent talalhato segely osztalyokat
hasznalja fel kontenerkent
*/
template<class T, int N, int M, class Cont= simple_container<T, N, M> >
class my_matrix : matrix_base {
  /**
  kontener az adott matrixnak, lehet virtualis vagy fizikai is
  */
  Cont cont;

public:
  /**
  alapertelemezett konstruktor
  */
  my_matrix() {}
  /**
  konstruktor virtualizalt elemekhez, itt nincs szükség másolásra, hiszen ha
  ha fel van szabadítva egy objektum nem hívunk meg rá operációkat
  */
  my_matrix(const Cont& other) : cont(other){}

  /**
  ertekadas operator a simple_container-ekkel elvegzi a virtualis szamitasokat,
  egeszen az adott "other" objektum kontenereig, majd beallirja ertekul az
  aktualis objektumnak
  */
  template<class Cont2>
  my_matrix& operator=(const my_matrix<T, N, M, Cont2>& other){
    for (int i = 0; i < N; ++i)
      for (int j = 0; j < M; j++)
      {
        cont(i,j) = other.getByValue(i,j);
      }
    return *this;
  }

  /**
  (i,j). elem konstans referencia szerinti lekerese
  */
  const T& operator()(int i, int j) const{
    return this->cont(i,j);
  }

  /**
  (i,j). elem referencia szerinti lekerese
  */
  T& operator()(int i, int j) {
    return this->cont(i,j);
  }

  /**
  az (i,j). elemet érték szerint kéri le, nem referencia szerint
  */
  T getByValue(int i, int j) const {
    return cont(i,j);
  }

  /**
  visszaadja a kontener konstans referenciajat
  */
  const Cont& data() const {
    return cont;
  }

  /**
  visszaadja a kontener referenciajat
  */
  Cont& data(){
    return cont;
  }

  /**
  virtualis osszeadas matrixok kozott
  */
  template<class Other_cont>
  my_matrix<T, N, M, sum_container<T, Cont, Other_cont> > operator+(const my_matrix<T, N, M, Other_cont>& other){
    return my_matrix<T, N, M, sum_container<T, Cont, Other_cont> > (sum_container<T, Cont, Other_cont>(this->data(), other.data()));
  }

  /**
  virtualis osszeadas matrixok kozott (konstans)
  */
  template<class Other_cont>
  my_matrix<T, N, M, sum_container<T, Cont, Other_cont> > operator+(const my_matrix<T, N, M, Other_cont>& other) const{
    return my_matrix<T, N, M, sum_container<T, Cont, Other_cont> > (sum_container<T, Cont, Other_cont>(this->data(), other.data()));
  }

  /**
  virtualis kivonas matrixok kozott
  */
  template<class Other_cont>
  my_matrix<T, N, M, neg_container<T, Cont, Other_cont> > operator-(const my_matrix<T, N, M, Other_cont>& other){
    return my_matrix<T, N, M, neg_container<T, Cont, Other_cont> > (neg_container<T, Cont, Other_cont>(this->data(), other.data()));
  }

  /**
  virtualis kivonas matrixok kozott (konstans)
  */
  template<class Other_cont>
  my_matrix<T, N, M, neg_container<T, Cont, Other_cont> > operator-(const my_matrix<T, N, M, Other_cont>& other) const {
    return my_matrix<T, N, M, neg_container<T, Cont, Other_cont> > (neg_container<T, Cont, Other_cont>(this->data(), other.data()));
  }

  /**
  virtualis szorzas matrixok kozott
  */
  template<int K, class Other_cont>
  my_matrix<T, N, K, mul_container<T, Cont, Other_cont, M> > operator*(const my_matrix<T, M, K, Other_cont>& other){
    return my_matrix<T, N, K, mul_container<T, Cont, Other_cont, M> > (mul_container<T, Cont, Other_cont, M>(this->data(), other.data()));
  }

  /**
  virtualis szorzas matrixok kozott (konstans)
  */
  template<int K, class Other_cont>
  my_matrix<T, N, K, mul_container<T, Cont, Other_cont, M> > operator*(const my_matrix<T, M, K, Other_cont>& other) const {
    return my_matrix<T, N, K, mul_container<T, Cont, Other_cont, M> > (mul_container<T, Cont, Other_cont, M>(this->data(), other.data()));
  }

  /**
  virtualis szorzas matrix es konstans kozott
  */
  my_matrix<T, N, M, const_mult_container<T, Cont> > operator*(const T other) {
    return my_matrix<T, N, M, const_mult_container<T, Cont> >(const_mult_container<T, Cont>(other, this->data()));
  }

  /**
  virtualis szorzas matrix es konstans kozott (konstans)
  */
  my_matrix<T, N, M, const_mult_container<T, Cont> > operator*(const T other) const {
    return my_matrix<T, N, M, const_mult_container<T, Cont> >(const_mult_container<T, Cont>(other, this->data()));
  }

  /**
  virtualis szorzas konstans es matrix kozott
  */
  friend my_matrix<T, N, M, const_mult_container<T, Cont> > operator*(const T c, const my_matrix<T, N, M, Cont>& mtx) {
    return mtx.operator*(c);
  }
};


template<class T, int N, int M>
void printMtx(const my_matrix<T,N,M> &mtx) {
    for (int i = 0; i < N;i++) {
      for (int j = 0; j < M;j++) {
         std::cout << mtx(i,j) << '\t';
      }
      std::cout << '\n';
    }
}

template<class T, int N, int M>
void fillMtx(my_matrix<T,N,M> &mtx, T a) {
    for (int i = 0; i < N;i++) {
      for (int j = 0; j < M;j++) {
         mtx(i,j) = a;
      }
    }
}

int main(int argc, char const *argv[]) {
 my_matrix<int, 1,2> a;
 my_matrix<int, 2,2> ab;
 my_matrix<int, 2,1> b;
 my_matrix<int, 1,3> c;
 my_matrix<int, 3,1> d;
 my_matrix<int, 1,1> e;

 fillMtx(ab, 5);
 fillMtx(a, 4);
 fillMtx(b, 8);
 e(0,0)= 100;

 a(0,0) = 1;
 a(0,1) = 4;
 b(0,0) = 6;
 b(1,0) = 10;

 ab = ab*2 - ab * b * a * ab;
 printMtx(ab);
 return 0;
}
