#ifndef _DSPF_SP_fftSPxSP_CN_H_
#define _DSPF_SP_fftSPxSP_CN_H_

void DSPF_sp_fftSPxSP_cn (int N, float *ptr_x, float *ptr_w, float *ptr_y,
    int n_min, int offset, int n_max);
    
void tw_gen (float *w, int n);

#endif /* _DSPF_SP_fftSPxSP_CN_H_ */