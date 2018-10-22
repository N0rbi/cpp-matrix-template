#include <iostream>

using namespace std;


template<class T, int N, int M>
//class my_matrix : matrix_base {
class my_matrix  {
    T elems[N][M];
public:
    /** A sablonpéldány rendelkezik default konstruktorral. */
    //my_matrix();
    my_matrix() {
        for (size_t i=0; i<N; i++) {
            for (size_t j=0; j<M; j++) {
                elems[i][j] = 0;
            }
        }
    }

    /** 
     * A mátrix elemeihez való hozzáférést a kétparaméteres függvényhívás 
     * operátor felüldefiniálása biztosítja.
     */
    const T& operator()(int n, int m) const {
        return elems[n][m];
    }

    T& operator()(int n, int m) {
        return elems[n][m];
    }

    my_matrix<T, N, M> operator+(const my_matrix<T, N, M> other) {
        my_matrix<T, N, M> sum;
        for (size_t i = 0; i < N; i++) {
            for (size_t j = 0; j < M; j++) {
                sum(i,j) = (*this)(i,j) + other(i,j);
            }
        }
        return sum;
    }

    my_matrix<T, N, M> operator-(const my_matrix<T, N, M> other) {
        my_matrix<T, N, M> sum;
        for (size_t i = 0; i < N; i++) {
            for (size_t j = 0; j < M; j++) {
                sum(i,j) = (*this)(i,j) - other(i,j);
            }
        }
        return sum;
    }

    template<int K>
    my_matrix<T, N, K> operator*(const my_matrix<T, M, K> other) {
        my_matrix<T, N, K> prod;
        for (size_t j=0; j<M; j++) {
            for (size_t i=0; i<N; i++) {
                for (size_t k=0; k<K; k++) {
                    prod(i,k) += (*this)(i,j)*other(j,k);
                }
            }
        }
        return prod;
    }

    my_matrix<T, N, M> operator*(const T scalar) {
        my_matrix<T, N, M> prod;
        for (size_t i = 0; i < N; i++) {
            for (size_t j = 0; j < M; j++) {
                prod(i,j) = (*this)(i,j) * scalar;
            }
        }
        return prod;
    }
};

int main() {
    my_matrix<int,2,2> mtx1;
    my_matrix<int,2,2> mtx2;
    my_matrix<int,2,2> mtx3;
    my_matrix<int,2,3> mtx4;
    my_matrix<int,2,3> mtx5;
    
    for (size_t i=0; i<2; i++) {
        for (size_t j=0; j<2; j++) {
            mtx1(i,j) = 1;
            mtx2(i,j) = 1;
            mtx3(i,j) = 1;
        }
    }

    mtx1 = mtx1*3;

    for (size_t i=0; i<2; i++) {
        for (size_t j=0; j<3; j++) {
            mtx5(i,j) = 1;
        }
    }

    mtx4 = mtx1 * mtx5;
    
    for (size_t i=0; i<2; i++) {
        for (size_t j=0; j<3; j++) {
            cout << mtx4(i,j) << endl;
        }
    }
    
    return 0;
}