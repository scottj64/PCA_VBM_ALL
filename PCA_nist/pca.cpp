#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <set>
#include <map>
#include <assert.h>

#include <stdio.h>
#include <stdlib.h>

#include "tnt_array1d.h"
#include "tnt_array2d.h"

#include "jama_eig.h"


using namespace std;
using namespace TNT;
using namespace JAMA;

namespace PCA {
	bool debug = false;
	
	template < class T>

	void adjust_data(Array2D<double>& d, Array1D<double>& means) {
	   for (int i=0; i<d.dim2(); ++i) { 
		   double mean = 0;
		   for (int j=0; j<d.dim1(); ++j) {
			   mean += d[j][i];
		   }

		   mean /= d.dim1();

		   // store the mean
		   means[i] = mean;

		   // subtract the mean
		   for (int j=0; j<d.dim1(); ++j) {
			   d[j][i] -= mean;
		   }
	   }
	}

	double compute_covariance(const Array2D<double>& d, int i, int j) {
	   double cov = 0;
	   for (int k=0; k<d.dim1(); ++k) {
		   cov += d[k][i] * d[k][j];
	   }

	   return cov / (d.dim1() - 1);
	}

	void compute_covariance_matrix(const Array2D<double> & d, Array2D<double> & covar_matrix) {
		int dim = d.dim2();
		assert(dim == covar_matrix.dim1());
		assert(dim == covar_matrix.dim2());
		for (int i=0; i<dim; ++i) {
			for (int j=i; j<dim; ++j) {
				covar_matrix[i][j] = compute_covariance(d, i, j);
			}
		}


		// fill the Left triangular matrix
		for (int i=1; i<dim; i++) {
			for (int j=0; j<i; ++j) {
				covar_matrix[i][j] = covar_matrix[j][i];
			}
		}

	}

	// Calculate the eigenvectors and eigenvalues of the covariance
	// matrix
	void eigen(const Array2D<double> & covar_matrix, Array2D<double>& eigenvector, Array2D<double>& eigenvalue) {
		Eigenvalue<double> eig(covar_matrix);
		eig.getV(eigenvector);
		eig.getD(eigenvalue);
	}
}

int main(int argc, char* argv[]) {
	if (argc != 2) {		
		cout << "Usage: " << argv[0] << " data_file" << endl;		
		return -1;   
	}
	using namespace PCA;
	
//	const int row = 10;
//	const int col = 2;
	
	const int row = 1399;
	const int col = 15;

	Array2D<double> d(row, col);
	load_data_from_file(d, argv[1]);
	Array1D<double> means(col);
	adjust_data(d, means);

	Array2D<double> covar_matrix(col, col);
	compute_covariance_matrix(d, covar_matrix);
/*	cout << "The covariance matrix:" << endl;
//	cout << eigenvector[1][1] << endl;
	for (int i=0; i < col; i++)
	{
		for (int j=0; j < col; j++)
		{
			cout << covar_matrix[i][j] << " ";
		}
		cout << endl;
	}
*/	

	int dim = covar_matrix.dim1();
	// get the eigenvectors
	Array2D<double> eigenvector(dim, dim);

	// get the eigenvalues
	Array2D<double> eigenvalue(dim, dim);
	eigen(covar_matrix, eigenvector, eigenvalue);
//	cout << "The eigenvectors:" << endl;
//	cout << eigenvector[1][1] << endl;
//	for (int i=0; i < dim; i++)
	for (int i=dim-1; i >=0; i--)
	{
		for (int j=0; j < dim; j++)
		{
			cout << eigenvector[j][i] << "\t";
		}
		cout << eigenvalue[i][i] << endl;
	}
	
//	cout << endl;
	
//	cout << "The eigenvalues:" << endl;
//	cout << eigenvalue[1][1] << endl;
//	for (int i=dim-1; i >=0; i--)
//	{
//		for (int j=0; j < dim; j++)
//		{
//			cout << eigenvalue[i][j] << " ";
//		}
//		cout << eigenvalue[i][i] << endl;
//	}
/*
	// restore the old data
	// final_data = RowFeatureVector * RowDataAdjust
	Array2D<double> final_data(row, col);
	Array2D<double> transpose_data(col, row);
	transpose(d, transpose_data);
	multiply(eigenvector, transpose_data, final_data);

	cout << "the final data" << endl;
//	cout << final_data[1][1] << endl;
	for (int i=0; i < row; i++)
	{
		for (int j=0; j < col; j++)
		{
			cout << final_data[i][j] << " ";
		}
		cout << endl;
	}
*/
	return 0;
}
