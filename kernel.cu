#include <cuda.h>
#include <cuda_runtime.h>
#include <cuda_gl_interop.h>

#include <GL/glut.h>

#include "global.h"

#define blockWidth 8
#define blockHeight 8

//extern double xmin, xmax;
//extern double ymin, ymax;
//extern int XSize, YSize;
//extern const int MAX_ITER_NUM;

//extern ParameterStruct paras;


__global__ void kernel(uchar4 * pbo, uchar4 * palette, ParameterStruct paras)
{
	int indx = (blockIdx.x*blockDim.x) + threadIdx.x;
	int indy = (blockIdx.y*blockDim.y) + threadIdx.y;
	
	if (indx >= paras.xSize || indy >= paras.ySize)
		return;

	int index = indx * paras.xSize + indy;

	pbo[index] = palette[index];
}


//extern const int blockWidth = 8;
//extern const int blockHeight = 8;
//extern const int numBlocksWidth = paras.xSize/blockWidth;
//extern const int numBlocksHeight = paras.ySize/blockHeight;
// Declare the cuda dimension parameters
//extern dim3 blockSize(blockWidth, blockHeight);
//extern dim3 numBlocks(numBlocksWidth, numBlocksHeight);

extern "C" void run_kernel(uchar4 *pbo, uchar4 * palette, ParameterStruct paras)
{
	uchar4 * d_palette;
	cudaMalloc(&d_palette, sizeof(uchar4)*(paras.maxIterNum+1)); 
	cudaMemcpy(d_palette, palette, sizeof(uchar4)*(paras.maxIterNum+1), cudaMemcpyHostToDevice);

	const int numBlocksWidth = paras.xSize/blockWidth;
	const int numBlocksHeight = paras.ySize/blockHeight;
	dim3 blockSize(blockWidth, blockHeight);
	dim3 numBlocks(numBlocksWidth, numBlocksHeight);

	// Call the kernel
	kernel<<<numBlocks,blockSize>>>(pbo, d_palette, paras);
	cudaFree(d_palette);
}

extern "C" void runCuda(struct cudaGraphicsResource **pbo_resource, uchar4 *palette, ParameterStruct paras)
{
    // map OpenGL buffer object for writing from CUDA
    float4 *dptr;
    cudaGraphicsMapResources(1, pbo_resource, 0);
    size_t num_bytes;
    cudaGraphicsResourceGetMappedPointer((void **)&dptr, &num_bytes,
                                                         *pbo_resource);
    //printf("CUDA mapped VBO: May access %ld bytes\n", num_bytes);

    // execute the kernel
    //    dim3 block(8, 8, 1);
    //    dim3 grid(mesh_width / block.x, mesh_height / block.y, 1);
    //    kernel<<< grid, block>>>(dptr, mesh_width, mesh_height, g_fAnim);

    run_kernel((uchar4*)dptr, palette, paras);

    // unmap buffer object
    cudaGraphicsUnmapResources(1, pbo_resource, 0);
}

extern "C" void createPBO(GLuint *pbo, struct cudaGraphicsResource **pbo_res, unsigned int pbo_res_flags, ParameterStruct paras)
{
    //assert(pbo);

    // create buffer object
    glGenBuffers(1, pbo);
    glBindBuffer(GL_ARRAY_BUFFER, *pbo);

    // initialize buffer object
    unsigned int size = paras.xSize * paras.ySize * 4 * sizeof(float);
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // register this buffer object with CUDA
    cudaGraphicsGLRegisterBuffer(pbo_res, *pbo, pbo_res_flags);

    SDK_CHECK_ERROR_GL();
}
