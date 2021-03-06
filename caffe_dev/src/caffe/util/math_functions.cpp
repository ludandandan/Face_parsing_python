#include <boost/math/special_functions/next.hpp>
#include <boost/random.hpp>

#include <limits>

#include "caffe/common.hpp"
#include "caffe/util/math_functions.hpp"
#include "caffe/util/rng.hpp"

namespace caffe {

template<>
void caffe_cpu_gemm<float>(const CBLAS_TRANSPOSE TransA,
    const CBLAS_TRANSPOSE TransB, const int M, const int N, const int K,
    const float alpha, const float* A, const float* B, const float beta,
    float* C) {
  int lda = (TransA == CblasNoTrans) ? K : M;
  int ldb = (TransB == CblasNoTrans) ? N : K;
  cblas_sgemm(CblasRowMajor, TransA, TransB, M, N, K, alpha, A, lda, B,
      ldb, beta, C, N);
}

template<>
void caffe_cpu_gemm<double>(const CBLAS_TRANSPOSE TransA,
    const CBLAS_TRANSPOSE TransB, const int M, const int N, const int K,
    const double alpha, const double* A, const double* B, const double beta,
    double* C) {
  int lda = (TransA == CblasNoTrans) ? K : M;
  int ldb = (TransB == CblasNoTrans) ? N : K;
  cblas_dgemm(CblasRowMajor, TransA, TransB, M, N, K, alpha, A, lda, B,
      ldb, beta, C, N);
}

template <>
void caffe_cpu_gemv<float>(const CBLAS_TRANSPOSE TransA, const int M,
    const int N, const float alpha, const float* A, const float* x,
    const float beta, float* y) {
  cblas_sgemv(CblasRowMajor, TransA, M, N, alpha, A, N, x, 1, beta, y, 1);
}

template <>
void caffe_cpu_gemv<double>(const CBLAS_TRANSPOSE TransA, const int M,
    const int N, const double alpha, const double* A, const double* x,
    const double beta, double* y) {
  cblas_dgemv(CblasRowMajor, TransA, M, N, alpha, A, N, x, 1, beta, y, 1);
}

template <>
void caffe_axpy<float>(const int N, const float alpha, const float* X,
    float* Y) { cblas_saxpy(N, alpha, X, 1, Y, 1); }

template <>
void caffe_axpy<double>(const int N, const double alpha, const double* X,
    double* Y) { cblas_daxpy(N, alpha, X, 1, Y, 1); }

template <typename Dtype>
void caffe_set(const int N, const Dtype alpha, Dtype* Y) {
  if (alpha == 0) {
    memset(Y, 0, sizeof(Dtype) * N);  // NOLINT(caffe/alt_fn)
    return;
  }
  for (int i = 0; i < N; ++i) {
    Y[i] = alpha;
  }
}

template void caffe_set<int>(const int N, const int alpha, int* Y);
template void caffe_set<float>(const int N, const float alpha, float* Y);
template void caffe_set<double>(const int N, const double alpha, double* Y);

template <>
void caffe_add_scalar(const int N, const float alpha, float* Y) {
  for (int i = 0; i < N; ++i) {
    Y[i] += alpha;
  }
}

template <>
void caffe_add_scalar(const int N, const double alpha, double* Y) {
  for (int i = 0; i < N; ++i) {
    Y[i] += alpha;
  }
}

template <typename Dtype>
void caffe_copy(const int N, const Dtype* X, Dtype* Y) {
  if (X != Y) {
    if (Caffe::mode() == Caffe::GPU) {
#ifndef CPU_ONLY
      // NOLINT_NEXT_LINE(caffe/alt_fn)
      CUDA_CHECK(cudaMemcpy(Y, X, sizeof(Dtype) * N, cudaMemcpyDefault));
#else
      NO_GPU;
#endif
    } else {
      memcpy(Y, X, sizeof(Dtype) * N);  // NOLINT(caffe/alt_fn)
    }
  }
}

template void caffe_copy<int>(const int N, const int* X, int* Y);
template void caffe_copy<unsigned int>(const int N, const unsigned int* X,
    unsigned int* Y);
template void caffe_copy<float>(const int N, const float* X, float* Y);
template void caffe_copy<double>(const int N, const double* X, double* Y);

template <>
void caffe_scal<float>(const int N, const float alpha, float *X) {
  cblas_sscal(N, alpha, X, 1);
}

template <>
void caffe_scal<double>(const int N, const double alpha, double *X) {
  cblas_dscal(N, alpha, X, 1);
}

template <>
void caffe_cpu_axpby<float>(const int N, const float alpha, const float* X,
                            const float beta, float* Y) {
  cblas_saxpby(N, alpha, X, 1, beta, Y, 1);
}

template <>
void caffe_cpu_axpby<double>(const int N, const double alpha, const double* X,
                             const double beta, double* Y) {
  cblas_daxpby(N, alpha, X, 1, beta, Y, 1);
}

template <>
void caffe_add<float>(const int n, const float* a, const float* b,
    float* y) {
  vsAdd(n, a, b, y);
}

template <>
void caffe_add<double>(const int n, const double* a, const double* b,
    double* y) {
  vdAdd(n, a, b, y);
}

template <>
void caffe_sub<float>(const int n, const float* a, const float* b,
    float* y) {
  vsSub(n, a, b, y);
}

template <>
void caffe_sub<double>(const int n, const double* a, const double* b,
    double* y) {
  vdSub(n, a, b, y);
}

template <>
void caffe_mul<float>(const int n, const float* a, const float* b,
    float* y) {
  vsMul(n, a, b, y);
}

template <>
void caffe_mul<double>(const int n, const double* a, const double* b,
    double* y) {
  vdMul(n, a, b, y);
}

template <>
void caffe_div<float>(const int n, const float* a, const float* b,
    float* y) {
  vsDiv(n, a, b, y);
}

template <>
void caffe_div<double>(const int n, const double* a, const double* b,
    double* y) {
  vdDiv(n, a, b, y);
}

template <>
void caffe_powx<float>(const int n, const float* a, const float b,
    float* y) {
  vsPowx(n, a, b, y);
}

template <>
void caffe_powx<double>(const int n, const double* a, const double b,
    double* y) {
  vdPowx(n, a, b, y);
}

template <>
void caffe_sqr<float>(const int n, const float* a, float* y) {
  vsSqr(n, a, y);
}

template <>
void caffe_sqr<double>(const int n, const double* a, double* y) {
  vdSqr(n, a, y);
}

template <>
void caffe_exp<float>(const int n, const float* a, float* y) {
  vsExp(n, a, y);
}

template <>
void caffe_exp<double>(const int n, const double* a, double* y) {
  vdExp(n, a, y);
}

template <>
void caffe_log<float>(const int n, const float* a, float* y) {
  vsLn(n, a, y);
}

template <>
void caffe_log<double>(const int n, const double* a, double* y) {
  vdLn(n, a, y);
}

template <>
void caffe_abs<float>(const int n, const float* a, float* y) {
    vsAbs(n, a, y);
}

template <>
void caffe_abs<double>(const int n, const double* a, double* y) {
    vdAbs(n, a, y);
}

unsigned int caffe_rng_rand() {
  return (*caffe_rng())();
}

template <typename Dtype>
Dtype caffe_nextafter(const Dtype b) {
  return boost::math::nextafter<Dtype>(
      b, std::numeric_limits<Dtype>::max());
}

template
float caffe_nextafter(const float b);

template
double caffe_nextafter(const double b);

template <typename Dtype>
void caffe_rng_uniform(const int n, const Dtype a, const Dtype b, Dtype* r) {
  CHECK_GE(n, 0);
  CHECK(r);
  CHECK_LE(a, b);
  boost::uniform_real<Dtype> random_distribution(a, caffe_nextafter<Dtype>(b));
  boost::variate_generator<caffe::rng_t*, boost::uniform_real<Dtype> >
      variate_generator(caffe_rng(), random_distribution);
  for (int i = 0; i < n; ++i) {
    r[i] = variate_generator();
  }
}

template
void caffe_rng_uniform<float>(const int n, const float a, const float b,
                              float* r);

template
void caffe_rng_uniform<double>(const int n, const double a, const double b,
                               double* r);

template <typename Dtype>
void caffe_rng_gaussian(const int n, const Dtype a,
                        const Dtype sigma, Dtype* r) {
  CHECK_GE(n, 0);
  CHECK(r);
  CHECK_GT(sigma, 0);
  boost::normal_distribution<Dtype> random_distribution(a, sigma);
  boost::variate_generator<caffe::rng_t*, boost::normal_distribution<Dtype> >
      variate_generator(caffe_rng(), random_distribution);
  for (int i = 0; i < n; ++i) {
    r[i] = variate_generator();
  }
}

template
void caffe_rng_gaussian<float>(const int n, const float mu,
                               const float sigma, float* r);

template
void caffe_rng_gaussian<double>(const int n, const double mu,
                                const double sigma, double* r);

template <typename Dtype>
void caffe_rng_bernoulli(const int n, const Dtype p, int* r) {
  CHECK_GE(n, 0);
  CHECK(r);
  CHECK_GE(p, 0);
  CHECK_LE(p, 1);
  boost::bernoulli_distribution<Dtype> random_distribution(p);
  boost::variate_generator<caffe::rng_t*, boost::bernoulli_distribution<Dtype> >
      variate_generator(caffe_rng(), random_distribution);
  for (int i = 0; i < n; ++i) {
    r[i] = variate_generator();
  }
}

template
void caffe_rng_bernoulli<double>(const int n, const double p, int* r);

template
void caffe_rng_bernoulli<float>(const int n, const float p, int* r);

template <typename Dtype>
void caffe_rng_bernoulli(const int n, const Dtype p, unsigned int* r) {
  CHECK_GE(n, 0);
  CHECK(r);
  CHECK_GE(p, 0);
  CHECK_LE(p, 1);
  boost::bernoulli_distribution<Dtype> random_distribution(p);
  boost::variate_generator<caffe::rng_t*, boost::bernoulli_distribution<Dtype> >
      variate_generator(caffe_rng(), random_distribution);
  for (int i = 0; i < n; ++i) {
    r[i] = static_cast<unsigned int>(variate_generator());
  }
}

template
void caffe_rng_bernoulli<double>(const int n, const double p, unsigned int* r);

template
void caffe_rng_bernoulli<float>(const int n, const float p, unsigned int* r);

template <>
float caffe_cpu_strided_dot<float>(const int n, const float* x, const int incx,
    const float* y, const int incy) {
  return cblas_sdot(n, x, incx, y, incy);
}

template <>
double caffe_cpu_strided_dot<double>(const int n, const double* x,
    const int incx, const double* y, const int incy) {
  return cblas_ddot(n, x, incx, y, incy);
}

template <typename Dtype>
Dtype caffe_cpu_dot(const int n, const Dtype* x, const Dtype* y) {
  return caffe_cpu_strided_dot(n, x, 1, y, 1);
}

template
float caffe_cpu_dot<float>(const int n, const float* x, const float* y);

template
double caffe_cpu_dot<double>(const int n, const double* x, const double* y);

template <>
float caffe_cpu_asum<float>(const int n, const float* x) {
  return cblas_sasum(n, x, 1);
}

template <>
double caffe_cpu_asum<double>(const int n, const double* x) {
  return cblas_dasum(n, x, 1);
}

template <>
void caffe_cpu_scale<float>(const int n, const float alpha, const float *x,
                            float* y) {
  cblas_scopy(n, x, 1, y, 1);
  cblas_sscal(n, alpha, y, 1);
}

template <>
void caffe_cpu_scale<double>(const int n, const double alpha, const double *x,
                             double* y) {
  cblas_dcopy(n, x, 1, y, 1);
  cblas_dscal(n, alpha, y, 1);
}

//add by liangji20040249@gmail.com
template <typename Dtype>
void caffe_cpu_sigmoid_forward(int N, const Dtype* bottom, Dtype* top) {
  for (int i = 0; i < N; ++i) {
    top[i] = 1. / (1. + exp(-bottom[i]));
  }
}
template void caffe_cpu_sigmoid_forward<float>(int N, const float* bottom, float* top);
template void caffe_cpu_sigmoid_forward<double>(int N, const double* bottom, double* top);

template <typename Dtype>
void caffe_cpu_sigmoid_backward(int N, const Dtype* top_data,const Dtype* top_diff, Dtype* bottom_diff) {
  for (int i = 0; i < N; ++i) {
    const Dtype sigmoid_x = top_data[i];
    bottom_diff[i] = top_diff[i] * sigmoid_x * (1. - sigmoid_x);
  }
}
template void caffe_cpu_sigmoid_backward<float>(int N, const float* top_data,const float* top_diff, float* bottom_diff);
template void caffe_cpu_sigmoid_backward<double>(int N, const double* top_data,const double* top_diff, double* bottom_diff);

template <typename Dtype>
void caffe_cpu_relu_forward(int N, const Dtype* bottom, Dtype* top) {
  for (int i = 0; i < N; ++i) {
    top[i] = std::max(bottom[i], Dtype(0));
  }
}
template void caffe_cpu_relu_forward<float>(int N, const float* bottom, float* top);
template void caffe_cpu_relu_forward<double>(int N, const double* bottom, double* top);

template <typename Dtype>
void caffe_cpu_relu_backward(int N, const Dtype* top_data,const Dtype* top_diff, Dtype* bottom_diff) {
  for (int i = 0; i < N; ++i) {
      bottom_diff[i] = top_diff[i] * (top_data[i] > 0);
    }
}
template void caffe_cpu_relu_backward<float>(int N, const float* top_data,const float* top_diff, float* bottom_diff);
template void caffe_cpu_relu_backward<double>(int N, const double* top_data,const double* top_diff, double* bottom_diff);

template <typename Dtype>
void caffe_cpu_tanh_forward(int N, const Dtype* bottom, Dtype* top) {
  for (int i = 0; i < N; ++i) {
    top[i] = tanh(bottom[i]);
  }
}
template void caffe_cpu_tanh_forward<float>(int N, const float* bottom, float* top) ;
template void caffe_cpu_tanh_forward<double>(int N, const double* bottom, double* top) ;


template <typename Dtype>
void caffe_cpu_tanh_backward(int N, const Dtype* top_data,const Dtype* top_diff, Dtype* bottom_diff) {
  Dtype tanhx;
    for (int i = 0; i < N; ++i) {
      tanhx = top_data[i];
      bottom_diff[i] = top_diff[i] * (1 - tanhx * tanhx);
    }
}
template void caffe_cpu_tanh_backward<float>(int N, const float* top_data,const float* top_diff, float* bottom_diff);
template void caffe_cpu_tanh_backward<double>(int N, const double* top_data,const double* top_diff, double* bottom_diff);

template <typename Dtype>
void caffe_cpu_permute(const Dtype * datain, Dtype * dataout, const int * dimensions, const int * order, const int dimsize,const int flipdim)
{  /*
	* implement matlab permute, with additional operation. 
	*1. permutation matrix dimensions 
	*2. flip given dimension
	*liangji, 20150318
   */
	std::vector< int > idx , dim_capacity,dstidx,dstdimensions,dstdim_capacity;
	idx.resize(dimsize,0);
	dim_capacity.resize(dimsize,1);
	dstidx.resize(dimsize,0);
	dstdimensions.resize(dimsize,0);
	dstdim_capacity.resize(dimsize,1);

	for(int i = dimsize-2; i>=0; i-- )
	{
		dim_capacity[i]=1;
		for(int j=i+1;j<dimsize;j++)
			dim_capacity[i] *= dimensions[j];
	}
	for(int i =0; i<dimsize; i++ )
	{
		dstdimensions[i] = dimensions[order[i]];
	}
	for(int i = dimsize-2; i>=0; i-- )
	{
		dstdim_capacity[i]=1;
		for(int j=i+1;j<dimsize;j++)
			dstdim_capacity[i] *= dstdimensions[j];
	}
	int count = dim_capacity[0]* dimensions[0];
	int curidx=0;
	//int n=0;
	
    for(int i=0;i<count;i++)
    {
        int temp=i;
        for(int j=0;j<dimsize;j++)
        {
            dstidx[j]= temp / dstdim_capacity[j];
            temp = temp % dstdim_capacity[j];
        }
        if(flipdim >= 0)
        {
            dstidx[flipdim] = dstdimensions[flipdim] - 1 - dstidx[flipdim] ;
        }
        for(int j=0;j<dimsize;j++)
        {
            idx[order[j]] = dstidx[j];
        }	
        curidx=0;
        for(int j=0;j<dimsize;j++)
        {	
                curidx += idx[j]* dim_capacity[j];
        }
        dataout[i] = datain[curidx];
    }
	return;
}
template void caffe_cpu_permute<float>(const float * datain, float * dataout, const int * dimensions, const int * order, const int dimsize,const int flipdim);
template void caffe_cpu_permute<double>(const double * datain, double * dataout, const int * dimensions, const int * order, const int dimsize,const int flipdim);

template <typename Dtype>
void caffe_bound(const int N, const Dtype* a, const Dtype min, 
    const Dtype max, Dtype* y) {
  for (int i = 0; i < N; ++i) {
    y[i] = std::min(std::max(a[i], min), max);
  }
}
template void caffe_bound(const int N, const float* a, const float min, const float max, float* y);
template void caffe_bound(const int N, const double* a, const double min, const double max, double* y);

template <>
void caffe_scalar_max(const int N, const float alpha, const float* b, float* y) {
  for (int i = 0; i < N; ++i) {
	y[i] = alpha *(alpha > b[i]) + b[i]*(alpha <= b[i]);
  }
}

template <>
void caffe_scalar_max(const int N, const double alpha, const double* b, double* y) {
  for (int i = 0; i < N; ++i) {
    y[i] = alpha *(alpha > b[i]) + b[i]*(alpha <= b[i]);
  }
}

template <>
void caffe_scalar_min(const int N, const float alpha, const float* b, float* y) {
  for (int i = 0; i < N; ++i) {
	y[i] = alpha *(alpha < b[i]) + b[i]*(alpha >= b[i]);
  }
}

template <>
void caffe_scalar_min(const int N, const double alpha, const double* b, double* y) {
  for (int i = 0; i < N; ++i) {
	  y[i] = alpha *(alpha < b[i]) + b[i]*(alpha >= b[i]);
  }
}

template <typename Dtype>
void Get_dist_matrix_cpu(const Dtype * inputdata, const int feaNum, const int feaDim, Dtype * distmatrix)
{//return eculidean distance martix, liangji20040249@gmail.com, 20160426
	
	Dtype dist=0;
	std::vector<Dtype> temp(feaDim,0);
	Dtype* t = &temp[0];

	for(int r=0;r<feaNum;r++)
	{
		for(int c=0;c<feaNum;c++)
		{
			if(r==c)
			{
				distmatrix[r * feaNum + c]=0;
			}
			else
			{
				caffe_sub(feaDim, inputdata + r*feaDim, inputdata + c * feaDim,t);
				dist = caffe_cpu_dot(feaDim, t, t);
				distmatrix[r * feaNum + c]=dist;
				distmatrix[c * feaNum + r]=dist;
			}
		}
	}
}
template void Get_dist_matrix_cpu<float>(const float * inputdata, const int feaNum, const int feaDim, float * distmatrix);
template void Get_dist_matrix_cpu<double>(const double * inputdata, const int feaNum, const int feaDim, double * distmatrix);

}  // namespace caffe









