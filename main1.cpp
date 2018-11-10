using namespace std;

/**
A simple container tartalmazza a memoriaba lementett matrix ertekeket.
*/
template<class T, size_t N, size_t M>
struct simple_container {
    T items[N][M];

    /**
    visszaadunk egy konstans referenciat a tarolo (i,j). elemere
    */
    T operator()(const size_t i,  const size_t j) const{
      return items[i][j];
    }

    /**
    visszaadunk egy referenciat a tarolo (i,j). elemere, ekkor valtoztathato
    az erteke
    */
    T& operator()(const size_t i, const size_t j){
      return items[i][j];
    }
};

/**
const_mult_container letrehoz egy virtualis diagonalis matrixot,
hogy a konstanst matrixszorzas segitsegevel szorozhassuk.
*/
template<class T>
struct const_mult_container {
    T value;

    const_mult_container(T val) : value(val){};

    /**
    csak akkor allitjuk az erteket 'value'-re, ha foatlobeli elemrol van szo
    */
    T operator()(const size_t i, size_t j) const{
      return (i == j) ? value : 0;
    }
};

// template<class T>
// struct const_sum_container {
//     T value;
//
//     const_sum_container(T val) : value(val){};
//
//     T operator()(const size_t i, size_t j) const{
//       return value;
//     }
// };

/**
sum_container ket azonos metertu matrixot var, virtualisan osszeadva oket.
*/
template<class T, class EXPR1, class EXPR2>
struct sum_container {
  EXPR1 expr1;
  EXPR2 expr2;

  sum_container(const EXPR1& a, const EXPR2& b): expr1(a), expr2(b){}

  /**
  a ket parameterul kapott matrix (i,j). elem osszeget adja vissza
  */
  T operator()(const size_t i, const size_t j) const{
    return expr1(i,j) + expr2(i,j);
  }
};

/**
neg_container hasonló a sum_container-hez, csak kivonassal
*/
template<class T, class EXPR1, class EXPR2>
struct neg_container {
  EXPR1 expr1;
  EXPR2 expr2;

  neg_container(const EXPR1& a, const EXPR2& b): expr1(a), expr2(b){}

  /**
  az (i,j) parameterre visszaadja a ket parameterul kapott matrix
  (i,j). elemeinek a kulonbseget
  */
  T operator()(const size_t i, const size_t j) const{
    return expr1(i,j) - expr2(i,j);
  }
};

/**
mul_container egy N*M es M*K matrixok virtualis osszeszorzasara alkalmas
*/
template<class T, class EXPR1, class EXPR2, int M>
struct mul_container {
  EXPR1 expr1;
  EXPR2 expr2;

  mul_container(const EXPR1& a, const EXPR2& b): expr1(a), expr2(b){ }
  /**
  rekurziv fuggveny matrix szorzasra, az (i,k). elem megadja a definicio
  szerinti erteket a ket matrix szorzatanak
  a rekurzio alapja a ket matrix "kapcsolodasi dimenzioja"
  */
  T operator()(const size_t i, const size_t k) const{
    return (expr1(i,M-1) * expr2(M-1,k)) + (mul_container<T, EXPR1, EXPR2, M-1>(expr1, expr2))(i,k);
  }
};

/**
a mul_container specializacioja, ha a "kapcsolodo dimenzio" 0 akkor az (i, k).
elem sem lehet kulonbozo 0-nal.
*/
template<class T,  class EXPR1, class EXPR2>
struct mul_container <T, EXPR1, EXPR2, 0> {

  mul_container(const EXPR1& a, const EXPR2& b){}

  /**
  leszallasi feltetel matrix szorzashoz
  */
  T operator()(const size_t i, const size_t k) const{
    return 0;
  }
};

/**
my_matrix az alap matrix osztaly, mely a fent talalhato segely osztalyokat
hasznalja fel kontenerkent
*/
template<class T, size_t N, size_t M, class Cont= simple_container<T, N, M> >
class my_matrix {
  Cont cont;

public:
  /**
  alapertelemezett konstruktor
  */
  my_matrix() {}
  /**
  konstruktor virtualizalt elemekhez
  */
  my_matrix(const Cont& other) : cont(other){}

  /**
  ertekadas operator a simple_container-ekkel elvegzi a virtualis szamitasokat,
  egeszen az adott "other" objektum kontenereig, majd beallirja ertekul az
  aktualis objektumnak
  */
  template<class T2, class Cont2>
  my_matrix& operator=(const my_matrix<T2, N, M, Cont2>& other){
    for (size_t i = 0; i < N; ++i)
      for (size_t j = 0; j < M; j++)
      {
        cont(i,j) = other(i,j);
      }
    return *this;
  }

  /**
  (i,j). elem ertek szerinti lekerese
  */
  T operator()(const size_t i, const size_t j) const{
    return cont(i,j);
  }

  /**
  (i,j). elem referencia szerinti lekerese
  */
  T& operator()(const size_t i, const size_t j) {
    return cont(i,j);
  }

  /**
  visszaadja a kontener konstans referenciajat
  */
  const Cont& data() const{
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
  template<size_t K, class Other_cont>
  my_matrix<T, N, K, mul_container<T, Cont, Other_cont, M> > operator*(const my_matrix<T, M, K, Other_cont>& other){
    return my_matrix<T, N, K, mul_container<T, Cont, Other_cont, M> > (mul_container<T, Cont, Other_cont, M>(this->data(), other.data()));
  }

  /**
  virtualis szorzas matrixok kozott (konstans)
  */
  template<size_t K, class Other_cont>
  my_matrix<T, N, K, mul_container<T, Cont, Other_cont, M> > operator*(const my_matrix<T, M, K, Other_cont>& other) const {
    return my_matrix<T, N, K, mul_container<T, Cont, Other_cont, M> > (mul_container<T, Cont, Other_cont, M>(this->data(), other.data()));
  }

  /**
  virtualis szorzas matrix es konstans kozott
  */
  my_matrix<T, N, M, mul_container<T, Cont, const_mult_container<T>, M> > operator*(const T other){
    return this->operator*(my_matrix<T, N, M, const_mult_container<T> >(const_mult_container<T>(other)));
  }

  /**
  virtualis szorzas matrix es konstans kozott (konstans)
  */
  my_matrix<T, N, M, mul_container<T, Cont, const_mult_container<T>, M> > operator*(const T other) const {
    return this->operator*(my_matrix<T, N, M, const_mult_container<T> >(const_mult_container<T>(other)));
  }

  /**
  virtualis szorzas konstans es matrix kozott
  */
  friend my_matrix<T, N, M, mul_container<T, Cont, const_mult_container<T>, M> > operator*(const T c, const my_matrix<T, N, M, Cont>& mtx) {
    return mtx.operator*(c);
  }
};
