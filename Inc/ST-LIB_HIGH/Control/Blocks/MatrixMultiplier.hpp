#pragma once
#include "../ControlBlock.hpp"

template<size_t U, size_t V, size_t W> class MatrixMultiplier;

template<size_t U, size_t V, size_t W> requires (U > 1 && V > 1 && W > 1)
class MatrixMultiplier<U,V,W> : public ControlBlock<double(*)[U][V], double(*)[U][W]>{
	double(&A)[U][V], (&B)[V][W], (&result)[U][W];
public:
	MatrixMultiplier(double(&A)[U][V], double(&B)[V][W], double(&result)[U][W]) :  A(A), B(B), result(result){
        this->input_value = &A;
        this->output_value = &result;
	}

	void execute() override{
	    for (int i = 0; i < U; i++) {
	        for (int j = 0; j < W; j++) {
	            result[i][j] = 0;
	            for (int k = 0; k < V; k++)
	                result[i][j] += A[i][k] * B[k][j];
	        }
	    }
	}
};

//CTAD
#if __cpp_deduction_guides >= 201606
template<size_t U, size_t V, size_t W> requires (U > 1 && V > 1 && W > 1)
MatrixMultiplier(double(&)[U][V], double(&)[V][W], double(&)[U][W])->MatrixMultiplier<U,V,W>;
#endif

template<size_t U, size_t V>
class MatrixMultiplier<U,V,1> : public ControlBlock<double(*)[U][V], double(*)[U]>{
    double(&A)[U][V], (&B)[V], (&result)[U];
public:
    MatrixMultiplier(double(&A)[U][V], double(&B)[V], double(&result)[U]) :  A(A), B(B), result(result){
        this->input_value = &A;
        this->output_value = &result;
    }

    void execute() override{
        for (int i = 0; i < U; i++) {
            result[i] = 0;
            for (int k = 0; k < V; k++)
                result[i] += A[i][k] * B[k];
        }
    }
};

//CTAD
#if __cpp_deduction_guides >= 201606
template<size_t U, size_t V> requires (U > 1 && V > 1)
MatrixMultiplier(double(&)[U][V], double(&)[V], double(&)[U])->MatrixMultiplier<U,V,1>;
#endif

template<size_t U, size_t V>
class MatrixMultiplier<1,U,V> : public ControlBlock<double(*)[U], double(*)[V]>{
    double(&A)[U], (&B)[U][V], (&result)[V];
public:
    MatrixMultiplier(double(&A)[U], double(&B)[U][V], double(&result)[V]) :  A(A), B(B), result(result){
        this->input_value = &A;
        this->output_value = &result;
    }

    void execute() override{
        for (int j = 0; j < V; j++) {
            result[j] = 0;
            for (int k = 0; k < U; k++)
                result[j] += A[k] * B[k][j];
        }
    }
};

//CTAD
#if __cpp_deduction_guides >= 201606
template<size_t U, size_t V> requires (U > 1 && V > 1)
MatrixMultiplier(double(&)[U], double(&)[U][V], double(&)[V])->MatrixMultiplier<1,U,V>;
#endif

template<size_t U>
class MatrixMultiplier<1,U,1> : public ControlBlock<double(*)[U], double(*)>{
    double(&A)[U], (&B)[U], (&result);
public:
    MatrixMultiplier(double(&A)[U], double(&B)[U], double(&result)) :  A(A), B(B), result(result){
        this->input_value = &A;
        this->output_value = &result;
    }

    void execute() override{
        result = 0;
        for (int k = 0; k < U; k++)
            result += A[k] * B[k];
    }
};

//CTAD
#if __cpp_deduction_guides >= 201606
template<size_t U> requires (U > 1)
MatrixMultiplier(double(&)[U], double(&)[U], double(&))->MatrixMultiplier<1,U,1>;
#endif
